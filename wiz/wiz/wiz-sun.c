#include <time.h>

#include "lpc1768/tm/tm.h"
#include "clock/clk/clk.h"
#include "log/log.h"

//                                J          F        M       A+1       M+1       J+1       J+1       A+1       S+1       O+1       N         D
static uint16_t sunrises[] = {  8*60+45,  8*60+ 6,  7*60+ 1,  5*60+38,  4*60+23,  3*60+28,  3*60+26,  4*60+14,  5*60+16,  6*60+16,  7*60+21,  8*60+22 };
static uint16_t sunsets [] = { 15*60+45, 16*60+43, 17*60+45, 18*60+51, 19*60+53, 20*60+49, 21*60+ 3, 20*60+20, 19*60+ 5, 17*60+45, 16*60+28, 15*60+38 };

int WizSunSetMinutes ()
{
    struct tm tmUtc;
    ClkNowTmUtc  (&tmUtc);
    
    int start  = sunsets[tmUtc.tm_mon];
    int finish = tmUtc.tm_mon == 11 ? sunsets[0] : sunsets[tmUtc.tm_mon+1];
    int extra = (finish - start) * (tmUtc.tm_mday-1) / TmMonthLength(tmUtc.tm_year, tmUtc.tm_mon);
    
    return start + extra;
}
int WizSunRiseMinutes()
{
    struct tm tmUtc;
    ClkNowTmUtc  (&tmUtc);
    
    int start  = sunrises[tmUtc.tm_mon];
    int finish = tmUtc.tm_mon == 11 ? sunrises[0] : sunrises[tmUtc.tm_mon+1];
    int extra = (finish - start) * (tmUtc.tm_mday-1) / TmMonthLength(tmUtc.tm_year, tmUtc.tm_mon);
    
    return start + extra;
}