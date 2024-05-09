#include <stdint.h>
#include <stdbool.h>

#include "rsa.h"
#include "crypto/bignum/bignum.h"
#include "lpc1768/hrtimer/hrtimer.h"
#include "log/log.h"

#define STATUS_NONE     0
#define STATUS_STARTED  1
#define STATUS_FINISHED 2

#define MAX_COUNT 4

static uint32_t m[MAX_COUNT][32];
static uint32_t e[MAX_COUNT][32];
static uint32_t n[MAX_COUNT][32];
static uint32_t r[MAX_COUNT][32];

static int status[MAX_COUNT];

bool RsaSlowFinished(int slot)
{
    return status[slot] == STATUS_FINISHED;
}

uint32_t* RsaSlowResult(int slot)
{
    return r[slot];
}
void RsaSlowClear(int slot) //This is for security - call it as soon as you no longer need the result.
{
    status[slot] = STATUS_NONE;
    BnZer1024(m[slot]);
    BnZer1024(e[slot]);
    BnZer1024(n[slot]);
    BnZer1024(r[slot]);
}
int RsaSlowStart(uint32_t* message, uint32_t* exponent, uint32_t* modulus) //Returns the slot or -1 on failure - you must check!
{
    //If the exponent is empty then bomb out
    if (BnIse1024(exponent))
    {
        LogTime("RsaSlowStart - empty exponent\r\n");
        return -1;
    }
    
    //Look for an empty slot
    int slot = 0;
    while (slot < MAX_COUNT)
    {
        if (status[slot] == STATUS_NONE) goto found;
        slot++;
    }
    
    //Look for a slot whch has been used and not cleared
    slot = 0;
    while (slot < MAX_COUNT)
    {
        if (status[slot] == STATUS_FINISHED) goto found;
        slot++;
    }
    
    //No available slot so bomb out
    LogTimeF("RsaSlowStart - no available slots out of %d\r\n", MAX_COUNT);
    return -1;
    
    //Start the calculation
found:
    BnModExpStart1024(m[slot], e[slot], n[slot], r[slot], 1024, message, exponent, modulus);
    status[slot] = STATUS_STARTED;
    return slot;
}
void RsaSlowMain()
{
    int slot = 0;
    while (slot < MAX_COUNT)
    {
        if (status[slot] == STATUS_STARTED) goto found;
        slot++;
    }
    return;
    
found:
    ;
    bool finished = BnModExpIterate1024(m[slot], e[slot], n[slot], r[slot]);

    if (finished) status[slot] = STATUS_FINISHED;
}

void RsaSlowInit(void)
{
    for (int i = 0; i < MAX_COUNT; i++) status[i] = STATUS_NONE;
}