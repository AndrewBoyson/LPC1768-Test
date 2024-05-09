#include <stdint.h>
#include <alloca.h>

#include "crypto/prf/prf.h"
#include "tls-defs.h"
#include "crypto/sha/sha256.h"
#include "crypto/sha/sha1.h"
#include "crypto/aes-cbc/aes128cbc.h"

void TlsPrfMasterSecret(uint8_t * preMasterSecret, uint8_t* clientRandom, uint8_t* serverRandom, uint8_t* masterSecret)
{
    const int SEED_LENGTH = 13 + TLS_LENGTH_RANDOM + TLS_LENGTH_RANDOM;
    uint8_t* seed = alloca(SEED_LENGTH);
    for (int i = 0; i < 13;                i++) seed[i                         ] = "master secret"[i];
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++) seed[i + 13                    ] = clientRandom[i];
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++) seed[i + 13 + TLS_LENGTH_RANDOM] = serverRandom[i];
    
    const int ITERATIONS = 2;
    uint8_t* hash = alloca(SHA256_HASH_SIZE * ITERATIONS);                                             //32 * 2
    PrfHmacSha256(preMasterSecret, TLS_LENGTH_PRE_MASTER_SECRET, seed, SEED_LENGTH, ITERATIONS, hash); //2 iterations will generate 64 bytes
    for (int i = 0; i < TLS_LENGTH_MASTER_SECRET; i++) masterSecret[i] = hash[i];                      //just take the first 48 bytes
}

void TlsPrfKeysAes128Sha1(uint8_t * masterSecret, uint8_t* clientRandom, uint8_t* serverRandom, uint8_t* client_MAC_key,
                                                                                                uint8_t* server_MAC_key,
                                                                                                uint8_t* client_key,
                                                                                                uint8_t* server_key)
{
    const int SEED_LENGTH = 13 + TLS_LENGTH_RANDOM + TLS_LENGTH_RANDOM;
    uint8_t* seed = alloca(SEED_LENGTH);
    for (int i = 0; i < 13;                i++) seed[i                         ] = "key expansion"[i];
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++) seed[i + 13                    ] = serverRandom[i]; //Notice the order relative to the master secret algorithm
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++) seed[i + 13 + TLS_LENGTH_RANDOM] = clientRandom[i];
    
    const int ITERATIONS = 4;
    uint8_t* hash = alloca(SHA256_HASH_SIZE * ITERATIONS);                                      //4 iterations of 32 bytes
    PrfHmacSha256(masterSecret, TLS_LENGTH_MASTER_SECRET, seed, SEED_LENGTH, ITERATIONS, hash); //4 iteration will generate the keys required
    for (int i = 0; i < SHA1_HASH_SIZE;       i++) client_MAC_key[i] = hash[i                                                         ];
    for (int i = 0; i < SHA1_HASH_SIZE;       i++) server_MAC_key[i] = hash[i + SHA1_HASH_SIZE                                        ];
    for (int i = 0; i < AES128CBC_BLOCK_SIZE; i++) client_key[i]     = hash[i + SHA1_HASH_SIZE + SHA1_HASH_SIZE                       ];
    for (int i = 0; i < AES128CBC_BLOCK_SIZE; i++) server_key[i]     = hash[i + SHA1_HASH_SIZE + SHA1_HASH_SIZE + AES128CBC_BLOCK_SIZE];
}

void TlsPrfServerFinished(uint8_t * masterSecret, uint8_t* handshakeHash, uint8_t* verify)
{
    const int SEED_LENGTH = 15 + SHA256_HASH_SIZE;
    uint8_t* seed = alloca(SEED_LENGTH);
    for (int i = 0; i < 15;               i++) seed[i     ] = "server finished"[i];
    for (int i = 0; i < SHA256_HASH_SIZE; i++) seed[i + 15] = handshakeHash[i];
    
    const int ITERATIONS = 1;
    uint8_t* hash = alloca(SHA256_HASH_SIZE * ITERATIONS);
    PrfHmacSha256(masterSecret, TLS_LENGTH_MASTER_SECRET, seed, SEED_LENGTH, ITERATIONS, hash);
    for (int i = 0; i < TLS_LENGTH_VERIFY; i++) verify[i] = hash[i];
}
void TlsPrfClientFinished(uint8_t * masterSecret, uint8_t* handshakeHash, uint8_t* verify)
{
    const int SEED_LENGTH = 15 + SHA256_HASH_SIZE;
    uint8_t* seed = alloca(SEED_LENGTH);
    for (int i = 0; i < 15;               i++) seed[i     ] = "client finished"[i];
    for (int i = 0; i < SHA256_HASH_SIZE; i++) seed[i + 15] = handshakeHash[i];
    
    const int ITERATIONS = 1;
    uint8_t* hash = alloca(SHA256_HASH_SIZE * ITERATIONS);
    PrfHmacSha256(masterSecret, TLS_LENGTH_MASTER_SECRET, seed, SEED_LENGTH, ITERATIONS, hash);
    for (int i = 0; i < TLS_LENGTH_VERIFY; i++) verify[i] = hash[i];
}
