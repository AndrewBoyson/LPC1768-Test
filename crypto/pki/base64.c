#include <stdbool.h>
#include <stdio.h>
#include "base64.h"

char (*Base64ReadNextCharFunctionPointer)(void) = NULL;

static int get6bits()
{
    while (true)
    {
        char c = Base64ReadNextCharFunctionPointer();
        if      (c ==  0 ) return -1; //EOF or an error
        else if (c == '-') return -1; //Finished
        else if (c == '=') return -1; //Padding  
        else if (c == '/') return 63;
        else if (c == '+') return 62;
        else if (c >= 'a') return c - 'a' + 26;
        else if (c >= 'A') return c - 'A' +  0;
        else if (c >= '0') return c - '0' + 52;
        else continue;
    }
}

static int last6 = 0;
static int posn = 0;
static bool finished = false;
int Base64ReadByte()
{
    if (finished) return -1;
    
    /*
    L = last
    T = this
                out
    case 0    TTTTTTLL
    case 1    LLLLTTTT
    case 2    LLTTTTTT
    */
    int this6 = 0;
    int bits8 = 0;
    
    switch (posn)
    {
        case 0: //We have no bits leftover
        {
            this6 = get6bits(); if (this6 == -1)  return -1; //EOF or an error
            last6 = get6bits();
            if (last6 == -1)
            {
                last6 = 0;
                finished = true;
            }
            bits8  = this6 << 2;
            bits8 &= 0xFF;
            bits8 |= last6 >> 4;
            
            posn = 1;
            break;
        }
        case 1: //We have 4 bits leftover
        {
            this6 = get6bits(); if (this6 == -1)  return -1; //EOF or an error
            
            bits8  = last6 << 4;
            bits8 &= 0xFF;
            bits8 |= this6 >> 2;
            
            last6 = this6;
            
            posn = 2;
            break;
        }
        case 2: //We have 2 bits leftover
        {
            this6 = get6bits(); if (this6 == -1)  return -1; //EOF or an error
            
            bits8  = last6 << 6;
            bits8 &= 0xFF;
            bits8 |= this6;
            
            posn = 0;
            break;
        }
    }
    
    return bits8;
}
int Base64SkipLine()
{
    last6 = 0;
    posn = 0;
    finished = false;
    
    while (true)
    {
        char c = Base64ReadNextCharFunctionPointer();
        if      (c ==   0 ) return -1; //EOF or an error
        else if (c == '\n') return  0; //EOL
    }
}
