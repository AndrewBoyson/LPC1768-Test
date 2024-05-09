#include <stdarg.h>

#include "tls.h"
#include "tls-defs.h"
#include "log/log.h"

void TlsLogContentType(char contentType)
{
    switch (contentType)
    {
        case TLS_CONTENT_TYPE_CHANGE_CIPHER: Log ("Change cipher"     ); break;
        case TLS_CONTENT_TYPE_ALERT:         Log ("Alert"             ); break;
        case TLS_CONTENT_TYPE_HANDSHAKE:     Log ("Handshake"         ); break;
        case TLS_CONTENT_TYPE_APPLICATION:   Log ("Application"       ); break;
        case TLS_CONTENT_TYPE_HEARTBEAT:     Log ("Heartbeat"         ); break;
        default:                             LogF("%02hX", contentType); break;
    }
}
void TlsLogHandshakeType(char handshakeType)
{
    switch (handshakeType)
    {
        case TLS_HANDSHAKE_HELLO_REQUEST:        Log ("Hello request"       ); break;
        case TLS_HANDSHAKE_CLIENT_HELLO:         Log ("Client hello"        ); break;
        case TLS_HANDSHAKE_SERVER_HELLO:         Log ("Server hello"        ); break;
        case TLS_HANDSHAKE_NEW_SESSION_TICKET:   Log ("New session ticket"  ); break;
        case TLS_HANDSHAKE_ENCRYPTED_EXTENSIONS: Log ("Encrypted extensions"); break;
        case TLS_HANDSHAKE_CERTIFICATE:          Log ("Certificate"         ); break;
        case TLS_HANDSHAKE_SERVER_KEY_EXCHANGE:  Log ("Server key exchange" ); break;
        case TLS_HANDSHAKE_CERTIFICATE_REQUEST:  Log ("Certificate request" ); break;
        case TLS_HANDSHAKE_SERVER_HELLO_DONE:    Log ("Server hello done"   ); break;
        case TLS_HANDSHAKE_CERTIFICATE_VERIFY:   Log ("Certificate verify"  ); break;
        case TLS_HANDSHAKE_CLIENT_KEY_EXCHANGE:  Log ("Client key exchange" ); break;
        case TLS_HANDSHAKE_FINISHED:             Log ("Finished"            ); break;
        default:                                 LogF("%02hX", handshakeType); break;
    }
}
void TlsLogAlertLevel(char level)
{
    switch (level)
    {
        case  TLS_ALERT_WARNING: Log ("Warning"  ); break;
        case  TLS_ALERT_FATAL:   Log ("Fatal"    ); break;
        default:                 LogF("%d", level); break;
    }
}
void TlsLogAlertDescription(char description)
{
    switch (description)
    {
        case   0: Log("Close notify"                   ); break;  
        case  10: Log("Unexpected message"             ); break;  
        case  20: Log("Bad record MAC"                 ); break;
        case  21: Log("Decryption failed"              ); break;
        case  22: Log("Record overflow"                ); break;
        case  30: Log("Decompression failure"          ); break; 
        case  40: Log("Handshake failure"              ); break;
        case  41: Log("No certificate"                 ); break;
        case  42: Log("Bad certificate"                ); break;
        case  43: Log("Unsupported certificate"        ); break;
        case  44: Log("Certificate revoked"            ); break;
        case  45: Log("Certificate expired"            ); break;
        case  46: Log("Certificate unknown"            ); break;
        case  47: Log("Illegal parameter"              ); break;
        case  48: Log("Unknown CA"                     ); break;
        case  49: Log("Access denied"                  ); break;
        case  50: Log("Decode error"                   ); break;
        case  51: Log("Decrypt error"                  ); break;
        case  60: Log("Export restriction"             ); break;
        case  70: Log("Protocol version"               ); break;
        case  71: Log("Insufficient security"          ); break;
        case  80: Log("Internal error"                 ); break;
        case  86: Log("Inappropriate Fallback"         ); break;
        case  90: Log("User cancelled"                 ); break;
        case 100: Log("No renegotiation"               ); break;
        case 110: Log("Unsupported extension"          ); break;
        case 111: Log("Certificate unobtainable"       ); break;
        case 112: Log("Unrecognized name"              ); break;
        case 113: Log("Bad certificate status response"); break;
        case 114: Log("Bad certificate hash value"     ); break;
        case 115: Log("Unknown PSK identity"           ); break;
        case 120: Log("No Application Protocol"        ); break;
        default:  LogF("%d", description               ); break;
    }
}