
#include "crypto/aes-cbc/aes128cbc.h"
/*
    iv          * AES128CBC_BLOCK_SIZE (16)
    message:
        payload       * payloadLength
        mac           * SHA1_HASH_SIZE (20)
        padding       * 0 to AES128CBC_BLOCK_SIZE - 1 (0 to 15)
        paddingLength * 1
*/
#define TLS_AES_128_CBC_SHA1_MAX_OVERHEAD (AES128CBC_BLOCK_SIZE + SHA1_HASH_SIZE + AES128CBC_BLOCK_SIZE - 1 + 1)

extern void TlsAes128CbcSha1EncryptStart(uint8_t** pp);
extern void TlsAes128CbcSha1EncryptEnd(uint8_t** pp, struct TlsConnection* pConnection, uint8_t contentType);
extern void TlsAes128CbcSha1Decrypt(int* pLength, uint8_t** pp, struct TlsConnection* pConnection, uint8_t contentType);
