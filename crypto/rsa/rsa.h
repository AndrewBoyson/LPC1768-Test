#include <stdint.h>
#include <stdbool.h>

extern void      RsaMain(void);
extern void      RsaInit(void);

extern int       RsaStart(uint32_t* message, uint32_t* p, uint32_t* q, uint32_t* dp, uint32_t* dq, uint32_t* qi);

extern bool      RsaFinished(int slot);
extern uint32_t* RsaResult  (int slot);
extern void      RsaClear   (int slot);
