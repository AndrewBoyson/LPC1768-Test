#pragma once

#include <stdint.h>
#define SHA1_BLOCK_SIZE 64
#define SHA1_HASH_SIZE 20

struct Sha1State
{
    uint32_t count;
    uint32_t state[SHA1_HASH_SIZE / 4];
    union
    {
        uint8_t  b[SHA1_BLOCK_SIZE];
        uint32_t w[SHA1_BLOCK_SIZE / 4];
    } buf;
};
extern void Sha1Start (struct Sha1State *ctx                                            );
extern void Sha1Add   (struct Sha1State *ctx, const uint8_t *data, int len              );
extern void Sha1Finish(struct Sha1State *ctx,                               uint8_t *out);
extern void Sha1      (                       const uint8_t *data, int len, uint8_t *out);