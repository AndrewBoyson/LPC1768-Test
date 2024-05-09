#include <stdint.h>
#include "crypto/sha/sha1.h"

struct HmacSha1Struct
{
    uint8_t hmacKey[SHA1_BLOCK_SIZE];
    struct Sha1State shaStruct;
    uint8_t tmpHash[SHA1_HASH_SIZE];
};

extern void HmacSha1(const uint8_t* key, int keyLength, const uint8_t* message, int messageLength, uint8_t* mac);

extern void HmacSha1Start (struct HmacSha1Struct* md, const uint8_t* key,     int     keyLength);
extern void HmacSha1Add   (struct HmacSha1Struct* md, const uint8_t* message, int messageLength);
extern void HmacSha1Finish(struct HmacSha1Struct* md,       uint8_t* mac);