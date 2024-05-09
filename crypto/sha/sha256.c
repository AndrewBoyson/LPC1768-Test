#include <stdint.h>
#include <string.h>

#include "sha256.h"

/**
 * sha256_vector - SHA256 hash for data vector
 * @num_elem: Number of elements in the data vector
 * @addr: Pointers to the data areas
 * @len: Lengths of the data blocks
 * @mac: Buffer for the hash

void sha256_vector(int num_elem, const u8 *addr[], const int *len, u8 *mac)
{
    struct sha256_state ctx;
    int i;
    sha256_init(&ctx);
    for (i = 0; i < num_elem; i++) sha256_process(&ctx, addr[i], len[i]);
    sha256_done(&ctx, mac);
}
*/
/* ===== start - public domain SHA256 implementation ===== */
/* This is based on SHA256 implementation in LibTomCrypt that was released into
 * public domain by Tom St Denis. */
/* the K array */
static const uint32_t K[64] =
{
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

static uint32_t rotate(uint32_t x, int count) { return x >> count | x << (32 - count); }

static int compress(struct Sha256State *md, const uint8_t *buf) // compress 512-bits
{
    uint32_t s[8], w[64];
    
    // copy state into s
    for (int i = 0; i < 8; i++) s[i] = md->state[i];

    // copy the state into 512-bits into w[0..15]
    for (int i = 0; i < 16; i++) w[i] = (uint32_t)*(buf + 4 * i + 0) << 24 |
                                        (uint32_t)*(buf + 4 * i + 1) << 16 |
                                        (uint32_t)*(buf + 4 * i + 2) <<  8 |
                                        (uint32_t)*(buf + 4 * i + 3);
    
    // fill w[16..63]
    for (int i = 16; i < 64; i++)
    {
        const uint32_t s0 = rotate(w[i - 15],  7) ^ rotate(w[i - 15], 18) ^ (w[i - 15] >>  3);
        const uint32_t s1 = rotate(w[i -  2], 17) ^ rotate(w[i -  2], 19) ^ (w[i -  2] >> 10);
        w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }

    // Compress
    for (int i = 0; i < 64; ++i)
    {
        const uint32_t s1 = rotate(s[4], 6) ^ rotate(s[4], 11) ^ rotate(s[4], 25);
        const uint32_t ch = (s[4] & s[5]) ^ (~s[4] & s[6]);
        const uint32_t t0 = s[7] + s1 + ch + K[i] + w[i];
        const uint32_t s0 = rotate(s[0], 2) ^ rotate(s[0], 13) ^ rotate(s[0], 22);
        const uint32_t maj = (s[0] & s[1]) ^ (s[0] & s[2]) ^ (s[1] & s[2]);
        const uint32_t t1 = s0 + maj;

        s[7] = s[6];
        s[6] = s[5];
        s[5] = s[4]; 
        s[4] = s[3] + t0;
        s[3] = s[2];
        s[2] = s[1];
        s[1] = s[0];
        s[0] = t0 + t1;
    }
    
    // feedback
    for (int i = 0; i < 8; i++) md->state[i] = md->state[i] + s[i];

    return 0;
}

void Sha256Start(struct Sha256State *md) // Initialize the hash state
{
    md->curlen = 0;
    md->length = 0;
    md->state[0] = 0x6A09E667UL;
    md->state[1] = 0xBB67AE85UL;
    md->state[2] = 0x3C6EF372UL;
    md->state[3] = 0xA54FF53AUL;
    md->state[4] = 0x510E527FUL;
    md->state[5] = 0x9B05688CUL;
    md->state[6] = 0x1F83D9ABUL;
    md->state[7] = 0x5BE0CD19UL;
}

int Sha256Add(struct Sha256State *md, const uint8_t *in, int inlen) //Return 0 if ok; -1 on error
{
    if (md->curlen > SHA256_BLOCK_SIZE) return -1;
    
    while (inlen > 0)
    {
        if (md->curlen == 0 && inlen >= SHA256_BLOCK_SIZE)
        {
            if (compress(md, in) < 0) return -1;
            md->length += SHA256_BLOCK_SIZE * 8;
            in         += SHA256_BLOCK_SIZE;
            inlen      -= SHA256_BLOCK_SIZE;
        }
        else
        {
            int n = inlen < SHA256_BLOCK_SIZE - md->curlen ? inlen : SHA256_BLOCK_SIZE - md->curlen;
            memcpy(md->buf + md->curlen, in, n);
            md->curlen += n;
            in         += n;
            inlen      -= n;
            if (md->curlen == SHA256_BLOCK_SIZE)
            {
                if (compress(md, md->buf) < 0) return -1;
                md->length += 8 * SHA256_BLOCK_SIZE;
                md->curlen  = 0;
            }
        }
    }
    return 0;
}

int Sha256Finish(struct Sha256State *md, uint8_t *out) //returns 0 on success; -1 on failure
{
    if (md->curlen >= SHA256_BLOCK_SIZE) return -1;
    
    // increase the length of the message
    md->length += md->curlen * 8;
    
    // append the '1' bit
    md->buf[md->curlen++] = 0x80;
    
    /* if the length is currently above 56 bytes we append zeros
     * then compress.  Then we can fall back to padding zeros and length
     * encoding like normal.
     */
    if (md->curlen > 56)
    {
        while (md->curlen < 64) md->buf[md->curlen++] = 0;
        compress(md, md->buf);
        md->curlen = 0;
    }
    // pad upto 56 bytes of zeroes
    while (md->curlen < 56)  md->buf[md->curlen++] = 0;

    // store length
    *(md->buf + 56 + 0) = (uint8_t) (md->length >> 56);
    *(md->buf + 56 + 1) = (uint8_t) (md->length >> 48);
    *(md->buf + 56 + 2) = (uint8_t) (md->length >> 40);
    *(md->buf + 56 + 3) = (uint8_t) (md->length >> 32);
    *(md->buf + 56 + 4) = (uint8_t) (md->length >> 24);
    *(md->buf + 56 + 5) = (uint8_t) (md->length >> 16);
    *(md->buf + 56 + 6) = (uint8_t) (md->length >>  8);
    *(md->buf + 56 + 7) = (uint8_t) (md->length >>  0);


    compress(md, md->buf);
    
    // copy output
    for (int i = 0; i < 8; i++)
    {
        *(out + 4 * i + 0) = (uint8_t) (md->state[i] >> 24);
        *(out + 4 * i + 1) = (uint8_t) (md->state[i] >> 16);
        *(out + 4 * i + 2) = (uint8_t) (md->state[i] >>  8);
        *(out + 4 * i + 3) = (uint8_t) (md->state[i] >>  0);
    }
    
    return 0;
}
void Sha256(const uint8_t* in, int inlen, uint8_t* hash)
{
    struct Sha256State md;
    Sha256Start (&md);
    Sha256Add   (&md, in, inlen);
    Sha256Finish(&md, hash);
}

void Sha256Copy(struct Sha256State* pTo, struct Sha256State* pFrom)
{
    pTo->length = pFrom->length;
    pTo->curlen = pFrom->curlen;
    for (int i = 0; i < SHA256_HASH_SIZE / 4; i++) pTo->state[i] = pFrom->state[i];
    for (int i = 0; i < SHA256_BLOCK_SIZE;    i++) pTo->buf[i]   = pFrom->buf[i];
}