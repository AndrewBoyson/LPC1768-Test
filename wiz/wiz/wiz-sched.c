#include <stdint.h>
#include <string.h>

#include "wiz/wiz/wiz-list.h"
#include "wiz/wiz/wiz-sun.h"
#include "wiz/wiz/wiz-sched.h"
#include "settings/settings.h"
#include "web/http/http.h"
#include "clk/clk.h"
#include "log/log.h"
#include "net/eth/mac.h"
#include "lpc1768/tm/tm.h"

#define MINIMUM_DURATION_MINUTES 1

#define DEBUG false

struct item
{
    char    name[WIZ_LIST_NAME_LENGTH]; //Name or '*' for broadcast
    char    ondatum;
    int16_t onminutes;
    char    offdatum;
    int16_t offminutes;
};
static struct item items[WIZ_SCHED_MAX_ITEMS];

void WizSchedSetName    (int i, char*             name) { strncpy  ( items[i].name, name, WIZ_LIST_NAME_LENGTH); SetWizSchedName    (i, name ); }
void WizSchedSetOnEvent (int i, char datum, int16_t minutes)
{
    items[i].ondatum   = datum  ;
    items[i].onminutes = minutes;
    SetWizSchedOnDatum  (i, &datum  );
    SetWizSchedOnMinutes(i, &minutes);
}
void WizSchedSetOffEvent(int i, char datum, int16_t minutes)
{
    items[i].offdatum   = datum;
    items[i].offminutes = minutes;
    SetWizSchedOffDatum  (i, &datum  );
    SetWizSchedOffMinutes(i, &minutes);
}

static int minutesTodayUtc;
static int minutesSunriseUtc;
static int minutesSunsetUtc;
static bool isDst;

int WizSchedMinutesUtcToLocal(int minutes)
{
    if (isDst) minutes += TM_DST_OFFSET * 60;
    else       minutes += TM_STD_OFFSET * 60;
    if (minutes >= 1440) minutes -= 1440;
    return minutes;
}
int WizSchedMinutesLocalToUtc(int minutes)
{
    if (isDst) minutes -= TM_DST_OFFSET * 60;
    else       minutes -= TM_STD_OFFSET * 60;
    if (minutes < 0) minutes += 1440;
    return minutes;
}

static int getOnMinuteUtc(int i)
{
    switch (items[i].ondatum)
    {
        case 'U': return items[i].onminutes;
        case 'L': return WizSchedMinutesLocalToUtc(items[i].onminutes);
        case 'R': return items[i].onminutes + minutesSunriseUtc;
        case 'S': return items[i].onminutes + minutesSunsetUtc;
    }
    return -1;
}
static int getOffMinuteUtc(int i)
{
    switch (items[i].offdatum)
    {
        case 'U': return items[i].offminutes;
        case 'L': return WizSchedMinutesLocalToUtc(items[i].offminutes);
        case 'R': return items[i].offminutes + minutesSunriseUtc;
        case 'S': return items[i].offminutes + minutesSunsetUtc;
    }
    return -1;
}
static int subtractMinutes(int a, int b)
{
    int d = a - b;
    if (d >= 1080) d -= 1440; //Positive durations upto and including 18 hours are ok
    if (d <  -360) d += 1440; //Negative durations over 6 hours are assumed to be next day
    return d;
}
static void onOffHttp(char datum, int minutes)
{
    bool isNegative = minutes < 0;
    if (isNegative) minutes = -minutes;
    switch (datum)
    {
        case 'L':
            if (isNegative) HttpAddChar('-') ;
            HttpAddF("%02d", minutes / 60);
            HttpAddChar('h');
            HttpAddF("%02d", minutes % 60);
            break;
        case 'S':
        case 'R':
            HttpAddChar(datum);
            if (minutes)
            {
                HttpAddChar(isNegative ? '-' : '+') ;
                if (minutes < 60)
                {
                    HttpAddF("%d", minutes);
                }
                else
                {
                    HttpAddF("%02d", minutes / 60);
                    HttpAddChar('h');
                    HttpAddF("%02d", minutes % 60);
                }
            }
            break;
        case 'D':
            HttpAddChar(isNegative ? '-' : '+') ;
            HttpAddF("%02d", minutes / 60);
            HttpAddChar('h');
            HttpAddF("%02d", minutes % 60);
            break;
    }
}
void WizSchedParseEvent(char* text, char* pDatum, int16_t* pMinutes)
{
    if (!text || !pDatum || !pMinutes) return;
    
    *pDatum = 'L'; //Default to local
    bool isNegative = false;
    int minutes = 0;
    int hours = 0;
    for (int i = 0; i < 10 && text[i]; i++)
    {
        switch (text[i])
        {
            case 'H':
            case 'h':
                hours = minutes;
                minutes = 0;
                break;
            case 'L':
            case 'l':
                *pDatum = 'L';
                break;
            case 'R':
            case 'r':
                *pDatum = 'R';
                break;
            case 'S':
            case 's':
                *pDatum = 'S';
                break;
            case '0': minutes = minutes * 10 + 0; break;
            case '1': minutes = minutes * 10 + 1; break;
            case '2': minutes = minutes * 10 + 2; break;
            case '3': minutes = minutes * 10 + 3; break;
            case '4': minutes = minutes * 10 + 4; break;
            case '5': minutes = minutes * 10 + 5; break;
            case '6': minutes = minutes * 10 + 6; break;
            case '7': minutes = minutes * 10 + 7; break;
            case '8': minutes = minutes * 10 + 8; break;
            case '9': minutes = minutes * 10 + 9; break;
            case '+': isNegative = false; break;
            case '-': isNegative = true;  break;
        }
    }
    *pMinutes = hours * 60 + minutes;
    if (isNegative) *pMinutes = -*pMinutes;
}

void WizSchedMoveUp(int i)
{
    if (i <= 0) return; //Cannot move further up
    struct item item;
    strncpy  ( item.name, items[i].name, WIZ_LIST_NAME_LENGTH-1);
    item.ondatum = items[i].ondatum;
    item.onminutes = items[i].onminutes;
    item.offdatum = items[i].offdatum;
    item.offminutes = items[i].offminutes;
    
    WizSchedSetName    (i,   items[i-1].name);
    WizSchedSetOnEvent (i,   items[i-1].ondatum, items[i-1].onminutes);
    WizSchedSetOffEvent(i,   items[i-1].offdatum, items[i-1].offminutes);

    WizSchedSetName    (i-1, item.name      );
    WizSchedSetOnEvent (i-1, item.ondatum,  item.onminutes);
    WizSchedSetOffEvent(i-1, item.offdatum, item.offminutes);
}
void WizSchedMoveDown(int i)
{
    if (i >= WIZ_LIST_MAX_ITEMS - 1) return; //Cannot move further down
    struct item item;
    strncpy  ( item.name, items[i].name, WIZ_LIST_NAME_LENGTH-1);
    item.ondatum = items[i].ondatum;
    item.onminutes = items[i].onminutes;
    item.offdatum = items[i].offdatum;
    item.offminutes = items[i].offminutes;
    
    WizSchedSetName    (i,   items[i+1].name);
    WizSchedSetOnEvent (i,   items[i+1].ondatum, items[i+1].onminutes);
    WizSchedSetOffEvent(i,   items[i+1].offdatum, items[i+1].offminutes);

    WizSchedSetName    (i+1, item.name      );
    WizSchedSetOnEvent (i+1, item.ondatum, item.onminutes);
    WizSchedSetOffEvent(i+1, item.offdatum, item.offminutes);
}
void WizSchedHttp()
{
    int i = 0;
    while (i < WIZ_SCHED_MAX_ITEMS)
    {
        if (items[i].name[0])
        {
            HttpAddTextN(items[i].name, WIZ_LIST_NAME_LENGTH);
            HttpAddChar('\t');
            
            onOffHttp(items[i].ondatum, items[i].onminutes );
            HttpAddChar('\t');
            
            onOffHttp(items[i].offdatum, items[i].offminutes );
            HttpAddChar('\t');
            
            int  onMinuteUtc =  getOnMinuteUtc(i);
            int offMinuteUtc = getOffMinuteUtc(i);
            int duration = subtractMinutes(offMinuteUtc, onMinuteUtc);
            
            
            onOffHttp('L',  WizSchedMinutesUtcToLocal(onMinuteUtc));
            HttpAddChar('\t');
            
            onOffHttp('L', WizSchedMinutesUtcToLocal(offMinuteUtc));
            HttpAddChar('\t');
            
            onOffHttp('L', duration); 
        }
        HttpAddChar('\n');
        i++;
    }
}
static void doSchedule(int i)
{   
    if (items[i].name[0] == 0) return;
    
    int onMinuteUtc  = getOnMinuteUtc(i);
    int offMinuteUtc = getOffMinuteUtc(i);
    
    //Don't switch on for short or negative durations durations
    int duration = subtractMinutes(offMinuteUtc, onMinuteUtc);
    if (duration < MINIMUM_DURATION_MINUTES) return;
    
    if (onMinuteUtc == minutesTodayUtc)
    {
        if (items[i].name[0] == '*')
        {
            if (DEBUG) LogTimeF("Wiz schedule - turn all on\r\n");
            WizListTurnAllOn();
        }
        else
        {
            if (DEBUG) LogTimeF("Wiz schedule - turn %s on\r\n", items[i].name);
            WizListTurnRoomOn(items[i].name);
        }
    }
    if (offMinuteUtc == minutesTodayUtc)
    {
        if (items[i].name[0] == '*')
        {
            if (DEBUG) LogTimeF("Wiz schedule - turn all off\r\n");
            WizListTurnAllOff();
        }
        else
        {
            if (DEBUG) LogTimeF("Wiz schedule - turn %s off\r\n", items[i].name);
            WizListTurnRoomOff(items[i].name);
        }
    }
}

void WizSchedMain()
{   
    if (!ClkTimeIsSet()) return;
    
    struct tm tmUtc;
    ClkNowTmUtc  (&tmUtc);
    
    minutesTodayUtc   = tmUtc.tm_hour   * 60 + tmUtc.tm_min;
    isDst = tmUtc.tm_isdst;
    
    minutesSunsetUtc  = WizSunSetMinutes();
    minutesSunriseUtc = WizSunRiseMinutes();
    
    static int minutesLastScan = -1;
    if (minutesLastScan == minutesTodayUtc) return; //Start of new minute one-shot
    minutesLastScan = minutesTodayUtc;
    
    for (int i = 0; i < WIZ_SCHED_MAX_ITEMS; i++) doSchedule(i);
}
void WizSchedInit()
{
    for (int i = 0; i < WIZ_SCHED_MAX_ITEMS; i++)
    {
        GetWizSchedName      (i,  items[i].name      );
        GetWizSchedOnDatum   (i, &items[i].ondatum   );
        GetWizSchedOnMinutes (i, &items[i].onminutes );
        GetWizSchedOffDatum  (i, &items[i].offdatum  );
        GetWizSchedOffMinutes(i, &items[i].offminutes);
    }
}