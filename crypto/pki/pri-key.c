#include <stdint.h>

#include "base64.h"
#include "log/log.h"
#include "crypto/rsa/rsa.h"

/*
RSAPrivateKey ::= SEQUENCE {
      version           INTEGER,
      modulus           INTEGER,  -- n
      publicExponent    INTEGER,  -- e
      privateExponent   INTEGER,  -- d
      prime1            INTEGER,  -- p
      prime2            INTEGER,  -- q
      exponent1         INTEGER,  -- d mod (p-1)
      exponent2         INTEGER,  -- d mod (q-1)
      coefficient       INTEGER,  -- (inverse of q) mod p
      otherPrimeInfos   OtherPrimeInfos OPTIONAL
*/

const char* PriKeySourcePtr = 0;

uint8_t v   [  4]; //version
uint8_t n   [128]; //modulus
uint8_t e   [  4]; //publicExponent
uint8_t d   [128]; //privateExponent
uint8_t p   [ 64]; //prime1
uint8_t q   [ 64]; //prime2
uint8_t dp  [ 64]; //exponent1 -- d mod (p-1)
uint8_t dq  [ 64]; //exponent2 -- d mod (q-1)
uint8_t invq[ 64]; //coefficient -- (inverse of q) mod p

static const char* pNext;

static char readChar()
{
    char c = *pNext;
    if (!c) return 0;
    pNext++;
    return c;
}
static int readLength()
{
    int c = Base64ReadByte();
    if (c ==   -1) return -1; //EOF or an error
    if (c  < 0x80) return  c; //Single byte length
    if (c == 0x80) return -1; //indefinite form - do not use
    int numberOfBytes = c - 0x80;
    int len = 0;
    for (int i = 0; i < numberOfBytes; i++)
    {
        len <<= 8;
        c = Base64ReadByte();
        if (c == -1) return -1; //EOF or an error
        len |= c;
    }
    return len;
}
static int readSequence()
{
    int c = Base64ReadByte();
    if (c ==   -1) return -1;  //EOF or an error
    if (c != 0x30) return -1;  //Not SEQUENCE type
    int len = readLength();
    return len;
}
static int readData(uint8_t* data, int size) //Reads the data from big-endian 'pem' format into little-endian 'bn' format
{
    int c = Base64ReadByte();
    if (c ==   -1) return -1;            //EOF or an error
    if (c != 0x02) return -1;            //Not INTEGER type
    int pemLength = readLength();
    if (pemLength == -1)   return -1;    //EOF or an error
    
    int finalLength = pemLength;
    for (int i = 0; i < pemLength; i++)
    {
        c = Base64ReadByte();
        if (c == -1) return -1;          //EOF or an error
        if (!i && !c)                    //Don't add if this is the msb and that byte is zero but reduce the final length by 1
        {
            finalLength--;
        }
        else
        {
            data[pemLength - 1 - i] = c; //If pemLength is one too big then 'i' will also be one too big so the result is correct
        }
    }
    for (int i = finalLength; i < size; i++) data[i] = 0;
    
    return finalLength + 1;
}
void PriKeyInit()
{
    pNext = PriKeySourcePtr;
    Base64ReadNextCharFunctionPointer = readChar;
    
    int r = 0;
    r = Base64SkipLine();             if (r < 0) return;
    r = readSequence();               if (r < 0) return;
    
    r = readData(   v, sizeof(   v)); if (r < 0) return;
    r = readData(   n, sizeof(   n)); if (r < 0) return;
    r = readData(   e, sizeof(   e)); if (r < 0) return;
    r = readData(   d, sizeof(   d)); if (r < 0) return;
    r = readData(   p, sizeof(   p)); if (r < 0) return;
    r = readData(   q, sizeof(   q)); if (r < 0) return;
    r = readData(  dp, sizeof(  dp)); if (r < 0) return;
    r = readData(  dq, sizeof(  dq)); if (r < 0) return;
    r = readData(invq, sizeof(invq)); if (r < 0) return;
    
/*    
    Log("Primary key content\r\n");
    LogBytesAsHex(   v, sizeof(   v)); Log("\n\n");
    LogBytesAsHex(   n, sizeof(   n)); Log("\n\n");
    LogBytesAsHex(   e, sizeof(   e)); Log("\n\n");
    LogBytesAsHex(   d, sizeof(   d)); Log("\n\n");
    LogBytesAsHex(   p, sizeof(   p)); Log("\n\n");
    LogBytesAsHex(   q, sizeof(   q)); Log("\n\n");
    LogBytesAsHex(  dp, sizeof(  dp)); Log("\n\n");
    LogBytesAsHex(  dq, sizeof(  dq)); Log("\n\n");
    LogBytesAsHex(invq, sizeof(invq)); Log("\n\n");
*/
}
int PriKeyDecryptStart(uint8_t* message) //return the slot number for the decryption
{
    //Convert message to big number (little endian) format prior to decryption
    uint8_t leMessage[128];
    for (int i = 0; i < 128; i++) leMessage[127 - i] = message[i];
    return RsaStart((uint32_t*)leMessage, (uint32_t*)p, (uint32_t*)q, (uint32_t*)dp, (uint32_t*)dq, (uint32_t*)invq);
}
bool PriKeyDecryptFinished(int slot)
{
    return RsaFinished(slot);
}
uint8_t* PriKeyDecryptResultLittleEndian(int slot)
{
    return (uint8_t*) RsaResult(slot);
}
void PriKeyDecryptResultTail(int slot, int length, uint8_t* pBuffer)
{
    //Convert result from big number (little endian) format after decryption
    uint8_t* pLittleEndian = (uint8_t*) RsaResult(slot);
    for (int i = 0; i < length; i++) *(pBuffer + i) = *(pLittleEndian + length - 1 - i);
}
void PriKeyDecryptClear(int slot)
{
    RsaClear(slot);
}