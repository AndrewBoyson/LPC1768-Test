#include "tls.h"
#include "tls-defs.h"
#include "tls-connection.h"
#include "tls-session.h"
#include "tls-log.h"
#include "tls-prf.h"
#include "crypto/pki/ser-cer.h"
#include "crypto/pki/pri-key.h"
#include "log/log.h"
#include "crypto/tls/tls-aes128cbc-sha/tls-aes128cbc-sha.h"
#include "crypto/tls/tls-aes128cbc-sha/tls-mac.h"
#include "lpc1768/random/random.h"
#include "web/http/http.h"

void backfillSize(uint8_t* pCurrent, uint8_t* pStart)
{
    int size = pCurrent - pStart - 2;
    *(pStart + 0) = size >> 8;
    *(pStart + 1) = size & 0xFF;
}
void addSize(uint8_t** pp, int size)
{
    uint8_t* p = *pp;
    *p++ = size >> 8;
    *p++ = size & 0xFF;
    *pp = p;
}
static uint8_t* pHandshakeSize;
static uint8_t* pHandshakePayload;   
static void addHandshakeStart(uint8_t** pp)
{
    uint8_t* p = *pp;
    
    *p++ = TLS_CONTENT_TYPE_HANDSHAKE;
    *p++ = 0x03; *p++ = 0x03;
    pHandshakeSize = p;       //Store the position to backfill the handshake size
    p += 2;                   //Leave room to backfill the handshake size
    pHandshakePayload = p;    //Record the position of the handshake payload to later calculate the hash
    
    *pp = p;
}
static void addHandshakeEnd(uint8_t* p, struct TlsConnection* pConnection)
{
    backfillSize(p, pHandshakeSize);
    Sha256Add(&pConnection->handshakeSha, pHandshakePayload, p - pHandshakePayload); //Add the handshake hash
    pConnection->serverSequence++;
}
static void addHandshakeServerHello(uint8_t** pp, struct TlsConnection* pConnection)
{
    if (TlsTrace) Log("     sending handshake server hello\r\n");
    uint8_t* p = *pp;
    
    *p++ = TLS_HANDSHAKE_SERVER_HELLO;
    *p++ = 0x00;
    uint8_t* pSize = p;
    p += 2;
    *p++ = 0x03; *p++ = 0x03;
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++)
    {
        uint8_t r = RandomGetByte();
        pConnection->serverRandom[i] = r;
        *p++ = r;                         //32 bit random number
    }
    *p++ = 0x04;                          //SessionId length 4
    *p++ = pConnection->sessionId >> 24;  //Session id
    *p++ = pConnection->sessionId >> 16;  //Session id
    *p++ = pConnection->sessionId >>  8;  //Session id
    *p++ = pConnection->sessionId >>  0;  //Session id
    *p++ = 0x00; *p++ = 0x2f;             //Cipher Suite: TLS_RSA_WITH_AES_128_CBC_SHA (0x002f)
    *p++ = 0x00;                          //Compression method none
    *p++ = 0x00; *p++ = 0x05;             //Extensions length (2 bytes) 5 bytes
    *p++ = 0xff; *p++ = 0x01;             //Extension Renegotiation Info
    *p++ = 0x00; *p++ = 0x01;             //1 bytes of "Renegotiation Info" extension data follows
    *p++ = 0x00;                          //length is zero, because this is a new connection 
    backfillSize(p, pSize);
    
    *pp = p;
}
static void addHandshakeCertificate(uint8_t** pp)
{
    uint8_t* p = *pp;
    
    *p++ = TLS_HANDSHAKE_CERTIFICATE; *p++ = 0x00;
    addSize(&p, SerCerSize + 6);      *p++ = 0x00;             //Size of this handshake
    addSize(&p, SerCerSize + 3);      *p++ = 0x00;             //Size of all certificates
    addSize(&p, SerCerSize    );                               //Size of first certificate
    for (int i = 0; i < SerCerSize; i++) *p++ = SerCerData[i]; //Certificate
    
    *pp = p;
}
static void addHandshakeServerHelloDone(uint8_t** pp)
{   
    if (TlsTrace) LogTime("     sending handshake server hello done\r\n");
    uint8_t* p = *pp;
    
    *p++ = TLS_HANDSHAKE_SERVER_HELLO_DONE; *p++ = 0x00;
    addSize(&p, 0);
    
    *pp = p;
}
static void addHandshakeFinished(uint8_t** pp, struct TlsConnection* pConnection, struct TlsSession* pSession)
{
    if (TlsTrace) LogTime("     sending handshake finished\r\n");
    uint8_t* p = *pp;
    
    TlsAes128CbcSha1EncryptStart(&p);
    
    //Make the 'finished' handshake which is part of the payload to be encrypted
    *p++ = TLS_HANDSHAKE_FINISHED;
    *p++ = 0x00;
    *p++ = 0x00;
    *p++ = TLS_LENGTH_VERIFY; //Length 12
    
    //Hash over all handshake payloads exchanged so far
    uint8_t hash[SHA256_HASH_SIZE];
    Sha256Finish(&pConnection->handshakeSha, hash);
    
    //Make verify data
    TlsPrfServerFinished(pSession->masterSecret, hash, p);    //Hash over all handshakes
    p += TLS_LENGTH_VERIFY;
    
    TlsAes128CbcSha1EncryptEnd(&p, pConnection, TLS_CONTENT_TYPE_HANDSHAKE);
    
    *pp = p;
}
static void addChangeCipher(uint8_t** pp, struct TlsConnection* pConnection)
{
    if (TlsTrace) LogTime("     sending change cipher\r\n");
    uint8_t* p = *pp;
    
    *p++ = TLS_CONTENT_TYPE_CHANGE_CIPHER;             //Content is change cipher
    *p++ = 0x03; *p++ = 0x03;                          //Legacy TLS version
    *p++ = 0x00; *p++ = 0x01;                          //Change cipher Length (2 bytes)
    *p++ = 0x01;                                       //Change cipher message 1
    
    //Record that all outgoing messages are now encrypted
    pConnection->serverEncrypted = true;
    pConnection->serverSequence = 0;
   
    *pp = p;
}
static void addAlert(uint8_t** pp, struct TlsConnection* pConnection, uint8_t level, uint8_t description)
{
    if (TlsTrace) LogTime("     sending alert\r\n");
    Log    ("     - "); TlsLogAlertLevel(level); Log(": "); TlsLogAlertDescription(description); Log("\r\n");
    
    uint8_t* p = *pp;
    
    *p++ = TLS_CONTENT_TYPE_ALERT;
    *p++ = 0x03; *p++ = 0x03;
    addSize(&p, 2);
    *p++ = level;
    *p++ = description;
    
    pConnection->serverSequence++;
    
    *pp = p;
}
static void sendServerHelloNew(struct TlsConnection* pConnection, struct TlsSession* pSession, int* pWindowSize, uint8_t* pWindow, uint32_t positionOfWindowInStream)
{
    uint8_t* p = pWindow;
    
    addHandshakeStart          (&p);
    addHandshakeServerHello    (&p, pConnection);
    addHandshakeCertificate    (&p);
    addHandshakeServerHelloDone(&p);
    addHandshakeEnd            ( p, pConnection);
         
    *pWindowSize = p - pWindow;
    pConnection->serverPositionInStreamOffset = positionOfWindowInStream + *pWindowSize;
}
static void sendServerHelloResume(struct TlsConnection* pConnection, struct TlsSession* pSession, int* pWindowSize, uint8_t* pWindow, uint32_t positionOfWindowInStream)
{
    uint8_t* p = pWindow;
    
    addHandshakeStart      (&p);
    addHandshakeServerHello(&p, pConnection);
    addHandshakeEnd        ( p, pConnection);
    
    TlsPrfKeysAes128Sha1(pSession->masterSecret, pConnection->clientRandom, pConnection->serverRandom, pConnection->clientMacKey,
                                                                                                       pConnection->serverMacKey,
                                                                                                       pConnection->clientWriteKey,
                                                                                                       pConnection->serverWriteKey);

    addChangeCipher        (&p, pConnection);
    
    addHandshakeStart      (&p);
    addHandshakeFinished   (&p, pConnection, pSession);
    addHandshakeEnd        ( p, pConnection);
    
    *pWindowSize = p - pWindow;
    pConnection->serverPositionInStreamOffset = positionOfWindowInStream + *pWindowSize;
}
static void sendServerChange(struct TlsConnection* pConnection, struct TlsSession* pSession, int* pWindowSize, uint8_t* pWindow, uint32_t positionOfWindowInStream)
{
    uint8_t* p = pWindow;

    addChangeCipher(&p, pConnection);
    
    addHandshakeStart   (&p);
    addHandshakeFinished(&p, pConnection, pSession);
    addHandshakeEnd     ( p, pConnection);
    
    *pWindowSize = p - pWindow;
    pConnection->serverPositionInStreamOffset = positionOfWindowInStream + *pWindowSize;
}
static void sendFatal(uint8_t description, struct TlsConnection* pConnection, int* pWindowSize, uint8_t* pWindow, uint32_t positionOfWindowInStream)
{
    uint8_t* p = pWindow;
    
    addAlert(&p, pConnection, TLS_ALERT_FATAL, description);
    
    *pWindowSize = p - pWindow;
    pConnection->serverPositionInStreamOffset = positionOfWindowInStream + *pWindowSize;
}
static bool sendContent(struct TlsConnection* pConnection, int* pWindowSize, uint8_t* pWindow, uint32_t positionOfWindowInStream)
{
    //Start
    if (TlsTrace)
    {
        LogTime("     adding application content\r\n");
        LogF("- available window size %d\r\n", *pWindowSize);
        LogF("- position of window in stream %d\r\n", positionOfWindowInStream);
    }
    uint8_t* p = pWindow;
    *p++ = TLS_CONTENT_TYPE_APPLICATION;
    *p++ = 0x03; *p++ = 0x03;
    
    //Prepare a place to backfill the size
    uint8_t* pBackfillSize = p;
    *p++ = 0; *p++ = 0;
    
    TlsAes128CbcSha1EncryptStart(&p);

    //Add the plain payload
    int payloadSize = *pWindowSize - 5 - TLS_AES_128_CBC_SHA1_MAX_OVERHEAD;
    if (TlsTrace) LogF("- available payload size %d\r\n", payloadSize);
    uint32_t positionOfPayloadInStream = positionOfWindowInStream - pConnection->serverPositionInStreamOffset;
    if (TlsTrace) LogF("- position of payload in stream %d\r\n", positionOfPayloadInStream);
    bool finished = HttpAdd(pConnection->id, &payloadSize, (char*)p, positionOfPayloadInStream); //Return whatever HTTP would be
    if (TlsTrace) LogF("- resulting payload size %d\r\n", payloadSize);
    p += payloadSize;
    
    TlsAes128CbcSha1EncryptEnd(&p, pConnection, TLS_CONTENT_TYPE_APPLICATION);
    
    //Backfill the size
    backfillSize(p, pBackfillSize);
    
    //Finalise
    pConnection->serverSequence++;
    *pWindowSize = p - pWindow;
    if (TlsTrace) LogF("- resulting window size %d\r\n", *pWindowSize);
    pConnection->serverPositionInStreamOffset += *pWindowSize - payloadSize;
    
    return finished;
}
bool TlsResponse(int connectionId, bool clientFinished, int* pWindowSize, uint8_t* pWindow, uint32_t positionOfWindowInStream)
{   
    struct TlsConnection* pConnection =  TlsConnectionOrNull(connectionId);
    if (!pConnection)
    {
        *pWindowSize = 0;
        return clientFinished;     //Ignore empty connections in poll
    }
    
    if (!pConnection->sessionId)
    {
        *pWindowSize = 0;
        return clientFinished;     //Ignore empty sessions in poll
    }
    
    struct TlsSession* pSession = TlsSessionOrNull(pConnection->sessionId);
    if (!pSession)
    {
        pConnection->toDo = DO_SEND_ALERT_INTERNAL_ERROR;  //Abort invalid sessions
        LogTimeF("TlsPoll - invalid session %u - sending internal error\r\n", pConnection->sessionId);
    }
    
    switch (pConnection->toDo)
    {
        case DO_WAIT_CLIENT_HELLO:
        case DO_WAIT_CLIENT_CHANGE:
        case DO_WAIT_DECRYPT_MASTER_SECRET:
            *pWindowSize = 0;
            if (clientFinished) return true;  //The client hasn't made a request and never will so finish
            else                return false; //The client hasn't made a request yet but it could.
            
         case DO_SEND_SERVER_HELLO_NEW:
            sendServerHelloNew(pConnection, pSession, pWindowSize, pWindow, positionOfWindowInStream);
            pConnection->toDo = DO_WAIT_CLIENT_CHANGE;
            return false;                     //Not finished
            
         case DO_SEND_SERVER_HELLO_RESUME:
            sendServerHelloResume(pConnection, pSession, pWindowSize, pWindow, positionOfWindowInStream);
            pConnection->toDo = DO_WAIT_CLIENT_CHANGE;
            return false;                     //Not finished
            
        case DO_SEND_SERVER_CHANGE:
            sendServerChange(pConnection, pSession, pWindowSize, pWindow, positionOfWindowInStream);
            pConnection->toDo = DO_APPLICATION;
            return false;
            
        case DO_APPLICATION:
        {            
            int status = HttpPoll(connectionId, clientFinished);
            bool finished = false;
            switch (status)
            {
                case HTTP_WAIT:                   finished = false;                  *pWindowSize = 0;                                 break;
                case HTTP_FINISHED:               finished = true;                   *pWindowSize = 0;                                 break;
                case HTTP_HAVE_SOMETHING_TO_SEND: finished = sendContent(pConnection, pWindowSize, pWindow, positionOfWindowInStream); break;
            }
            if (finished) pConnection->toDo = DO_WAIT_CLIENT_HELLO;
            return finished;
        }   
        case DO_SEND_ALERT_ILLEGAL_PARAMETER:
            sendFatal(TLS_ALERT_ILLEGAL_PARAMETER, pConnection, pWindowSize, pWindow, positionOfWindowInStream);
            pConnection->toDo = DO_WAIT_CLIENT_HELLO;
            return true; //Finished
            
        case DO_SEND_ALERT_INTERNAL_ERROR:
            sendFatal(TLS_ALERT_INTERNAL_ERROR, pConnection, pWindowSize, pWindow, positionOfWindowInStream);
            pConnection->toDo = DO_WAIT_CLIENT_HELLO;
            return true; //Finished
            
        default:
            LogTimeF("TlsPoll - unspecified TLS state %d\r\n", pConnection->toDo);
            sendFatal(TLS_ALERT_INTERNAL_ERROR, pConnection, pWindowSize, pWindow, positionOfWindowInStream); //Internal error
            pConnection->toDo = DO_WAIT_CLIENT_HELLO;
            return true; //Finished
    }
}