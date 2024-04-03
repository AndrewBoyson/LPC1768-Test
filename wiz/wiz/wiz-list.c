#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "web/http/http.h"
#include "net/eth/mac.h"
#include "clock/clk/clk.h"
#include "clock/clk/clktime.h"
#include "lpc1768/mstimer/mstimer.h"
#include "log/log.h"
#include "wiz/wiz/wiz.h"
#include "wiz/wiz/wiz-send.h"
#include "wiz/wiz/wiz-list.h"
#include "settings/settings.h"

#define TIME_TO_WAIT_FOR_SUCCESS_MS 500

#define DEBUG false

#define DO_NOTHING  0
#define DO_TURN_ON  1
#define DO_TURN_OFF 2

static uint32_t msTimer = 0;

struct wiz
{
    char mac[6];                     //Stored in FRAM
    char name[WIZ_LIST_NAME_LENGTH]; //Stored in FRAM
    char room[WIZ_LIST_NAME_LENGTH]; //Stored in FRAM
    clktime nowtai;
    int     signal;
    bool    on;
    int     dimming;
    int     todo;
    clktime requestMadeTai;
};

struct wiz items[WIZ_LIST_MAX_ITEMS];

int WizListGetIndexFromMac(char* mac)
{
    if (!mac) return -1;
    if (MacIsEmpty(mac)) return -1;
    int i = 0;
    while (i < WIZ_LIST_MAX_ITEMS)
    {
        if (MacIsSame(items[i].mac, mac)) return i;
        i++;
    }
    return -1;
}
int getIndexFirstEmpty()
{
    int i = 0;
    while (i < WIZ_LIST_MAX_ITEMS)
    {
        if (MacIsEmpty(items[i].mac)) return i;
        i++;
    }
    return -1;
}
char* WizListNameFromMac(char* mac) //Be careful that this returns NUL if not found and that the string returned may not be terminated if there is no room
{
    if (!mac) return 0;
    if (MacIsEmpty(mac)) return 0;
    int i = 0;
    while (i < WIZ_LIST_MAX_ITEMS)
    {
        if (MacIsSame(mac, items[i].mac)) return items[i].name;
        i++;
    }
    return 0;
}
char* WizListNameToMac(char* name) //be careful that this returns NUL if not found
{
    if (!name) return 0;
    if (name[0] == 0) return 0;
    int i = 0;
    while (i < WIZ_LIST_MAX_ITEMS)
    {
        if (strncmp(name, items[i].name, WIZ_LIST_NAME_LENGTH) == 0) return items[i].mac;
        i++;
    }
    return 0;
}

void WizListSetMac (int i, char* mac ) { MacCopy(items[i].mac, mac);                         SetWizListMac (i, mac ); }
void WizListGetMac (int i, char* mac ) { MacCopy(mac, items[i].mac); }
void WizListSetName(int i, char* name) { strncpy(items[i].name, name, WIZ_LIST_NAME_LENGTH); SetWizListName(i, name); }
void WizListGetName(int i, char* name) { strncpy(name, items[i].name, WIZ_LIST_NAME_LENGTH); }
void WizListSetRoom(int i, char* room) { strncpy(items[i].room, room, WIZ_LIST_NAME_LENGTH); SetWizListRoom(i, room); }
void WizListGetRoom(int i, char* room) { strncpy(room, items[i].room, WIZ_LIST_NAME_LENGTH); }


void WizListStatusAdd(char* mac, char* rssi, char* state, char* sceneId, char* dimming)
{
    int i = WizListGetIndexFromMac(mac);
    if (i < 0)
    {
        i = getIndexFirstEmpty();
        if (i < 0)
        {
            LogTimeF("WIZ - no more spaces in lights list\r\n");
            return;
        }
        WizListSetMac(i, mac);
    }
    items[i].nowtai = ClkNowTai();
    items[i].signal = atoi(rssi);
    items[i].on = state[0] == 't';
    items[i].dimming = atoi(dimming);
}
void WizListMoveUp(int i)
{
    if (i <= 0) return; //Cannot move further up
    struct wiz item;
    MacCopy(item.mac,      items[i].mac);
    strncpy(item.name,     items[i].name, WIZ_LIST_NAME_LENGTH-1);
    strncpy(item.room,     items[i].room, WIZ_LIST_NAME_LENGTH-1);
            item.nowtai  = items[i].nowtai;
            item.signal  = items[i].signal;
            item.on      = items[i].on;
            item.dimming = items[i].dimming;
    
    WizListSetMac (i,  items[i-1].mac );
    WizListSetName(i,  items[i-1].name);
    WizListSetRoom(i,  items[i-1].room);
    items[i].nowtai  = items[i-1].nowtai;
    items[i].signal  = items[i-1].signal;
    items[i].on      = items[i-1].on;
    items[i].dimming = items[i-1].dimming;
            
    WizListSetMac (i-1,  item.mac );
    WizListSetName(i-1,  item.name);
    WizListSetRoom(i-1,  item.room);
    items[i-1].nowtai  = item.nowtai;
    items[i-1].signal  = item.signal;
    items[i-1].on      = item.on;
    items[i-1].dimming = item.dimming;
}
void WizListMoveDown(int i)
{
    if (i >= WIZ_LIST_MAX_ITEMS - 1) return; //Cannot move further down
    struct wiz item;
    MacCopy(item.mac,      items[i].mac);
    strncpy(item.name,     items[i].name, WIZ_LIST_NAME_LENGTH-1);
    strncpy(item.room,     items[i].room, WIZ_LIST_NAME_LENGTH-1);
            item.nowtai  = items[i].nowtai;
            item.signal  = items[i].signal;
            item.on      = items[i].on;
            item.dimming = items[i].dimming;
    
    WizListSetMac (i,  items[i+1].mac);
    WizListSetName(i,  items[i+1].name);
    WizListSetRoom(i,  items[i+1].room);
    items[i].nowtai  = items[i+1].nowtai;
    items[i].signal  = items[i+1].signal;
    items[i].on      = items[i+1].on;
    items[i].dimming = items[i+1].dimming;
            
    WizListSetMac (i+1,  item.mac);
    WizListSetName(i+1,  item.name);
    WizListSetRoom(i+1,  item.room);
    items[i+1].nowtai  = item.nowtai;
    items[i+1].signal  = item.signal;
    items[i+1].on      = item.on;
    items[i+1].dimming = item.dimming;
}
void WizListHttp()
{
    int i = 0;
    while (i < WIZ_LIST_MAX_ITEMS)
    {
        if (!MacIsEmpty(items[i].mac))
        {
            MacHttp    (   items[i].mac                       ); HttpAddChar('\t'); //MAC
            HttpAddTextN(  items[i].name, WIZ_LIST_NAME_LENGTH); HttpAddChar('\t'); //Name
            HttpAddTextN(  items[i].room, WIZ_LIST_NAME_LENGTH); HttpAddChar('\t'); //Room
            
            if (items[i].nowtai)
            {
                clktime taiDiff = ClkNowTai() - items[i].nowtai;
                int seconds = taiDiff >> CLK_TIME_ONE_SECOND_SHIFT;
                HttpAddF("%d", seconds);
            }
            HttpAddChar('\t'); //Seconds since last status received
            
            HttpAddF("%d", items[i].signal                    ); HttpAddChar('\t'); //-127 to 0
            HttpAddF("%d", items[i].on                        ); HttpAddChar('\t'); //1 or 0
            HttpAddF("%d", items[i].dimming                   );                    //0 to 100
        }
        HttpAddChar('\n');
        i++;
    }
}
void WizListInit()
{
    msTimer = MsTimerCount;
    for (int i = 0; i < WIZ_LIST_MAX_ITEMS; i++)
    {
        GetWizListMac (i, items[i].mac );
        GetWizListName(i, items[i].name);
        GetWizListRoom(i, items[i].room);
        items[i].nowtai         = 0;
        items[i].signal         = 0;
        items[i].on             = 0;
        items[i].dimming        = 0;
        items[i].todo           = 0;
        items[i].requestMadeTai = 0;
    }
}
static void logDebug(char* mac, char* text)
{
    if (!DEBUG) return;
    LogTimeF("Wiz");
    if (mac)
    {
        Log(" ");
        MacLog(mac);
    }
    Log(" - ");
    Log(text);
}

void WizListMain()
{
    if (WizSendBusy()) return;
    if (MsTimerRepetitive(&msTimer, 1000))
    {
        static int seconds = 0;
        seconds++;
        if (seconds > 20)
        {
            WizSendRegister(); //This will set WizSendBusy
            seconds = 0;
            return;
        }
    }
    
    static int i = -1;
    i++;
    if (i >= WIZ_LIST_MAX_ITEMS) i = 0;
    
    if (items[i].todo && !MacIsEmpty(items[i].mac))
    {
        clktime taiDiff = ClkNowTai() - items[i].requestMadeTai;
        int ms = taiDiff >> (CLK_TIME_ONE_SECOND_SHIFT - 10); //1 ==> 1 /1024 rather than 1/1000 seconds but its good enough
        if (!items[i].requestMadeTai || ms > TIME_TO_WAIT_FOR_SUCCESS_MS)
        {
            switch(items[i].todo)
            {
                case DO_TURN_ON:
                    if (!items[i].requestMadeTai) logDebug(items[i].mac, "Turn on\r\n");
                    else                          logDebug(items[i].mac, "Turn on repeat\r\n");
                    WizSendTurnOneOn(items[i].mac);
                    items[i].requestMadeTai = ClkNowTai();
                    break;
                case DO_TURN_OFF:
                    if (!items[i].requestMadeTai) logDebug(items[i].mac, "Turn off\r\n");
                    else                          logDebug(items[i].mac, "Turn off repeat\r\n");
                    WizSendTurnOneOff(items[i].mac);
                    items[i].requestMadeTai = ClkNowTai();
                    break;
            }
        }
    }
}
void WizListTurnIndexOn(int i)
{
    if (MacIsEmpty(items[i].mac)) return;
    items[i].todo = DO_TURN_ON;
    items[i].requestMadeTai = 0;
}
void WizListTurnIndexOff(int i)
{
    if (MacIsEmpty(items[i].mac)) return;
    items[i].todo = DO_TURN_OFF;
    items[i].requestMadeTai = 0;
}
void WizListTurnMacOn (char* mac)
{
    if (!mac) return;
    if (MacIsEmpty(mac)) return;
    int i = WizListGetIndexFromMac(mac);
    if (i < 0) return;
    items[i].todo = DO_TURN_ON;
    items[i].requestMadeTai = 0;
}
void WizListTurnMacOff(char* mac)
{
    if (!mac) return;
    if (MacIsEmpty(mac)) return;
    int i = WizListGetIndexFromMac(mac);
    if (i < 0) return;
    items[i].todo = DO_TURN_OFF;
    items[i].requestMadeTai = 0;
}
void WizListTurnAllOn()
{
    for (int i = 0; i < WIZ_LIST_MAX_ITEMS; i++)
    {
        if (!MacIsEmpty(items[i].mac))
        {
            items[i].todo = DO_TURN_ON;
            items[i].requestMadeTai = ClkNowTai();
        }
    }
    WizSendTurnAllOn();
    logDebug(0, "Turn all on\r\n");
}
void WizListTurnAllOff(         )
{
    for (int i = 0; i < WIZ_LIST_MAX_ITEMS; i++)
    {
        if (!MacIsEmpty(items[i].mac))
        {
            items[i].todo = DO_TURN_OFF;
            items[i].requestMadeTai = ClkNowTai();
        }
    }
    WizSendTurnAllOff();
    logDebug(0, "Turn all off\r\n");
}
void WizListTurnRoomOn(char* room)
{
    for (int i = 0; i < WIZ_LIST_MAX_ITEMS; i++)
    {
        if (!MacIsEmpty(items[i].mac) && strncmp(items[i].room, room, WIZ_LIST_NAME_LENGTH) == 0)
        {
            WizListTurnMacOn(items[i].mac);
        }
    }
}
void WizListTurnRoomOff(char* room)
{
    for (int i = 0; i < WIZ_LIST_MAX_ITEMS; i++)
    {
        if (!MacIsEmpty(items[i].mac) && strncmp(items[i].room, room, WIZ_LIST_NAME_LENGTH) == 0)
        {
            WizListTurnMacOff(items[i].mac);
        }
    }
}
void WizListReceivedSuccess(char* mac)
{
    if (!mac) return;
    int i = WizListGetIndexFromMac(mac);
    if (i < 0) return;
    switch (items[i].todo)
    {
        case DO_TURN_ON:
            items[i].on = true;
            logDebug(items[i].mac, "Turn on success\r\n");
            break;
        case DO_TURN_OFF:
            items[i].on = false;
            logDebug(items[i].mac, "Turn off success\r\n");
            break;
    }
    items[i].todo = DO_NOTHING;
    items[i].requestMadeTai = 0;
}