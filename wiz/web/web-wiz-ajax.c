#include  <stdint.h>
#include   <stdio.h>
#include <time.h>

#include "web/http/http.h"
#include "wiz/wiz/wiz.h"
#include "wiz/wiz/wiz-sun.h"
#include "wiz/wiz/wiz-sched.h"
#include "wiz/wiz/wiz-list.h"
#include "clk/clk.h"

void WebWizAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    HttpAddNibbleAsHex(WizTrace);
    HttpAddChar('\n');
    HttpAddChar('\f');
    WizListHttp();
    HttpAddChar('\f');
    
    struct tm tmUtc;
    ClkNowTmUtc  (&tmUtc);
    
    int minutesTodayUtc   = tmUtc.tm_hour   * 60 + tmUtc.tm_min;
    
    HttpAddInt32AsHex(WizSchedMinutesUtcToLocal(minutesTodayUtc));
    HttpAddChar('\n');
    HttpAddInt32AsHex(minutesTodayUtc);
    HttpAddChar('\n');
    HttpAddInt32AsHex(WizSchedMinutesUtcToLocal(WizSunRiseMinutes()));
    HttpAddChar('\n');
    HttpAddInt32AsHex(WizSchedMinutesUtcToLocal(WizSunSetMinutes()));
    HttpAddChar('\n');
    HttpAddChar('\f');
    
    WizSchedHttp();
}

