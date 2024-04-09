#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "net/user.h"
#include "log/log.h"
#include "net/eth/eth.h"
#include "net/eth/mac.h"
#include "net/ip4/ip4.h"
#include "net/ip4/ip4addr.h"
#include "wiz/wiz/wiz.h"
#include "wiz/wiz/wiz-list.h"

static char method [20];
static char env    [20];
static char mac    [20];
static char rssi   [20];
static char src    [20];
static char mqttCd [20];
static char ts     [20];
static char state  [20];
static char sceneId[20];
static char temp   [20];
static char dimming[20];
static char id     [20];
static char result [20];
static char success[20];

static void showTrace()
{
    MacLog(EthMacRemote);
    Log(" ");
    Ip4AddrLog(Ip4Remote);
    if (*method ) { Log(" method:" ); Log(method ); }
    if (*env    ) { Log(" env:"    ); Log(env    ); }
    if (*mac    ) { Log(" mac:"    ); Log(mac    ); }
    if (*rssi   ) { Log(" rssi:"   ); Log(rssi   ); }
    if (*src    ) { Log(" src:"    ); Log(src    ); }
    if (*mqttCd ) { Log(" mqttCd:" ); Log(mqttCd ); }
    if (*ts     ) { Log(" ts:"     ); Log(ts     ); }
    if (*state  ) { Log(" state:"  ); Log(state  ); }
    if (*sceneId) { Log(" sceneId:"); Log(sceneId); }
    if (*temp   ) { Log(" temp:"   ); Log(temp   ); }
    if (*dimming) { Log(" dimming:"); Log(dimming); }
    if (*id     ) { Log(" id:"     ); Log(id     ); }
    if (*result ) { Log(" result:" ); Log(result ); }
    if (*success) { Log(" success:"); Log(success); }
    Log("\r\n");
}

static void handleNameValue(char* pName, char* pValue)
{
    if (*pName  ==   0 ) return; //Ignore an empty name
    if (*pName  == '\n') return; //Ignore an empty name
    if (*pName  == '\r') return; //Ignore an empty name
         if (strcmp(pName, "params"      ) == 0) return; //Ignore the params name
    else if (strcmp(pName, "method"      ) == 0) strncpy(method , pValue, sizeof(method )-1);
    else if (strcmp(pName, "env"         ) == 0) strncpy(env    , pValue, sizeof(env    )-1);
    else if (strcmp(pName, "mac"         ) == 0) strncpy(mac    , pValue, sizeof(mac    )-1);
    else if (strcmp(pName, "rssi"        ) == 0) strncpy(rssi   , pValue, sizeof(rssi   )-1);
    else if (strcmp(pName, "src"         ) == 0) strncpy(src    , pValue, sizeof(src    )-1);
    else if (strcmp(pName, "mqttCd"      ) == 0) strncpy(mqttCd , pValue, sizeof(mqttCd )-1);
    else if (strcmp(pName, "ts"          ) == 0) strncpy(ts     , pValue, sizeof(ts     )-1);
    else if (strcmp(pName, "state"       ) == 0) strncpy(state  , pValue, sizeof(state  )-1);
    else if (strcmp(pName, "sceneId"     ) == 0) strncpy(sceneId, pValue, sizeof(sceneId)-1);
    else if (strcmp(pName, "temp"        ) == 0) strncpy(temp   , pValue, sizeof(temp   )-1);
    else if (strcmp(pName, "dimming"     ) == 0) strncpy(dimming, pValue, sizeof(dimming)-1);
    else if (strcmp(pName, "id"          ) == 0) strncpy(id     , pValue, sizeof(id     )-1);
    else if (strcmp(pName, "result"      ) == 0) strncpy(result , pValue, sizeof(result )-1);
    else if (strcmp(pName, "success"     ) == 0) strncpy(success, pValue, sizeof(success)-1);
    else if (strcmp(pName, "fwVersion"   ) == 0) return; //Ignore
    else if (strcmp(pName, "homeId"      ) == 0) return; //Ignore
    else if (strcmp(pName, "phoneIp"     ) == 0) return; //Ignore
    else if (strcmp(pName, "phoneMac"    ) == 0) return; //Ignore
    else if (strcmp(pName, "register"    ) == 0) return; //Ignore
    else if (strcmp(pName, "rad"         ) == 0) return; //Ignore
    else if (strcmp(pName, "updateStatus") == 0) return; //Ignore
    else if (strcmp(pName, "speed"       ) == 0) return; //Ignore
    
    else LogF("Unrecognised name '%s'\r\n", pName);
}
static void clear()
{
    *method  = 0;
    *env     = 0;
    *mac     = 0;
    *rssi    = 0;
    *src     = 0;
    *mqttCd  = 0;
    *ts      = 0;
    *state   = 0;
    *sceneId = 0;
    *temp    = 0;
    *dimming = 0;
    *id      = 0;
    *result  = 0;
    *success = 0;
}
static void decodePacket(int length, char* pData)
{
    clear();
    int i = 0;
    bool finished = length <= 0;
    char  name[20];
    char  value[20];
    //char* pName = name;
    char* pValue = value;
    char* p = name;
    *pValue = 0;
    while(!finished)
    {
        switch (pData[i])
        {
            case 0: //Finish if EoS
                finished = true;
                *p = 0;
                handleNameValue(name, value);
                break;
            case '\"': //Ignore paranthesis
                break;
            case '{': //End of name value pair
            case '}':
            case ',':
                *p = 0;
                handleNameValue(name, value);
                p = name;
                *pValue = 0;
                *p = 0;
                break;
            case ':':
                *p = 0;
                p = value;
                *p = 0;
                break;
            default:
                *p++ = pData[i];
                break;
        }
        i++;
        if (i == length)
        {
            finished = true;
            *p = 0;
            handleNameValue(name, value);
        }
    }
    if (WizTrace) showTrace();
    
    if      (strcmp(method, "syncPilot"   ) == 0                                ) WizListStatusAdd(EthMacRemote, rssi, state, sceneId, dimming);
    else if (strcmp(method, "registration") == 0                                ) ; //Ignore registration method
    else if (strcmp(method, "firstBeat"   ) == 0                                ) ; //Ignore firstBeat method
    else if (strcmp(method, "syncAccEvt"  ) == 0                                ) ; //Ignore syncAccEvt method
    else if (strcmp(method, "updateOta"   ) == 0                                ) ; //Ignore updateOta method
    else if (strcmp(method, "setPilot"    ) == 0 && strcmp(success, "true") == 0) WizListReceivedSuccess(EthMacRemote);
    else                                                                          LogF("Unrecognised method '%s' in %.*s\r\n", method, length, pData);
}

static int handleReceivedPacket(uint16_t port, void (*traceback)(void), int dataLengthRx, char* pDataRx, int* pPataLengthTx, char* pDataTx)
{
    if (WizTrace)
    {
        LogTimeF("WIZ packet received on port %u\r\n", port);
        traceback();
        LogF("%.*s\r\n", dataLengthRx, pDataRx);
    }
    decodePacket(dataLengthRx, pDataRx);
    pPataLengthTx = 0;
    return 0;
}

void WizRecvInit()
{
    UserUdpPort1 = 38899;
    UserUdpPort2 = 38900;
    UserHandleReceivedUdpPacket = handleReceivedPacket;
}