#include <stdbool.h>
#include <stdint.h>
#include "bignum.h"

void Bn1024Mul2048(uint32_t* a1024, uint32_t* b1024, uint32_t* r2048)
{
    uint32_t row2048[64];
    uint32_t tmp[64];
    BnZer2048(tmp);
    int i, j;

    BnZer2048(r2048);

    for (i = 0; i < 32; ++i)
    {
        uint32_t aData = a1024[i];
        if (aData)
        {
            BnZer2048(row2048);
            for (j = 0; j < 32; ++j)
            {
                uint32_t bData = b1024[j];
                if (bData)
                {
                    uint64_t intermediate = (uint64_t)aData * (uint64_t)bData;
                    tmp[i + j    ] = intermediate;
                    tmp[i + j + 1] = intermediate >> 32;
                    BnAdd2048(row2048, tmp);
                    tmp[i + j    ] = 0;
                    tmp[i + j + 1] = 0;
                }
            }
            BnAdd2048(r2048, row2048);
        }
    }
}

void Bn512Mul1024(uint32_t* a512, uint32_t* b512, uint32_t* r1024)
{
    uint32_t row1024[32];
    uint32_t tmp[32];
    BnZer1024(tmp);
    int i, j;

    BnZer1024(r1024);

    for (i = 0; i < 16; ++i)
    {
        uint32_t aData = a512[i];
        if (aData)
        {
            BnZer1024(row1024);
            for (j = 0; j < 16; ++j)
            {
                uint32_t bData = b512[j];
                if (bData)
                {
                    uint64_t intermediate = (uint64_t)aData * (uint64_t)bData;
                    tmp[i + j    ] = intermediate;
                    tmp[i + j + 1] = intermediate >> 32;
                    BnAdd1024(row1024, tmp);
                    tmp[i + j    ] = 0;
                    tmp[i + j + 1] = 0;
                }
            }
            BnAdd1024(r1024, row1024);
        }
    }
}

/*
N / D = ( Q , R ) , where
    N = Numerator (dividend)
    D = Denominator (divisor)
and
    Q = Quotient
    R = Remainder

if D = 0 then error(DivisionByZeroException) end
Q := 0                  -- Initialize quotient and remainder to zero
R := 0                     
for i := n − 1 .. 0 do  -- Where n is number of bits in N
  R := R << 1           -- Left-shift R by 1 bit
  R(0) := N(i)          -- Set the least-significant bit of R equal to bit i of the numerator
  if R ≥ D then         -- If D fits into R
    R := R − D
    Q(i) := 1
  end
end
*/
void BnRem1024(int numSize, uint32_t* num, uint32_t* d1024, uint32_t* r1024)
{
    BnZer1024(r1024);                  
    for (int i = numSize - 1; i >= 0; --i)
    {
        bool rBit0000 = (num[i / 32] >> (i % 32)) & 1;
        bool rBit1025 = BnShl1024(r1024, rBit0000);
        if (rBit1025 || BnCmp1024(r1024, d1024) != -1)
        {
            BnSub1024(r1024, d1024);
        }
    }
}
void BnRem512(int numSize, uint32_t* num, uint32_t* d512, uint32_t* r512)
{
    BnZer512(r512);                  
    for (int i = numSize - 1; i >= 0; --i)
    {
        bool rBit0000 = (num[i / 32] >> (i % 32)) & 1;
        bool rBit513 = BnShl512(r512, rBit0000);
        if (rBit513 || BnCmp512(r512, d512) != -1)
        {
            BnSub512(r512, d512);
        }
    }
}