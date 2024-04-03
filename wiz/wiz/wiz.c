#include <time.h>

#include "net/user.h"
#include "wiz-sched.h"
#include "wiz-list.h"
#include "wiz-sun.h"
#include "wiz-send.h"
#include "wiz-recv.h"
#include "lpc1768/led.h"


bool WizTrace = true;

void WizMain()
{
    WizListMain();
    WizSchedMain();
}

void WizInit()
{
    WizRecvInit();
    WizSendInit();
    WizListInit();
    WizSchedInit();
}