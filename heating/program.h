#include <stdbool.h>

extern int  ProgramGetDay       (int i); extern void ProgramSetDay       (int i, int value);
extern int  ProgramGetNewDayHour(void ); extern void ProgramSetNewDayHour(       int value);

extern void ProgramToString(int i, int buflen, char* buffer);
extern void ProgramSendAjax(void);
extern void ProgramParse   (int i, char* text);

extern bool ProgramTimerOutput;

extern  int ProgramMain(void);
extern  int ProgramInit(void);

#define PROGRAM_COUNT             3
#define PROGRAM_TRANSITIONS_COUNT 4