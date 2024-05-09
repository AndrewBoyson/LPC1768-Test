#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "tls-session.h"
#include "lpc1768/mstimer/mstimer.h"
#include "lpc1768/hrtimer/hrtimer.h"
#include "log/log.h"

#define TLS_MAX_SESSIONS 4

static struct TlsSession sessions[TLS_MAX_SESSIONS];

static void zeroSession(struct TlsSession* p)
{
    p->id                   = 0;
    p->lastUsed             = 0;
    p->valid                = false;
    for (int i = 0; i < TLS_KEY_SIZE_MASTER; i++) p->masterSecret[i] = 0;
}

struct TlsSession* TlsSessionNew() //Never fails so never returns NULL
{
    struct TlsSession* p;
    
    //look for an empty connection
    struct TlsSession* pOldest = 0;
    uint32_t ageOldest = 0;
    for (p = sessions; p < sessions + TLS_MAX_SESSIONS; p++)
    {
        if (!p->id) goto end;   //Found an empty slot so just return it
        uint32_t age = MsTimerCount - p->lastUsed;
        if (age >= ageOldest)
        {
            ageOldest = age;
              pOldest = p;
        }
    }  
    
    //No empty ones found so use the oldest
    p = pOldest;
    
end:
    zeroSession(p);
    uint32_t id = 0;
    while (!id) id = HrTimerCount(); //This is used as a convenient unique identifier which is never zero;
    p->id       = id;
    p->lastUsed = MsTimerCount;
    return p;
}

struct TlsSession* TlsSessionOrNull(uint32_t id)
{
    if (!id) return NULL;
    for (struct TlsSession* p = sessions; p < sessions + TLS_MAX_SESSIONS; p++)
    {
        if (p->id == id) return p;
    }
    return NULL;
}
void TlsSessionReset(uint32_t id)
{
    if (!id) return; 
    for (struct TlsSession* p = sessions; p < sessions + TLS_MAX_SESSIONS; p++)
    {
        if (p->id == id) zeroSession(p);
    }
}