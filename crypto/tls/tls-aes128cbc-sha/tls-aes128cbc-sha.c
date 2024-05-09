#include <stdint.h>

#include "crypto/tls/tls-defs.h"
#include "crypto/tls/tls-connection.h"
#include "crypto/tls/tls-aes128cbc-sha/tls-mac.h"
#include "log/log.h"
#include "crypto/aes-cbc/aes128cbc.h"
#include "lpc1768/random/random.h"

static uint8_t* encryptIvPointer;
static uint8_t* encryptPayloadPointer;
static int      encryptPayloadSize;
static void encryptAddIv(uint8_t** pp)
{
    uint8_t* p = *pp;
    
    //Add the IV
    encryptIvPointer = p;
    for (int i = 0; i < AES128CBC_BLOCK_SIZE; i++) *p++ = RandomGetByte();
    
    encryptPayloadPointer = p;
    
    *pp = p;
}
static void encryptAddMac(uint8_t** pp, struct TlsConnection* pConnection, uint8_t contentType)
{
    uint8_t* p = *pp;
    
    encryptPayloadSize = p - encryptPayloadPointer;
    
    //Add the MAC
    TlsMacSha1(SHA1_HASH_SIZE,
               pConnection->serverMacKey,
               pConnection->serverSequence,
               contentType,
               0x03,
               0x03,
               encryptPayloadSize,
               encryptPayloadPointer,
               p);
    p += SHA1_HASH_SIZE;

    *pp = p;
}

static void encryptAddPadding(uint8_t** pp)
{
    uint8_t* p = *pp;
    
    int paddingSize = AES128CBC_BLOCK_SIZE - 1 - (encryptPayloadSize + SHA1_HASH_SIZE + 1 - 1) % AES128CBC_BLOCK_SIZE;
    for (int i = 0; i < paddingSize; i++) *p++ = paddingSize;
    
    *p++ = paddingSize;
    
    *pp = p;
}

static void encryptPayload(uint8_t* p, struct TlsConnection* pConnection)
{
    //Encrypt payload + mac + padding
    Aes128CbcEncrypt(pConnection->serverWriteKey, encryptIvPointer, encryptPayloadPointer, p - encryptPayloadPointer);
}
void TlsAes128CbcSha1EncryptStart(uint8_t** pp)
{
    encryptAddIv(pp);
}
void TlsAes128CbcSha1EncryptEnd(uint8_t** pp, struct TlsConnection* pConnection, uint8_t contentType)
{
    encryptAddMac    ( pp, pConnection, contentType);
    encryptAddPadding( pp);
    encryptPayload   (*pp, pConnection);
}
void TlsAes128CbcSha1Decrypt(int* pLength, uint8_t** pp, struct TlsConnection* pConnection, uint8_t contentType)
{
    uint8_t* p = *pp;
    int length = *pLength;
    
    uint8_t* pIv = p;
    p += AES128CBC_BLOCK_SIZE;
    Aes128CbcDecrypt(pConnection->clientWriteKey, pIv, p, length - AES128CBC_BLOCK_SIZE);
    uint8_t paddingLength = *(p + length - AES128CBC_BLOCK_SIZE - 1);
    int payloadLength = length - AES128CBC_BLOCK_SIZE - paddingLength - SHA1_HASH_SIZE - 1;
    
    uint8_t mac[SHA1_HASH_SIZE];
    TlsMacSha1(SHA1_HASH_SIZE,
               pConnection->clientMacKey,
               pConnection->clientSequence,
               contentType,
               0x03,
               0x03,
               payloadLength,
               p,
               mac);
    
    pConnection->clientSequence++;
    
    *pp = p;
    *pLength = payloadLength;
}