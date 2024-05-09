#include <stdint.h>
#include <stdbool.h>

#include "bignum.h"
/*
UInt64 result = 1;
message = message % modulus;
while (exponent != 0)
{
    if ((exponent & 1) == 1) result = (result * message) % modulus;
    message = (message * message) % modulus;
    exponent = exponent >> 1;
}
return result;
*/

void BnModExpStart1024(uint32_t m[32], uint32_t e[32], uint32_t n[32], uint32_t r[32], int messageSize, uint32_t* message, uint32_t* exponent, uint32_t* modulus)
{
    BnRem1024(messageSize, message, modulus, m);
    BnCpy1024(e, exponent);
    BnCpy1024(n, modulus);
    BnZer1024(r);
    r[0] = 1;
}
bool BnModExpIterate1024(uint32_t m[32], uint32_t e[32], uint32_t n[32], uint32_t r[32]) //Returns true if finished
{
    uint32_t temp[64];
    
    if (e[0] & 1)
    {
        Bn1024Mul2048(r, m, temp);
        BnRem1024(2048, temp, n, r);
    }
    Bn1024Mul2048(m, m, temp);
    BnRem1024(2048, temp, n, m);
    
    BnShr1024(e, false);
    return BnIse1024(e);
}

void BnModExpStart512(uint32_t m[16], uint32_t e[16], uint32_t n[16], uint32_t r[16], int messageSize, uint32_t* message, uint32_t* exponent, uint32_t* modulus)
{
    BnRem512(messageSize, message, modulus, m);
    BnCpy512(e, exponent);
    BnCpy512(n, modulus);
    BnZer512(r);
    r[0] = 1;
}
bool BnModExpIterate512(uint32_t m[16], uint32_t e[16], uint32_t n[16], uint32_t r[16]) //Returns true if finished
{
    uint32_t temp[32];
    
    if (e[0] & 1)
    {
        Bn512Mul1024(r, m, temp);
        BnRem512(1024, temp, n, r);
    }
    Bn512Mul1024(m, m, temp);
    BnRem512(1024, temp, n, m);
    
    BnShr512(e, false);
    return BnIse512(e);
}
