#include <stdint.h>
#include <time.h>
#include <stdbool.h>

extern bool    ValuesTrace;

extern char*   ValuesGetServerName   (void);
extern char*   ValuesGetFileName     (void);
extern int     ValuesGetWriteSize    (void);
extern int     ValuesGetReadInterval (void);
extern void    ValuesGetStartTm      (struct tm* ptm);
extern int64_t ValuesGetStartTime    (void);
extern int     ValuesGetCount        (void);
extern void    ValuesSetServerName   (char* value);
extern void    ValuesSetFileName     (char* value);
extern void    ValuesSetWriteSize    (int   value);
extern void    ValuesSetReadInterval (int   value);

extern void    ValuesMain(void);
extern void    ValuesInit(void);