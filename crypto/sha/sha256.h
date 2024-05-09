#pragma once

#include <stdint.h>

#define SHA256_BLOCK_SIZE 64
#define SHA256_HASH_SIZE  32

struct Sha256State
{
    uint64_t length;
    uint32_t curlen;
    uint32_t state[SHA256_HASH_SIZE / 4];
    uint8_t  buf[SHA256_BLOCK_SIZE];
};
extern void Sha256Copy  (struct Sha256State* pTo, struct Sha256State* pFrom);

extern void Sha256Start (struct Sha256State *md                                             );
extern int  Sha256Add   (struct Sha256State *md, const uint8_t* in, int inlen               );
extern int  Sha256Finish(struct Sha256State *md,                               uint8_t* hash);
extern void Sha256      (                        const uint8_t* in, int inlen, uint8_t* hash);