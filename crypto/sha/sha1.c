#include <string.h>
#include "sha1.h"

static uint32_t ror27(uint32_t val)
{
    return (val >> 27) | (val << 5);
}
static uint32_t ror2(uint32_t val)
{
    return (val >> 2) | (val << 30);
}
static uint32_t ror31(uint32_t val)
{
    return (val >> 31) | (val << 1);
}
static void sha1_transform(struct Sha1State *ctx)
{
    uint32_t W[80];
    register uint32_t A, B, C, D, E;
    int t;
    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
#define SHA_F1(A, B, C, D, E, t)                    \
    E += ror27(A) +                         \
            (W[t] = __builtin_bswap32(ctx->buf.w[t])) + \
            (D^(B&(C^D))) + 0x5A827999;         \
    B = ror2(B);
    for (t = 0; t < 15; t += 5) {
        SHA_F1(A, B, C, D, E, t + 0);
        SHA_F1(E, A, B, C, D, t + 1);
        SHA_F1(D, E, A, B, C, t + 2);
        SHA_F1(C, D, E, A, B, t + 3);
        SHA_F1(B, C, D, E, A, t + 4);
    }
    SHA_F1(A, B, C, D, E, t + 0);  /* 16th one, t == 15 */
#undef SHA_F1
#define SHA_F1(A, B, C, D, E, t)                          \
    E += ror27(A) +                               \
            (W[t] = ror31(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16])) + \
            (D^(B&(C^D))) + 0x5A827999;               \
    B = ror2(B);
    SHA_F1(E, A, B, C, D, t + 1);
    SHA_F1(D, E, A, B, C, t + 2);
    SHA_F1(C, D, E, A, B, t + 3);
    SHA_F1(B, C, D, E, A, t + 4);
#undef SHA_F1
#define SHA_F2(A, B, C, D, E, t)                          \
    E += ror27(A) +                               \
            (W[t] = ror31(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16])) + \
            (B^C^D) + 0x6ED9EBA1;                     \
    B = ror2(B);
    for (t = 20; t < 40; t += 5) {
        SHA_F2(A, B, C, D, E, t + 0);
        SHA_F2(E, A, B, C, D, t + 1);
        SHA_F2(D, E, A, B, C, t + 2);
        SHA_F2(C, D, E, A, B, t + 3);
        SHA_F2(B, C, D, E, A, t + 4);
    }
#undef SHA_F2
#define SHA_F3(A, B, C, D, E, t)                          \
    E += ror27(A) +                               \
            (W[t] = ror31(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16])) + \
            ((B&C)|(D&(B|C))) + 0x8F1BBCDC;               \
    B = ror2(B);
    for (; t < 60; t += 5) {
        SHA_F3(A, B, C, D, E, t + 0);
        SHA_F3(E, A, B, C, D, t + 1);
        SHA_F3(D, E, A, B, C, t + 2);
        SHA_F3(C, D, E, A, B, t + 3);
        SHA_F3(B, C, D, E, A, t + 4);
    }
#undef SHA_F3
#define SHA_F4(A, B, C, D, E, t)                          \
    E += ror27(A) +                               \
            (W[t] = ror31(W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16])) + \
            (B^C^D) + 0xCA62C1D6;                     \
    B = ror2(B);
    for (; t < 80; t += 5) {
        SHA_F4(A, B, C, D, E, t + 0);
        SHA_F4(E, A, B, C, D, t + 1);
        SHA_F4(D, E, A, B, C, t + 2);
        SHA_F4(C, D, E, A, B, t + 3);
        SHA_F4(B, C, D, E, A, t + 4);
    }
#undef SHA_F4
    ctx->state[0] += A;
    ctx->state[1] += B;
    ctx->state[2] += C;
    ctx->state[3] += D;
    ctx->state[4] += E;
}
void Sha1Add(struct Sha1State *ctx, const uint8_t *data, int len)
{
    int i = ctx->count % SHA1_BLOCK_SIZE;
    const uint8_t *p = (const uint8_t *)data;
    ctx->count += len;
    while (len > SHA1_BLOCK_SIZE - i)
    {
        memcpy(&ctx->buf.b[i], p, SHA1_BLOCK_SIZE - i);
        len -= SHA1_BLOCK_SIZE - i;
        p   += SHA1_BLOCK_SIZE - i;
        sha1_transform(ctx);
        i = 0;
    }
    while (len--) {
        ctx->buf.b[i++] = *p++;
        if (i == SHA1_BLOCK_SIZE) {
            sha1_transform(ctx);
            i = 0;
        }
    }
}
void Sha1Finish(struct Sha1State *ctx, uint8_t *out)
{
    uint32_t cnt = ctx->count * 8;
    int i;
    Sha1Add(ctx, (uint8_t *)"\x80", 1);
    while ((ctx->count % SHA1_BLOCK_SIZE) != (SHA1_BLOCK_SIZE - 8))
        Sha1Add(ctx, (uint8_t *)"\0", 1);
    for (i = 0; i < 8; ++i) {
        uint8_t tmp = cnt >> ((7 - i) * 8);
        Sha1Add(ctx, &tmp, 1);
    }
    for (i = 0; i <  5; i++) ctx->buf.w[i] = __builtin_bswap32(ctx->state[i]);
    for (i = 0; i < 20; i++) out[i] = ctx->buf.b[i];
}
void Sha1Start(struct Sha1State *ctx)
{
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xEFCDAB89;
    ctx->state[2] = 0x98BADCFE;
    ctx->state[3] = 0x10325476;
    ctx->state[4] = 0xC3D2E1F0;
    ctx->count = 0;
}

void Sha1(const uint8_t* in, int inlen, uint8_t* hash)
{
    struct Sha1State md;
    Sha1Start (&md);
    Sha1Add   (&md, in, inlen);
    Sha1Finish(&md, hash);
}