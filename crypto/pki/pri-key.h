#include <stdbool.h>

extern const char* PriKeySourcePtr;

extern int         PriKeyDecryptStart             (uint8_t* message); //Returns the slot
extern bool        PriKeyDecryptFinished          (int slot);
extern uint8_t*    PriKeyDecryptResultLittleEndian(int slot);
extern void        PriKeyDecryptResultTail        (int slot, int length, uint8_t* pBuffer);
extern void        PriKeyDecryptClear             (int slot);

extern void        PriKeyInit(void);