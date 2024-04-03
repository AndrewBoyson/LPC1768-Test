#include <string.h>
#include <stdlib.h>
#include "web/http/http.h"
#include "net/eth/mac.h"
#include "wiz/wiz/wiz.h"
#include "wiz/wiz/wiz-list.h"
#include "wiz/wiz/wiz-sched.h"
#include "settings/settings.h"
#include "log/log.h"

void WebWizQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        HttpQueryUnencode(pValue);

        int value = HttpQueryValueAsInt(pValue);
        
        if (HttpSameStr(pName, "wiztrace"    )) ChgTraceWiz();
        
        if (HttpSameStr(pName, "turn-all-on" )) WizListTurnAllOn ();
        if (HttpSameStr(pName, "turn-all-off")) WizListTurnAllOff();
        
        //Lights
        if (strncmp(pName, "lnam", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            WizListSetName(i, pValue ? pValue : "");
        }
        if (strncmp(pName, "lroo", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            WizListSetRoom(i, pValue ? pValue : "");
        }
        if (strncmp(pName, "ldel", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            char mac[6];
            MacClear(mac);
            WizListSetMac (i, mac);
            WizListSetName(i, "");
        }
        if (strncmp(pName, "lmov", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            if (pValue)
            {
                if (pValue[0] == 'u') WizListMoveUp  (i);
                if (pValue[0] == 'd') WizListMoveDown(i);
            }
        }
        if (strncmp(pName, "lswi", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            if (pValue)
            {
                if (pValue[0] == '1') WizListTurnIndexOn (i);
                if (pValue[0] == '0') WizListTurnIndexOff(i);
            }
        }
        
        //Schedules 'snam', 'soff', 's-on', 'sdel' 'smov', 'sswi'
        
        if (strncmp(pName, "snam", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            WizSchedSetName(i, pValue ? pValue : "");
        }
        if (strncmp(pName, "s-on", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            char datum = 0;
            int16_t minutes = 0;
            WizSchedParseEvent(pValue, &datum, &minutes);
            WizSchedSetOnEvent(i, datum, minutes);
        }
        if (strncmp(pName, "soff", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            char datum = 0;
            int16_t minutes = 0;
            WizSchedParseEvent(pValue, &datum, &minutes);
            WizSchedSetOffEvent(i, datum, minutes);
        }
        if (strncmp(pName, "sdel", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            WizSchedSetName(i, "");
            WizSchedSetOnEvent (i, 0, 0);
            WizSchedSetOffEvent(i, 0, 0);
        }
        if (strncmp(pName, "smov", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            if (pValue)
            {
                if (pValue[0] == 'u') WizSchedMoveUp  (i);
                if (pValue[0] == 'd') WizSchedMoveDown(i);
            }
        }
        if (strncmp(pName, "sswi", 4) == 0)
        {
            int posn = 4;
            int i = pName[posn] - '0';
            if (pName[posn+1])
            {
                i *= 10;
                i += pName[posn+1] - '0';
            }
            if (i < 0) i = 0;
            if (i > WIZ_LIST_MAX_ITEMS - 1) i = WIZ_LIST_MAX_ITEMS - 1;
            if (pValue)
            {
                if (pValue[0] == '1')
                {
                     char room[WIZ_LIST_NAME_LENGTH];
                     WizListGetRoom(i, room);
                     WizListTurnRoomOn(room);
                }
                if (pValue[0] == '0')
                {
                     char room[WIZ_LIST_NAME_LENGTH];
                     WizListGetRoom(i, room);
                     WizListTurnRoomOff(room);
                }
            }
        }
        
    }
}
