#include "tls.h"
#include "tls-defs.h"
#include "tls-connection.h"
#include "tls-session.h"
#include "tls-log.h"
#include "tls-prf.h"
#include "lpc1768/mstimer/mstimer.h"
#include "log/log.h"
#include "crypto/pki/pri-key.h"
#include "crypto/tls/tls-aes128cbc-sha/tls-aes128cbc-sha.h"
#include "crypto/tls/tls-aes128cbc-sha/tls-mac.h"
#include "lpc1768/led.h"
#include "web/http/http.h"

static void handleClientHello(int length, uint8_t* pBuffer, struct TlsConnection* pConnection)
{   
    //Check things look ok
    uint8_t* p = pBuffer;
    if (length < 100)
    {
        LogF("TLS - %d byte client hello message is not at least 100 bytes long\r\n", length);
        pConnection->toDo = DO_SEND_ALERT_ILLEGAL_PARAMETER;
        return;
    }
    
    //get the version
    uint8_t versionH         = *p++;
    uint8_t versionL         = *p++;
    
    //Get the client random
    for (int i = 0; i < 32; i++) pConnection->clientRandom[i] = *p++;
    
    //Get the session id
    int sessionIdLength = *p++;
    uint8_t* pSessionId = p;
    
    if (sessionIdLength == 4)
    {
        pConnection->sessionId  = *p++;
        pConnection->sessionId <<= 8;
        pConnection->sessionId |= *p++;
        pConnection->sessionId <<= 8;
        pConnection->sessionId |= *p++;
        pConnection->sessionId <<= 8;
        pConnection->sessionId |= *p++;
    }
    else
    {
        pConnection->sessionId = 0;
        p += sessionIdLength;
    }
    
    //Work out from the session id if this connection can be resumed from an existing session
    struct TlsSession* pSession = TlsSessionOrNull(pConnection->sessionId);
    pConnection->resume = pSession && pSession->valid;
    
    //If it cannot be resumed then create a new one
    if (!pConnection->resume)
    {
        pSession = TlsSessionNew();
        pConnection->sessionId = pSession->id;
    }

    pSession->lastUsed = MsTimerCount;
    
    //Log the parameters
    if (TlsTrace)
    {
        LogF("- client version HH:LL: %02x:%02x\r\n", versionH, versionL);
        Log ("- client random:\r\n");     LogBytesAsHex(pConnection->clientRandom, TLS_LENGTH_RANDOM); Log("\r\n");
        Log ("- client session id:\r\n"); LogBytesAsHex(pSessionId, sessionIdLength); Log("\r\n");
        LogF("- session index: %u\r\n",  pConnection->sessionId);
        if (pConnection->resume) Log ("- existing session so resume\r\n");
        else                     Log ("- new session\r\n");
    }
    
    if (pConnection->resume) pConnection->toDo = DO_SEND_SERVER_HELLO_RESUME;
    else                     pConnection->toDo = DO_SEND_SERVER_HELLO_NEW;
    
    return;
}
static void handleClientKeyExchange(int length, uint8_t* pBuffer, struct TlsConnection* pConnection) //returns 0 on success; -1 on error
{
    struct TlsSession* pSession = TlsSessionOrNull(pConnection->sessionId);
    if (!pSession)
    {
        LogTimeF("handleClientKeyExchange - invalid session %u\r\n", pConnection->sessionId);
        pConnection->toDo = DO_SEND_ALERT_ILLEGAL_PARAMETER;
        return;
    }
    
    if (length != 130)
    {
        LogF("TLS - %d byte client key exchange message is not 130 bytes long\r\n", length);
        pConnection->toDo = DO_SEND_ALERT_ILLEGAL_PARAMETER;
        return;
    }
    int premasterLength = pBuffer[0] << 8 | pBuffer[1]; //Overall length 2 bytes
    if (premasterLength != 128)
    {
        LogF("TLS - %d byte encrypted pre master secret is not 128 bytes long\r\n", length);
        pConnection->toDo = DO_SEND_ALERT_ILLEGAL_PARAMETER;
        return;
    }
    uint8_t* pEncryptedPreMasterSecret = pBuffer + 2;
    pConnection->slotPriKeyDecryption = PriKeyDecryptStart(pEncryptedPreMasterSecret);
    
    if (TlsTrace)
    {
        LogF("- encrypted pre master\r\n", premasterLength);
        LogBytesAsHex(pEncryptedPreMasterSecret, 128);
        Log("\r\n");
    }
    
    pConnection->toDo = DO_WAIT_DECRYPT_MASTER_SECRET;
    return;
}
static void handleClientFinished(int length, uint8_t* pBuffer, struct TlsConnection* pConnection) //returns 0 on success; -1 on error
{
    struct TlsSession* pSession = TlsSessionOrNull(pConnection->sessionId);
    if (!pSession)
    {
        LogTimeF("handleClientKeyExchange - invalid session %u\r\n", pConnection->sessionId);
        pConnection->toDo = DO_SEND_ALERT_ILLEGAL_PARAMETER;
        return;
    }
    
    //Calculate the verify message
    uint8_t verify[TLS_LENGTH_VERIFY];
    TlsPrfClientFinished(pSession->masterSecret, pConnection->clientHandshakeHash, verify);
    if (TlsTrace) { Log("- verify handshake\r\n"); LogBytesAsHex(verify, TLS_LENGTH_VERIFY); Log("\r\n"); }
    
    if (pConnection->resume) pConnection->toDo = DO_APPLICATION;
    else                     pConnection->toDo = DO_SEND_SERVER_CHANGE;
    
    return;
}
static void changeCipher(int length, uint8_t* pBuffer, struct TlsConnection* pConnection)
{
    uint8_t message = pBuffer[0];
    if (TlsTrace) LogF("- message: %d\r\n", message);
    
    //Take snapshot of the handshake hash up to this point
    struct Sha256State handshakeSha;
    Sha256Copy  (&handshakeSha, &pConnection->handshakeSha);
    Sha256Finish(&handshakeSha, pConnection->clientHandshakeHash);
    
    //Record that all incoming messages are now encrypted
    pConnection->clientEncrypted = true;
    pConnection->clientSequence = 0;
}
static void handleAlert(int length, uint8_t* pBuffer)
{
    uint8_t level       = pBuffer[0];
    uint8_t description = pBuffer[1];
    if (TlsTrace)
    {
        Log("- alert level:       "); TlsLogAlertLevel      (level);       Log("\r\n");
        Log("- alert description: "); TlsLogAlertDescription(description); Log("\r\n");
    }
}
static void handleApplication(int length, uint8_t* pBuffer, struct TlsConnection* pConnection)
{
    if (TlsTrace)
    {
        Log("- application data:\r\n");
        LogBytesAsHex(pBuffer, length);
        Log("\r\n");
    }
    HttpRequest(pConnection->id, length, (char*)pBuffer, 0);
}

static void handleHandshake(int length, uint8_t* pBuffer, struct TlsConnection* pConnection)
{
    Sha256Add(&pConnection->handshakeSha, pBuffer, length);
    
    uint8_t* p = pBuffer;
    while (p < pBuffer + length)
    {
        uint8_t handshakeType    = *p++;
        int     handshakeLength  = *p++ << 16;
                handshakeLength |= *p++ <<  8;
                handshakeLength |= *p++      ; //Handshake length 3 bytes
             
        if (TlsTrace)
        {
            Log ("- handshake type: "); TlsLogHandshakeType(handshakeType); Log("\r\n");
            LogF("- handshake length: %d\r\n", handshakeLength);
        }
        
        switch (handshakeType)
        {
            case TLS_HANDSHAKE_CLIENT_HELLO:
                handleClientHello(handshakeLength, p, pConnection);
                break;
                
            case TLS_HANDSHAKE_CLIENT_KEY_EXCHANGE:
                handleClientKeyExchange(handshakeLength, p, pConnection);
                break;
                
            case TLS_HANDSHAKE_FINISHED:
                handleClientFinished(handshakeLength, p, pConnection);
                break;
                
            default:
                LogF("TLS - ignoring handshake type ");
                TlsLogHandshakeType(handshakeType);
                LogF(" and skipping %d bytes\r\n", handshakeLength);
                break;
        }
        p += handshakeLength;
    }
}
static int handleContent(struct TlsConnection* pConnection, uint8_t* pBuffer, int available)
{   
    uint8_t contentType = *pBuffer++;
    uint8_t versionH    = *pBuffer++;
    uint8_t versionL    = *pBuffer++;
    int     length      = *pBuffer++ << 8;
            length     |= *pBuffer++;
    int     overallLen  = length + 5;
            
    if (TlsTrace)
    {
        Log ("- content type: "); TlsLogContentType(contentType); Log("\r\n");
        LogF("- legacy HH:LL: %02x:%02x\r\n", versionH, versionL);
        LogF("- length      : %d\r\n"       , length);
    }
    
    if (contentType < 20 || contentType > 24) return overallLen;
    if (versionH    != 3                    ) return overallLen;
    if (overallLen  > available             ) return overallLen;
        
    if (pConnection->clientEncrypted)
    {
        //Decrypt the message
        TlsAes128CbcSha1Decrypt(&length, &pBuffer, pConnection, contentType);
    }
    
    switch (contentType)
    {
        case TLS_CONTENT_TYPE_HANDSHAKE:
            handleHandshake(length, pBuffer, pConnection);
            break;

        case TLS_CONTENT_TYPE_CHANGE_CIPHER:
            changeCipher(length, pBuffer, pConnection);
            break;

        case TLS_CONTENT_TYPE_ALERT:
            handleAlert(length, pBuffer);
            break;

        case TLS_CONTENT_TYPE_APPLICATION:
            handleApplication(length, pBuffer, pConnection);
            pConnection->toDo = DO_APPLICATION;
            break;
        
        default:
            Log("TLS - ignoring content type ");
            TlsLogContentType(contentType);
            LogF(" and skipping %d bytes\r\n", overallLen);
            pConnection->toDo = DO_WAIT_CLIENT_HELLO;
            break;
    }
    return overallLen;
}
void TlsRequest(int connectionId, int size, uint8_t* pRequestStream, uint32_t positionInRequestStream)
{   
    //Log what we are doing
    if (TlsTrace) LogF("TLS %d <<< %d (%u)\r\n", connectionId, size, positionInRequestStream);
    
    //Get new or existing connection information
    struct TlsConnection* pConnection;
    if (!positionInRequestStream)
    {
        //If this is the start of the request then open a new connection (this starts handshakeSha)
        pConnection = TlsConnectionNew(connectionId);
    }
    else
    {
        //If this is in the middle of a request then open an existing connection
        pConnection = TlsConnectionOrNull(connectionId);
        if (!pConnection)
        {
            LogTimeF("TlsRequest - no connection corresponds to id %d\r\n", connectionId);
            return;
        }
    }
        
    //Handle each item of coalesced content
    uint8_t* pNext = pRequestStream;
    uint8_t* pDeferred = pConnection->deferredContent;
    while (pNext < pRequestStream + size)
    {
        if (pConnection->clientEncrypted && pConnection->toDo == DO_WAIT_DECRYPT_MASTER_SECRET)
        {
            if (pDeferred >= pConnection->deferredContent + TLS_DEFERRED_CONTENT_SIZE) break;
            *pDeferred++ = *pNext++; //Defer handling the remaining content until have master secret
        }
        else
        {
            pNext += handleContent(pConnection, pNext, size + pRequestStream - pNext);
        }
    }
}
void TlsRequestMain()
{  
    struct TlsConnection* pConnection = TlsConnectionGetNext();
    
    if (pConnection->toDo != DO_WAIT_DECRYPT_MASTER_SECRET) return;
    
    struct TlsSession* pSession = TlsSessionOrNull(pConnection->sessionId);
    if (!pSession)
    {
        LogTimeF("TlsRequestAsync - invalid session %u\r\n", pConnection->sessionId);
        return;
    }
    
    if (pSession->valid) return;
    
    if (!PriKeyDecryptFinished(pConnection->slotPriKeyDecryption)) return;
    
    uint8_t preMasterSecret[TLS_LENGTH_PRE_MASTER_SECRET];
    PriKeyDecryptResultTail(pConnection->slotPriKeyDecryption, TLS_LENGTH_PRE_MASTER_SECRET, preMasterSecret);
    if (TlsTrace)
    {
        LogTime("Pre master secret\r\n");
        LogBytesAsHex(preMasterSecret, TLS_LENGTH_PRE_MASTER_SECRET);
        Log("\r\n");
    }
    PriKeyDecryptClear(pConnection->slotPriKeyDecryption);
    
    TlsPrfMasterSecret(preMasterSecret, pConnection->clientRandom, pConnection->serverRandom, pSession->masterSecret);
    
    TlsPrfKeysAes128Sha1(pSession->masterSecret, pConnection->clientRandom, pConnection->serverRandom, pConnection->clientMacKey,
                                                                                                       pConnection->serverMacKey,
                                                                                                       pConnection->clientWriteKey,
                                                                                                       pConnection->serverWriteKey);

    pSession->valid = true;
    
    if (TlsTrace)
    {
        LogTime("Sending deferred encrypted bytes\r\n");
        LogBytesAsHex(pConnection->deferredContent, TLS_DEFERRED_CONTENT_SIZE);
        Log("\r\n");
    }
    
    handleContent(pConnection, pConnection->deferredContent, TLS_DEFERRED_CONTENT_SIZE);
}
