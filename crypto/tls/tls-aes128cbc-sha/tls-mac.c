#include <stdint.h>
#include "crypto/hmac/hmac-sha1.h"

/*
Calculate the MAC
seq_num + TLSCompressed.type + TLSCompressed.version + TLSCompressed.length + TLSCompressed.fragment
sequence='0000000000000000', rechdr='16 03 03', datalen='00 10'
*/

void TlsMacSha1(int      macKeyLength,
                uint8_t* macKey,
                uint64_t sequence,
                uint8_t  contentType,
                uint8_t  versionH,
                uint8_t  versionL,
                int      payloadLength,
                uint8_t* payload,
                uint8_t* mac)
{
    struct HmacSha1Struct md;
    HmacSha1Start (&md, macKey, macKeyLength);
    uint8_t prequel[8 + 5];
    uint8_t *p = prequel;
    *p++ = sequence >> 56;
    *p++ = sequence >> 48;
    *p++ = sequence >> 40;
    *p++ = sequence >> 32;
    *p++ = sequence >> 24;
    *p++ = sequence >> 16;
    *p++ = sequence >>  8;
    *p++ = sequence >>  0;
    *p++ = contentType;
    *p++ = versionH;
    *p++ = versionL;
    *p++ = payloadLength >> 8;
    *p++ = payloadLength;
    HmacSha1Add   (&md, prequel, sizeof(prequel));
    HmacSha1Add   (&md, payload, payloadLength);
    HmacSha1Finish(&md, mac);
}