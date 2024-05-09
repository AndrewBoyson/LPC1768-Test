#include <stdint.h>

#include "hmac-sha1.h"
#include "crypto/sha/sha1.h"

#define BLOCK_SIZE 64
#define  HASH_SIZE 20

void HmacSha1Start(struct HmacSha1Struct* md, const uint8_t* key, int keyLength)
{
    //Make the key BLOCK_SIZE bytes long by hashing longer ones or padding shorter one with 0s
    if (keyLength > BLOCK_SIZE)
    {
        Sha1(key, keyLength, md->hmacKey);
        for (int i = HASH_SIZE; i < BLOCK_SIZE; i++) md->hmacKey[i] = 0;
    }
    else
    {
        for (int i = 0;         i < keyLength;  i++) md->hmacKey[i] = key[i];
        for (int i = keyLength; i < BLOCK_SIZE; i++) md->hmacKey[i] = 0;
    }
    Sha1Start(&md->shaStruct);
    
    //Make the inner and outer padded keys
    uint8_t iKeyPad[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        iKeyPad[i] = md->hmacKey[i] ^ 0x36; //Inner padded key
    }
    Sha1Add(&md->shaStruct, iKeyPad, BLOCK_SIZE);
}
void HmacSha1Add(struct HmacSha1Struct* md, const uint8_t* message, int messageLength)
{
    Sha1Add(&md->shaStruct, message, messageLength);
}
void HmacSha1Finish(struct HmacSha1Struct* md, uint8_t* mac)
{
    //Make the inner and outer padded keys
    uint8_t oKeyPad[BLOCK_SIZE];
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        oKeyPad[i] = md->hmacKey[i] ^ 0x5c; //Outer padded key
    }
    
    uint8_t innerHash[HASH_SIZE];
    Sha1Finish(&md->shaStruct, innerHash);
    
    Sha1Start(&md->shaStruct);
    Sha1Add(&md->shaStruct, oKeyPad, BLOCK_SIZE);
    Sha1Add(&md->shaStruct, innerHash, HASH_SIZE);
    
    Sha1Finish(&md->shaStruct, mac);
}
void HmacSha1(const uint8_t* key, int keyLength, const uint8_t* message, int messageLength, uint8_t* mac)
{
    struct HmacSha1Struct md;
    HmacSha1Start (&md, key, keyLength);
    HmacSha1Add   (&md, message, messageLength);
    HmacSha1Finish(&md, mac);
}