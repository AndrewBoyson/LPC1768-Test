#include <stdint.h>
#include <stdbool.h>

extern bool TlsTrace;

extern void TlsRequestMain(void);
extern void TlsRequest    (int connectionId,                      int  windowSize, uint8_t* pWindow, uint32_t windowPositionInStream);
extern bool TlsResponse   (int connectionId, bool clientFinished, int* windowSize, uint8_t* pWindow, uint32_t windowPositionInStream);

extern void TlsInit       (void);
extern void TlsMain       (void);
extern void TlsReset      (int connectionId);