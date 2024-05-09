#include <stdint.h>
#include "crypto/sha/sha256.h"

struct HmacSha256Struct
{
    uint8_t hmacKey[SHA256_BLOCK_SIZE];
    struct Sha256State shaStruct;
    uint8_t tmpHash[SHA256_HASH_SIZE];
};

extern void HmacSha256(const uint8_t* key, int keyLength, const uint8_t* message, int messageLength, uint8_t* mac);

extern void HmacSha256Start (struct HmacSha256Struct* md, const uint8_t* key,     int     keyLength);
extern void HmacSha256Add   (struct HmacSha256Struct* md, const uint8_t* message, int messageLength);
extern void HmacSha256Finish(struct HmacSha256Struct* md,       uint8_t* mac);