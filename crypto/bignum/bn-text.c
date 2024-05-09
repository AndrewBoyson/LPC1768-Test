#include <stdbool.h>
#include <stdint.h>
#include "bignum.h"
#include "web/http/http.h"

void BnAsHttp2048(uint32_t* n)
{
    for (int i = 63; i >= 56; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 55; i >= 48; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 47; i >= 40; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 39; i >= 32; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 31; i >= 24; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 23; i >= 16; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 15; i >=  8; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i =  7; i >=  0; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
}
void BnParseHex2048(uint32_t* n, const char* text)
{
    const char* p = text;
    
    //Find the least significant digit in the text
    while (*p) p++; //Position ourselves beyond the last nibble in the string
    
    BnZer2048(n);
    int word  = 0;
    int shift = 0;
    while(1)
    {
        p--;
        if (p < text) break; //Stop when we have reversed beyond the start of the string
        
        int byte = 0;
        if      (*p >= 'a' && *p <= 'f') byte = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'F') byte = *p - 'A' + 10;
        else if (*p >= '0' && *p <= '9') byte = *p - '0';
        n[word] |= byte << shift;
        shift += 4;
        if (shift >= 32)
        {
            shift = 0;
            word++;
            if (word >= 64) break; //Stop if beyond the size of the destination
        }
    }
}

void BnAsHttp1024(uint32_t* n)
{
    for (int i = 31; i >= 24; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 23; i >= 16; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i = 15; i >=  8; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
    for (int i =  7; i >=  0; i--) HttpAddInt32AsHex(n[i]); HttpAddText("\r\n");
}
void BnParseHex1024(uint32_t* n, const char* text)
{
    const char* p = text;
    
    //Find the least significant digit in the text
    while (*p) p++; //Position ourselves beyond the last nibble in the string
    
    BnZer1024(n);
    int word  = 0;
    int shift = 0;
    while(1)
    {
        p--;
        if (p < text) break; //Stop when we have reversed beyond the start of the string
        
        int byte = 0;
        if      (*p >= 'a' && *p <= 'f') byte = *p - 'a' + 10;
        else if (*p >= 'A' && *p <= 'F') byte = *p - 'A' + 10;
        else if (*p >= '0' && *p <= '9') byte = *p - '0';
        n[word] |= byte << shift;
        shift += 4;
        if (shift >= 32)
        {
            shift = 0;
            word++;
            if (word >= 32) break; //Stop if beyond the size of the destination
        }
    }
}