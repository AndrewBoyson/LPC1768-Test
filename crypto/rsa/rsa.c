#include <stdint.h>
#include <stdbool.h>
#include "rsa.h"
#include "crypto/bignum/bignum.h"
#include "lpc1768/hrtimer/hrtimer.h"
#include "log/log.h"

typedef enum status_e
{
    STATUS_NONE,
    STATUS_FINISHED,
    STATUS_STARTED,
    STATUS_CALCULATE_M1,
    STATUS_CALCULATE_M2,
    STATUS_CALCULATE_HBIS,
    STATUS_CALCULATE_H,
    STATUS_CALCULATE_R
} status_t;

typedef struct slot_s
{
    uint32_t m1[16];
    uint32_t m2[16];
    uint32_t n1[16];
    uint32_t n2[16];
    uint32_t e1[16];
    uint32_t e2[16];
    uint32_t r1[16];
    uint32_t r2[16];
    uint32_t qi[16];
    uint32_t  r[32];
    status_t status;
} slot_t;
#define MAX_COUNT 4
static slot_t slots[MAX_COUNT];

static uint32_t  hbis[32];
static uint32_t  h[16];


void start(slot_t* pSlot, uint32_t* message, uint32_t* p, uint32_t* q, uint32_t* dp, uint32_t* dq, uint32_t* qInv)
{
    BnModExpStart512(pSlot->m1, pSlot->e1, pSlot->n1, pSlot->r1, 1024, message, dp, p);
    BnModExpStart512(pSlot->m2, pSlot->e2, pSlot->n2, pSlot->r2, 1024, message, dq, q);
    BnCpy512(pSlot->qi, qInv);
    pSlot->status = STATUS_STARTED;
}
void iterate(slot_t* pSlot)
{
    /*
    m1 = c^dP mod p
    m2 = c^dQ mod q
    h = qInv.(m1 - m2) mod p
    m = m2 + h.q 
    */
    
    switch (pSlot->status)
    {
        case STATUS_NONE:
        case STATUS_FINISHED:
        {
            break;
        }
        case STATUS_STARTED:
        {
            pSlot->status = STATUS_CALCULATE_M1;
            break;
        }
        case STATUS_CALCULATE_M1:
        {
            bool finished = BnModExpIterate512(pSlot->m1, pSlot->e1, pSlot->n1, pSlot->r1);
            if (finished) pSlot->status = STATUS_CALCULATE_M2;
            break;
        }
        case STATUS_CALCULATE_M2:
        {
            bool finished = BnModExpIterate512(pSlot->m2, pSlot->e2, pSlot->n2, pSlot->r2);
            if (finished) pSlot->status = STATUS_CALCULATE_HBIS;
            break;
        }
        case STATUS_CALCULATE_HBIS:
        {
            uint32_t acc512[16];
            BnCpy512(acc512, pSlot->r1);
            if (BnCmp512(pSlot->r1, pSlot->r2) < 0) BnAdd512(acc512, pSlot->n1); // if m1 < m2 add p to keep positive
            BnSub512(acc512, pSlot->r2);
            Bn512Mul1024(pSlot->qi, acc512, hbis);
            pSlot->status = STATUS_CALCULATE_H;
            break;
        }
        case STATUS_CALCULATE_H:
        {
            BnRem512(1024, hbis, pSlot->n1, h);
            pSlot->status = STATUS_CALCULATE_R;
            break;
        }
        case STATUS_CALCULATE_R:
        {
            uint32_t hq1024[32];
            Bn512Mul1024(h, pSlot->n2, hq1024);
            BnZer1024(pSlot->r);
            BnCpy512(pSlot->r, pSlot->r2);
            BnAdd1024(pSlot->r, hq1024);
            pSlot->status = STATUS_FINISHED;
            break;
        }
    }
}

bool RsaFinished(int slotIndex)
{
    return slots[slotIndex].status == STATUS_FINISHED;
}
uint32_t* RsaResult(int slotIndex)
{
    return slots[slotIndex].r;
}
void RsaClear(int slotIndex) //This is for security - call it as soon as you no longer need the result.
{
    slot_t* pSlot = slots + slotIndex;
    pSlot->status = STATUS_NONE;
    BnZer512 (pSlot->m1);
    BnZer512 (pSlot->m2);
    BnZer512 (pSlot->e1);
    BnZer512 (pSlot->e2);
    BnZer512 (pSlot->n1);
    BnZer512 (pSlot->n2);
    BnZer512 (pSlot->r1);
    BnZer512 (pSlot->r2);
    BnZer512 (pSlot->qi);
    BnZer1024(pSlot->r);
}
int RsaStart(uint32_t* message, uint32_t* p, uint32_t* q, uint32_t* dp, uint32_t* dq, uint32_t* qInv) //Returns the slot or -1 on failure - you must check!
{
    //If the exponent is empty then bomb out
    if (BnIse1024(dp))
    {
        LogTime("Fast - empty dp\r\n");
        return -1;
    }
    if (BnIse1024(dq))
    {
        LogTime("Fast - empty dq\r\n");
        return -1;
    }
    
    //Look for an empty slot
    for (slot_t* pSlot = slots; pSlot < slots + MAX_COUNT; pSlot++)
    {
        if (pSlot->status == STATUS_NONE)
        {            
            start(pSlot, message, p, q, dp, dq, qInv);
            return pSlot - slots;
        }
    }
    
    //Look for a slot whch has been used and not cleared
    for (slot_t* pSlot = slots; pSlot < slots + MAX_COUNT; pSlot++)
    {
        if (pSlot->status == STATUS_FINISHED)
        {            
            start(pSlot, message, p, q, dp, dq, qInv);
            return pSlot - slots;
        }
    }
    
    //No available slot so bomb out
    LogTimeF("RsaFastStart - no available slots out of %d\r\n", MAX_COUNT);
    return -1;

}
void RsaMain()
{
    //Always complete existing calculations first
    slot_t* pHighestSlot = 0;
    int highestStatus = STATUS_FINISHED;
    
    for (slot_t* pSlot = slots; pSlot < slots + MAX_COUNT; pSlot++)
    {
        if (pSlot->status > highestStatus)
        {
            highestStatus = pSlot->status;
            pHighestSlot = pSlot;
        }
    }
    if (highestStatus > STATUS_FINISHED) iterate(pHighestSlot);
}

void RsaInit(void)
{
    for (int i = 0; i < MAX_COUNT; i++) slots[i].status = STATUS_NONE;
}