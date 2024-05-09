#include <stdint.h>

#include "crypto/sha/sha256.h"

#define TLS_KEY_SIZE_MASTER 48

struct TlsSession
{
    uint32_t id;
    uint32_t lastUsed;
    bool     valid;
    
    uint8_t  masterSecret  [TLS_KEY_SIZE_MASTER];
};

extern struct TlsSession* TlsSessionNew   (void);        //Never fails so never returns NULL
extern struct TlsSession* TlsSessionOrNull(uint32_t id); //Returns NULL if not found
extern void               TlsSessionReset (uint32_t id);