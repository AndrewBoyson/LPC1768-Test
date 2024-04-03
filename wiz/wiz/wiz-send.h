#include <stdbool.h>

extern void WizSendInit(void);

extern void WizSendRegister  (void);
extern void WizSendTurnAllOn (void);
extern void WizSendTurnAllOff(void);
extern void WizSendTurnOneOn (char* mac);
extern void WizSendTurnOneOff(char* mac);
extern bool WizSendBusy      (void);