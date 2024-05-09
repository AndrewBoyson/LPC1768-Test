#include <stdint.h>

#include "hmac-sha256.h"
#include "crypto/sha/sha256.h"

#define BLOCK_SIZE 64
#define  HASH_SIZE 32

void HmacSha256Start(struct HmacSha256Struct* md, const uint8_t* key, int keyLength)
{
    //Make the key BLOCK_SIZE bytes long by hashing longer ones or padding shorter one with 0s
    if (keyLength > BLOCK_SIZE)
    {
        Sha256(key, keyLength, md->hmacKey);
        for (int i = HASH_SIZE; i < BLOCK_SIZE; i++) md->hmacKey[i] = 0;
    }
    else
    {
        for (int i = 0;         i < keyLength;  i++) md->hmacKey[i] = key[i];
        for (int i = keyLength; i < BLOCK_SIZE; i++) md->hmacKey[i] = 0;
    }
    Sha256Start(&md->shaStruct);
    
    //Make the inner and outer padded keys
    uint8_t iKeyPad[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        iKeyPad[i] = md->hmacKey[i] ^ 0x36; //Inner padded key
    }
    Sha256Add(&md->shaStruct, iKeyPad, BLOCK_SIZE);
}
void HmacSha256Add(struct HmacSha256Struct* md, const uint8_t* message, int messageLength)
{
    Sha256Add(&md->shaStruct, message, messageLength);
}
void HmacSha256Finish(struct HmacSha256Struct* md, uint8_t* mac)
{
    //Make the inner and outer padded keys
    uint8_t oKeyPad[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        oKeyPad[i] = md->hmacKey[i] ^ 0x5c; //Outer padded key
    }
    
    uint8_t innerHash[HASH_SIZE];
    Sha256Finish(&md->shaStruct, innerHash);
    
    Sha256Start(&md->shaStruct);
    Sha256Add(&md->shaStruct, oKeyPad, BLOCK_SIZE);
    Sha256Add(&md->shaStruct, innerHash, HASH_SIZE);
    
    Sha256Finish(&md->shaStruct, mac);
}

void HmacSha256(const uint8_t* key, int keyLength, const uint8_t* message, int messageLength, uint8_t* mac)
{
    struct HmacSha256Struct md;
    HmacSha256Start (&md, key, keyLength);
    HmacSha256Add   (&md, message, messageLength);
    HmacSha256Finish(&md, mac);
}