#include <stdlib.h>

#include "tls-connection.h"
#include "lpc1768/mstimer/mstimer.h"

#define MAX_CONNECTIONS 4

static struct TlsConnection connections[MAX_CONNECTIONS];

struct TlsConnection* TlsConnectionGetNext()
{
    static struct TlsConnection* p = connections + MAX_CONNECTIONS - 1; //Initialise to last element
    p++;
    if (p >= connections + MAX_CONNECTIONS) p = connections;
    return p;
}

static void zeroConnection(struct TlsConnection* p)
{
    p->id        = 0;
    p->lastUsed  = 0;
    p->toDo      = 0;
    p->sessionId = 0;
    p->resume    = false;
    Sha256Start(&p->handshakeSha); //This just clears any information previously calculated
    p->clientEncrypted = false;
    p->serverEncrypted = false;
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++) p->clientRandom[i] = 0;
    for (int i = 0; i < TLS_LENGTH_RANDOM; i++) p->serverRandom[i] = 0;
    for (int i = 0; i < TLS_DEFERRED_CONTENT_SIZE; i++) p->deferredContent[i] = 0;
    p->clientSequence = 0;
    p->serverSequence = 0;
    p->clientPositionInStreamOffset = 0;
    p->serverPositionInStreamOffset = 0;
    p->slotPriKeyDecryption = 0;
}

struct TlsConnection* TlsConnectionNew(int id) //Never fails so never returns NULL
{
    struct TlsConnection* p;
    
    //Look for an existing connection
    for (p = connections; p < connections + MAX_CONNECTIONS; p++)
    {
        if (p->id == id) goto end;
    }
    
    //look for an empty connection
    {
        struct TlsConnection* pOldest = 0;
        uint32_t ageOldest = 0;
        for (p = connections; p < connections + MAX_CONNECTIONS; p++)
        {
            if (!p->id) goto end;
            
            //Otherwise record the oldest and keep going
            uint32_t age = MsTimerCount - p->lastUsed;
            if (age >= ageOldest)
            {
                ageOldest = age;
                  pOldest = p;
            }
        }
        //No empty ones found so use the oldest
        p = pOldest;
    }

    
end:
    zeroConnection(p);
    p->id           = id;
    p->lastUsed     = MsTimerCount;
    return p;
}
struct TlsConnection* TlsConnectionOrNull(int id)
{
    for (struct TlsConnection* p = connections; p < connections + MAX_CONNECTIONS; p++)
    {
        if (p->id == id)
        {
            p->lastUsed = MsTimerCount;
            return p;
        }
    }
    return NULL;
}
void TlsConnectionReset(int id)
{
    for (struct TlsConnection* p = connections; p < connections + MAX_CONNECTIONS; p++)
    {
        if (p->id == id) zeroConnection(p);
    }
}

