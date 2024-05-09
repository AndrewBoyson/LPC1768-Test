#include <stdint.h>

void TlsMacSha1(int      macKeyLength,
                uint8_t* macKey,
                uint64_t sequence,
                uint8_t  contentType,
                uint8_t  versionH,
                uint8_t  versionL,
                int      payloadLength,
                uint8_t* payload,
                uint8_t* mac);