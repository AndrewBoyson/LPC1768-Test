#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lpc1768/rtc/rtc.h"
#include "clock/clk/clktime.h"
#include "clock/clk/clk.h"
#include "log/log.h"
#include "settings/settings.h"
#include "program.h"
#include "web/http/http.h"

static char    programDay[7];
static int16_t programs[PROGRAM_COUNT][PROGRAM_TRANSITIONS_COUNT];
static char    programNewDayHour;

void ProgramSetDay       (int i, int  value) { programDay[i] = (char)value; SetProgramDay(i, programDay + i); }
int  ProgramGetDay       (int i)             { return (int) programDay[i];    }

void ProgramSetNewDayHour(       int  value) { programNewDayHour    = (char)value; SetProgramNewDayHour(&programNewDayHour); }
int  ProgramGetNewDayHour()                  { return (int) programNewDayHour;}

int ProgramInit()
{
    for (int i = 0; i <             7; i++) GetProgramDay(i, programDay + i);
    for (int i = 0; i < PROGRAM_COUNT; i++) GetProgram   (i, programs[i]);
    GetProgramNewDayHour(&programNewDayHour);
    return 0;
}

/*
There are three programs available [0|1|2]; any of which can be allocated to a given day [0-6].
Each program contains four transitions with an index [0|1|2|3].
A transition is defined to be a short (16 bit) and consists of:
+---------+--------+--------+---------+
| 15 - 13 |   12   |   11   | 10 - 00 |
+---------+--------+--------+---------+
|         | in use | switch | minute  |
|         | yes/no | on/off | in day  |
|         |   1/0  |  1/0   | 0-1439  |
+---------+--------+--------+---------+
*/
static int16_t encodeTransition(bool inuse, bool onoff, int minutes)
{
    int16_t    transition  = minutes;
               transition &= 0x07FF;
    if (onoff) transition |= 0x0800;
    if (inuse) transition |= 0x1000;
    return transition;
}
static void decodeTransition(int16_t transition, bool* pinuse, bool* ponoff, int* pminutes)
{
    *pinuse   = transition & 0x1000;
    *ponoff   = transition & 0x0800;
    *pminutes = transition & 0x07FF;
}

static int compareTransition (const void * a, const void * b) //-ve a goes before b; 0 same; +ve a goes after b
{
    bool  inUseA,   inUseB;
    bool     onA,      onB;
    int minutesA, minutesB;
    decodeTransition(*(int16_t*)a, &inUseA, &onA, &minutesA);
    decodeTransition(*(int16_t*)b, &inUseB, &onB, &minutesB);
    
    if (!inUseA && !inUseB) return  0;
    if (!inUseA)            return +1;
    if (!inUseB)            return -1;
    
    if (minutesA < programNewDayHour * 60) minutesA += 1440;
    if (minutesB < programNewDayHour * 60) minutesB += 1440;
    
    if (minutesA < minutesB) return -1;
    if (minutesA > minutesB) return +1;
    return 0;
}
static void sort(int16_t* pProgram)
{
    qsort (pProgram, 4, sizeof(int16_t), compareTransition);
}

//[+|-][00-23][00-59];
void ProgramToString(int program, int buflen, char* buffer)
{
    if (buflen < 25) return;
    char* p = buffer;
    for (int i = 0; i < PROGRAM_TRANSITIONS_COUNT; i++)
    {
        int16_t transition = programs[program][i];
        bool  inuse;
        bool  on;
        int   minuteUnits;
        decodeTransition(transition, &inuse, &on, &minuteUnits);
        if (!inuse) continue;
        
        int minuteTens  = minuteUnits / 10; minuteUnits %= 10;
        int   hourUnits = minuteTens  /  6; minuteTens  %=  6;
        int   hourTens  =   hourUnits / 10;   hourUnits %= 10;
        
        if (p > buffer) *p++ = ' ';
        *p++ = on ? '+' : '-';
        *p++ = hourTens    + '0';
        *p++ = hourUnits   + '0';
        *p++ = minuteTens  + '0';
        *p++ = minuteUnits + '0';
    }
    *p = 0;
}
void ProgramSendAjax()
{
    for (int program = 0; program < PROGRAM_COUNT; program++)
    {
        for (int transition = 0; transition < PROGRAM_TRANSITIONS_COUNT; transition++)
        {
            bool  inuse;
            bool  on;
            int   minuteUnits;
            decodeTransition(programs[program][transition], &inuse, &on, &minuteUnits);
            if (!inuse) continue;
            
            int minuteTens  = minuteUnits / 10; minuteUnits %= 10;
            int   hourUnits = minuteTens  /  6; minuteTens  %=  6;
            int   hourTens  =   hourUnits / 10;   hourUnits %= 10;
            
            if (transition) HttpAddChar(' ');
            HttpAddChar(on ? '+' : '-');
            HttpAddChar(hourTens    + '0');
            HttpAddChar(hourUnits   + '0');
            HttpAddChar(minuteTens  + '0');
            HttpAddChar(minuteUnits + '0');
        }
        HttpAddChar('\n');
    }
}
static void handleParseDelim(int program, int* pIndex, bool* pInUse, bool* pOn, int* pHourTens,  int* pHourUnits, int* pMinuteTens, int* pMinuteUnits)
{
    int hour = *pHourTens * 10 + *pHourUnits;
    if (hour   <  0) *pInUse = false;
    if (hour   > 23) *pInUse = false;
    
    int minute = *pMinuteTens * 10 + *pMinuteUnits;
    if (minute <  0) *pInUse = false;
    if (minute > 59) *pInUse = false;
    
    int minutes = hour * 60 + minute;
    
    int16_t transition = encodeTransition(*pInUse, *pOn, minutes);
    programs[program][*pIndex] = transition;
    
    *pIndex      += 1;
    *pInUse       = 0;
    *pOn          = 0;
    *pHourTens    = 0;
    *pHourUnits   = 0;
    *pMinuteTens  = 0;
    *pMinuteUnits = 0;

}
void ProgramParse(int program, char* p)
{
    int            i = 0;
    bool       inUse = 0;  bool         on = 0;
    int    hourUnits = 0;  int    hourTens = 0;
    int  minuteUnits = 0;  int  minuteTens = 0;
    while (*p && i < PROGRAM_TRANSITIONS_COUNT) 
    {
        if      (*p == '+')              { on = true ; }
        else if (*p == '-')              { on = false; }
        else if (*p >= '0' && *p <= '9') { inUse = true; hourTens = hourUnits; hourUnits = minuteTens; minuteTens = minuteUnits; minuteUnits = *p - '0'; }
        else if (*p == ' ')              { handleParseDelim(program, &i, &inUse, &on, &hourTens, &hourUnits, &minuteTens, &minuteUnits); }
        p++;
    }
    while (i < PROGRAM_TRANSITIONS_COUNT) handleParseDelim(program, &i, &inUse, &on, &hourTens, &hourUnits, &minuteTens, &minuteUnits);
    sort(programs[program]);
    SetProgram(program, programs[program]);
}

static bool readProgramTimerOutput()
{   

    if (!ClkTimeIsSet()) return 0;

    struct tm tm;
    ClkNowTmLocal(&tm);
    
    int dayOfWeek = tm.tm_wday;
    int minutesNow = tm.tm_hour * 60 + tm.tm_min;
    if (tm.tm_hour < programNewDayHour) //Before 2am should be matched against yesterday's program.
    {
        dayOfWeek--;
        if (dayOfWeek < 0) dayOfWeek = 6;
    }
    
    int program = programDay[dayOfWeek];
    
    bool calling = 0;
    for (int i = 0; i < PROGRAM_TRANSITIONS_COUNT; i++)
    {
        int16_t transition = programs[program][i];
        bool inuse;
        bool on;
        int  minutes;
        decodeTransition(transition, &inuse, &on, &minutes);
        if (!inuse) continue;
        if (minutes <= minutesNow) calling = on;
    }
    
    return calling;
}

bool ProgramTimerOutput;

int ProgramMain()
{    
    ProgramTimerOutput = readProgramTimerOutput();
    return 0;
}
