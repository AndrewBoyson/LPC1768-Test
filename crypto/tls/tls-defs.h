
#define TLS_CONTENT_TYPE_CHANGE_CIPHER     20
#define TLS_CONTENT_TYPE_ALERT             21
#define TLS_CONTENT_TYPE_HANDSHAKE         22
#define TLS_CONTENT_TYPE_APPLICATION       23
#define TLS_CONTENT_TYPE_HEARTBEAT         24

#define TLS_HANDSHAKE_HELLO_REQUEST         0
#define TLS_HANDSHAKE_CLIENT_HELLO          1
#define TLS_HANDSHAKE_SERVER_HELLO          2
#define TLS_HANDSHAKE_NEW_SESSION_TICKET    4
#define TLS_HANDSHAKE_ENCRYPTED_EXTENSIONS  8
#define TLS_HANDSHAKE_CERTIFICATE          11
#define TLS_HANDSHAKE_SERVER_KEY_EXCHANGE  12
#define TLS_HANDSHAKE_CERTIFICATE_REQUEST  13
#define TLS_HANDSHAKE_SERVER_HELLO_DONE    14
#define TLS_HANDSHAKE_CERTIFICATE_VERIFY   15
#define TLS_HANDSHAKE_CLIENT_KEY_EXCHANGE  16
#define TLS_HANDSHAKE_FINISHED             20

#define TLS_ALERT_WARNING                   1
#define TLS_ALERT_FATAL                     2
#define TLS_ALERT_ILLEGAL_PARAMETER        47
#define TLS_ALERT_INTERNAL_ERROR           80

#define TLS_LENGTH_PRE_MASTER_SECRET 48
#define TLS_LENGTH_MASTER_SECRET     48
#define TLS_LENGTH_RANDOM            32
#define TLS_LENGTH_VERIFY            12
