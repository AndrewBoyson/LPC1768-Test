#include <stdbool.h>
#include <stdint.h>

#include "tls-defs.h"
#include "crypto/sha/sha1.h"
#include "crypto/sha/sha256.h"
#include "crypto/aes-cbc/aes128cbc.h"

#define DO_WAIT_CLIENT_HELLO                0
#define DO_SEND_SERVER_HELLO_NEW            1
#define DO_SEND_SERVER_HELLO_RESUME         2
#define DO_WAIT_CLIENT_CHANGE               3
#define DO_WAIT_DECRYPT_MASTER_SECRET       4
#define DO_SEND_SERVER_CHANGE               5
#define DO_APPLICATION                      6
#define DO_SEND_ALERT_ILLEGAL_PARAMETER     7
#define DO_SEND_ALERT_INTERNAL_ERROR        8

#define TLS_DEFERRED_CONTENT_SIZE 100

struct TlsConnection
{
    int                id; //An id of zero means the record is empty
    uint32_t           lastUsed;
    int                toDo;
    uint32_t           sessionId;
    bool               resume;
    struct Sha256State handshakeSha;
    bool               clientEncrypted;
    bool               serverEncrypted;
    uint8_t            clientRandom[TLS_LENGTH_RANDOM];
    uint8_t            serverRandom[TLS_LENGTH_RANDOM];
    uint8_t            clientHandshakeHash[SHA256_HASH_SIZE];
    uint8_t            deferredContent[TLS_DEFERRED_CONTENT_SIZE];
    uint64_t           clientSequence;
    uint64_t           serverSequence;
    uint32_t           clientPositionInStreamOffset;
    uint32_t           serverPositionInStreamOffset;
    
    int                slotPriKeyDecryption;
    uint8_t            clientMacKey  [SHA1_HASH_SIZE];
    uint8_t            serverMacKey  [SHA1_HASH_SIZE];
    uint8_t            clientWriteKey[AES128CBC_BLOCK_SIZE];
    uint8_t            serverWriteKey[AES128CBC_BLOCK_SIZE];    
};

extern struct TlsConnection* TlsConnectionGetNext(void);

extern struct TlsConnection* TlsConnectionNew   (int id); //Never fails so never returns NULL
extern struct TlsConnection* TlsConnectionOrNull(int id);
extern void                  TlsConnectionReset (int id);
