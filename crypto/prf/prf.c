#include <alloca.h>

#include "crypto/hmac/hmac-sha256.h"
#include "log/log.h"
/*
master_secret = PRF(pre_master_secret, "master secret",  ClientHello.random + ServerHello.random)[0..47];

key_block     = PRF(master_secret, "key expansion", server_random + client_random);                
                    
PRF(secret, label, seed) = P_<hash>(secret, label + seed)

P_hash(secret, seed) = HMAC_hash(secret, A(1) + seed) +
                       HMAC_hash(secret, A(2) + seed) +
                       HMAC_hash(secret, A(3) + seed) + ...
                       
A(0) = seed
A(i) = HMAC_hash(secret, A(i-1))
*/

void PrfHmacSha256(uint8_t* secret, int secretLength, uint8_t* seed, int seedLength, int number, uint8_t* output) //output needs to hold number * 32 bytes
{
    uint8_t prevA[SHA256_HASH_SIZE];
    uint8_t thisA[SHA256_HASH_SIZE];
    uint8_t hash [SHA256_HASH_SIZE];
    
    for (int j = 0; j < number; j++)
    {
        //Calculate the hash A(i)
        if (j) HmacSha256(secret, secretLength, prevA, SHA256_HASH_SIZE, thisA);
        else   HmacSha256(secret, secretLength, seed,  seedLength,       thisA);
        for (int i = 0; i < SHA256_HASH_SIZE; i++) prevA[i] = thisA[i];
        
        //Concatanate A(i) + seed
        uint8_t* aPlusSeed = alloca(SHA256_HASH_SIZE + seedLength);
        for (int i = 0; i < SHA256_HASH_SIZE; i++) aPlusSeed[i                   ] = thisA[i];
        for (int i = 0; i < seedLength;       i++) aPlusSeed[i + SHA256_HASH_SIZE] =  seed[i];
        
        //Calculate the hash 
        HmacSha256(secret, secretLength, aPlusSeed, SHA256_HASH_SIZE + seedLength, hash);
        
        //Concatanate the hash onto the output
        for (int i = 0; i < SHA256_HASH_SIZE; i++) output[j * SHA256_HASH_SIZE + i] = hash[i];
    }
}
void PrfHmacSha256Test()
{
    uint8_t Secret[] = {
                        0x9b, 0xbe, 0x43, 0x6b, 0xa9, 0x40, 0xf0, 0x17,
                        0xb1, 0x76, 0x52, 0x84, 0x9a, 0x71, 0xdb, 0x35
                    };

    uint8_t Seed[] =   {
                        0x74, 0x65, 0x73, 0x74, 0x20, 0x6c, 0x61, 0x62,
                        0x65, 0x6c,
                        0xa0, 0xba, 0x9f, 0x93, 0x6c, 0xda, 0x31, 0x18,
                        0x27, 0xa6, 0xf7, 0x96, 0xff, 0xd5, 0x19, 0x8c
                    };
    
    uint8_t* hash = alloca(128);
    PrfHmacSha256(Secret, 16, Seed, 26, 4, hash); //4 iteration will generate the keys required
    Log("TLS PRF test\r\n");
    LogBytesAsHex(hash, 128);
    Log("\r\n\r\n");
/*
Output (100 bytes):
0000    e3 f2 29 ba 72 7b e1 7b    ....r...
0008    8d 12 26 20 55 7c d4 53    ... U..S
0010    c2 aa b2 1d 07 c3 d4 95    ........
0018    32 9b 52 d4 e6 1e db 5a    2.R....Z
0020    6b 30 17 91 e9 0d 35 c9    k0....5.
0028    c9 a4 6b 4e 14 ba f9 af    ..kN....
0030    0f a0 22 f7 07 7d ef 17    ........
0038    ab fd 37 97 c0 56 4b ab    ..7..VK.
0040    4f bc 91 66 6e 9d ef 9b    O..fn...
0048    97 fc e3 4f 79 67 89 ba    ...Oyg..
0050    a4 80 82 d1 22 ee 42 c5    ......B.
0058    a7 2e 5a 51 10 ff f7 01    ..ZQ....
0060    87 34 7b 66                .4.f
*/
}