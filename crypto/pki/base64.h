#include <stdio.h>

extern char (*Base64ReadNextCharFunctionPointer)(void);
extern int    Base64ReadByte(void);
extern int    Base64SkipLine(void);