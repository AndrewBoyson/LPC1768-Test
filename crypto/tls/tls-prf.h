#include <stdint.h>

extern void TlsPrfMasterSecret  (uint8_t * preMasterSecret, uint8_t* clientRandom, uint8_t* serverRandom, uint8_t* masterSecret);

extern void TlsPrfKeysAes128Sha1(uint8_t * masterSecret,    uint8_t* clientRandom, uint8_t* serverRandom, uint8_t* client_MAC_key,
                                                                                                          uint8_t* server_MAC_key,
                                                                                                          uint8_t* client_key,
                                                                                                          uint8_t* server_key);
                                                                             
extern void TlsPrfServerFinished(uint8_t * masterSecret,    uint8_t* handshakeHash,                       uint8_t* verify);
extern void TlsPrfClientFinished(uint8_t * masterSecret,    uint8_t* handshakeHash,                       uint8_t* verify);
