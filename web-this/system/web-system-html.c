#include <stdio.h>
#include <string.h>

#include "web/http/http.h"
#include "web-this/web-nav-this.h"
#include "web/web-add.h"
#include "log/log.h"
#include "lpc1768/bignum/bignum.h"
#include "crypto/rsa/rsa.h"
#include "crypto/sha/sha256.h"
#include "crypto/hmac/hmac-sha256.h"
#include "crypto/tls/tls-prf.h"
#include "crypto/aes-cbc/aes128cbc.h"
#include "crypto/sha/sha1.h"
#include "crypto/pki/pri-key.h"

void WebSystemHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("System", "settings.css", "system.js");
    WebAddNav(SYSTEM_PAGE);
    WebAddH1("System");

    WebAddH2("TFTP");
    WebAddAjaxInput   ("Server url",                      5, "ajax-server-name",   "tftpserver"   );
    WebAddAjaxInput   ("File (strftime)",                11, "ajax-file-name",     "tftpfilename" );
    WebAddAjaxInput   ("Interval (secs) 0=no",            5, "ajax-read-interval", "tftpreadint"  );
    WebAddAjaxInput   ("Records per backup 0=no",         5, "ajax-write-size",    "tftpwriteint" );
    WebAddAjaxLabelled("Count",                              "ajax-count"                         );
    WebAddAjaxLabelled("Started",                            "ajax-start-time"                    );

    WebAddH2("FRAM");
    WebAddAjaxLabelled("Used",                               "ajax-fram-used"                     );
    
    WebAddH2("Compiler");
    WebAddLabelledText("Version", __VERSION__);
        
    WebAddH2("AES 128 test");
    uint8_t aeskey[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t in[]  = { 0x76, 0x49, 0xab, 0xac, 0x81, 0x19, 0xb2, 0x46, 0xce, 0xe9, 0x8e, 0x9b, 0x12, 0xe9, 0x19, 0x7d,
                      0x50, 0x86, 0xcb, 0x9b, 0x50, 0x72, 0x19, 0xee, 0x95, 0xdb, 0x11, 0x3a, 0x91, 0x76, 0x78, 0xb2,
                      0x73, 0xbe, 0xd6, 0xb8, 0xe3, 0xc1, 0x74, 0x3b, 0x71, 0x16, 0xe6, 0x9e, 0x22, 0x22, 0x95, 0x16,
                      0x3f, 0xf1, 0xca, 0xa1, 0x68, 0x1f, 0xac, 0x09, 0x12, 0x0e, 0xca, 0x30, 0x75, 0x86, 0xe1, 0xa7 };
    uint8_t iv[]  = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
    /*
    uint8_t out[] = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a,
                      0xae, 0x2d, 0x8a, 0x57, 0x1e, 0x03, 0xac, 0x9c, 0x9e, 0xb7, 0x6f, 0xac, 0x45, 0xaf, 0x8e, 0x51,
                      0x30, 0xc8, 0x1c, 0x46, 0xa3, 0x5c, 0xe4, 0x11, 0xe5, 0xfb, 0xc1, 0x19, 0x1a, 0x0a, 0x52, 0xef,
                      0xf6, 0x9f, 0x24, 0x45, 0xdf, 0x4f, 0x9b, 0x17, 0xad, 0x2b, 0x41, 0x7b, 0xe6, 0x6c, 0x37, 0x10 };
    */

    Aes128CbcDecrypt(aeskey, iv, in, 64);
    HttpAddText("<code>");
    HttpAddBytesAsHex(in, sizeof(in));
    HttpAddText("</code>");
    
    WebAddH2("SHA 256 stream test");
    uint8_t hash[32];
    char* input = "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu";
    struct Sha256State sha256state;
    Sha256Start (&sha256state);
    Sha256Add   (&sha256state, (uint8_t*)input, strlen(input));
    Sha256Finish(&sha256state, (uint8_t*)hash);

    HttpAddText("<code>");
    HttpAddBytesAsHex(hash, sizeof(hash));
    HttpAddText("</code>");
    
    WebAddH2("SHA1 stream test");
    struct Sha1State sha1state;
    Sha1Start (&sha1state);
    Sha1Add   (&sha1state, (uint8_t*)input, strlen(input));
    Sha1Finish(&sha1state, (uint8_t*)hash);

    HttpAddText("<code>");
    HttpAddBytesAsHex(hash, SHA1_HASH_SIZE);
    HttpAddText("</code>");
    
    WebAddH2("HMAC SHA 256 test");
    char* key = "Jefe";
    char* msg = "what do ya want for nothing?";
    uint8_t mac[32];
    HmacSha256((uint8_t*)key, strlen(key), (uint8_t*)msg, strlen(msg), mac);
    HttpAddText("<code>");
    HttpAddBytesAsHex(mac, sizeof(mac));
    HttpAddText("</code>");
    
    WebAddH2("HMAC SHA 256 stream test");
    struct HmacSha256Struct hmacState;
    HmacSha256Start (&hmacState, (uint8_t*)key, strlen(key));
    HmacSha256Add   (&hmacState, (uint8_t*)msg, strlen(msg));
    HmacSha256Finish(&hmacState, mac);
    HttpAddText("<code>");
    HttpAddBytesAsHex(mac, sizeof(mac));
    HttpAddText("</code>");
    
    WebAddH2("Verify data test");
    uint8_t masterSecret  [48] = {0x91, 0x6a, 0xbf, 0x9d, 0xa5, 0x59, 0x73, 0xe1,
                                  0x36, 0x14, 0xae, 0x0a, 0x3f, 0x5d, 0x3f, 0x37,
                                  0xb0, 0x23, 0xba, 0x12, 0x9a, 0xee, 0x02, 0xcc,
                                  0x91, 0x34, 0x33, 0x81, 0x27, 0xcd, 0x70, 0x49,
                                  0x78, 0x1c, 0x8e, 0x19, 0xfc, 0x1e, 0xb2, 0xa7,
                                  0x38, 0x7a, 0xc0, 0x6a, 0xe2, 0x37, 0x34, 0x4c};
                                  
    uint8_t hashHandshakes[32] = {0xb2, 0x01, 0x7b, 0xa2, 0x8d, 0x0e, 0x27, 0xf0, 
                                  0x3a, 0xe3, 0x27, 0x45, 0x6b, 0x6f, 0xf0, 0x0b,
                                  0x4d, 0x5b, 0xbf, 0x0e, 0xf7, 0xcd, 0xa8, 0x3c,
                                  0xe1, 0x02, 0x9b, 0x52, 0x1c, 0x3e, 0x7c, 0x35};
    uint8_t verifyData[12];
    TlsPrfServerFinished(masterSecret, hashHandshakes, verifyData);
    
    HttpAddText("<code>");
    HttpAddBytesAsHex(verifyData, sizeof(verifyData));
    HttpAddText("</code>");
    
    
    WebAddH2("Key test");
    
    uint8_t clientRandom[] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                               0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                               0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                               0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f };
                               
    uint8_t serverRandom[] = { 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
                               0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f,
                               0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                               0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f };

    uint8_t client_MAC_key_20[20];
    uint8_t server_MAC_key_20[20];
    uint8_t client_key_16[16];
    uint8_t server_key_16[16];
    TlsPrfKeysAes128Sha1(masterSecret, clientRandom, serverRandom, client_MAC_key_20, server_MAC_key_20, client_key_16, server_key_16);
    HttpAddText("<code>");
    HttpAddBytesAsHex(client_MAC_key_20, sizeof(client_MAC_key_20)); HttpAddText("\r\n");
    HttpAddBytesAsHex(server_MAC_key_20, sizeof(server_MAC_key_20)); HttpAddText("\r\n");
    HttpAddBytesAsHex(    client_key_16, sizeof(    client_key_16)); HttpAddText("\r\n");
    HttpAddBytesAsHex(    server_key_16, sizeof(    server_key_16));
    HttpAddText("</code>");

    
    
    char* m =
"0001FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF"
"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00302130"
"0906052B0E03021A05000414A9993E364706816ABA3E25717850C26C9CD0D89D";

    uint32_t message [32];
    BnParseHex1024(message,  m);
    
    /*
    WebAddH2("RSA test");

    char* n =
"E08973398DD8F5F5E88776397F4EB005BB5383DE0FB7ABDC7DC775290D052E6D"
"12DFA68626D4D26FAA5829FC97ECFA82510F3080BEB1509E4644F12CBBD832CF"
"C6686F07D9B060ACBEEE34096A13F5F7050593DF5EBA3556D961FF197FC981E6"
"F86CEA874070EFAC6D2C749F2DFA553AB9997702A648528C4EF357385774575F";

    char* d =
"00A403C327477634346CA686B57949014B2E8AD2C862B2C7D748096A8B91F736"
"F275D6E8CD15906027314735644D95CD6763CEB49F56AC2F376E1CEE0EBF282D"
"F439906F34D86E085BD5656AD841F313D72D395EFE33CBFF29E4030B3D05A28F"
"B7F18EA27637B07957D32F2BDE8706227D04665EC91BAF8B1AC3EC9144AB7F21";

    uint32_t exponent[32];
    uint32_t modulus [32];
    
    BnParseHex1024(exponent, d);
    BnParseHex1024(modulus,  n);

    static int ticket = 0;
    static bool started = false;
    if (!started)
    {
        ticket = RsaSlowStart(message, exponent, modulus);
        started = true;
    }
    
    HttpAddText("<code>");
    HttpAddText("Message\r\n");
    BnAsHttp1024(message);
    HttpAddText("\r\nExponent\r\n");
    BnAsHttp1024(exponent);
    HttpAddText("\r\nModulus\r\n");
    BnAsHttp1024(modulus);
    HttpAddText("\r\nResult\r\n");
    if (ticket >= 0)
    {
        if (RsaSlowFinished(ticket)) HttpAddText("Finished\r\n");
        else                         HttpAddText("Not finished\r\n");
        BnAsHttp1024(RsaSlowResult(ticket));
    }
    else
    {
        HttpAddText("No ticket available to calculate result\r\n");
    }
    HttpAddText("</code>");
    */
    
    WebAddH2("RSA test");
    static int rsaTicket = 0;
    static bool rsaSlowStarted = false;
    if (!rsaSlowStarted)
    {
        rsaTicket = PriKeyDecryptStart((uint8_t*)message);
        rsaSlowStarted = true;
    }
    
    HttpAddText("<code>");
    HttpAddText("Message\r\n");
    BnAsHttp1024(message);
    HttpAddText("\r\nResult\r\n");
    if (rsaTicket >= 0)
    {
        if (PriKeyDecryptFinished(rsaTicket)) HttpAddText("Finished\r\n");
        else                                  HttpAddText("Not finished\r\n");
        BnAsHttp1024((uint32_t*)PriKeyDecryptResultLittleEndian(rsaTicket));
    }
    else
    {
        HttpAddText("No ticket available to calculate result\r\n");
    }
    HttpAddText("</code>");
    
    WebAddEnd();
}
