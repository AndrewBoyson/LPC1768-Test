#include <stdio.h>

#include "net/action.h"
#include "net/eth/eth.h"
#include "net/ip4/ip4addr.h"
#include "net/resolve/ar4.h"
#include "net/eth/mac.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/user.h"
#include "log/log.h"
#include "wiz/wiz/wiz.h"

#define TODO_NOTHING      0
#define TODO_REGISTER     1
#define TODO_TURN_ALL_ON  2
#define TODO_TURN_ALL_OFF 3
#define TODO_TURN_ONE_ON  4
#define TODO_TURN_ONE_OFF 5

static int todo = 0;
static char _mac[6];

void WizSendRegister  (         ) { todo = TODO_REGISTER;     }
void WizSendTurnAllOn (         ) { todo = TODO_TURN_ALL_ON;  }
void WizSendTurnAllOff(         ) { todo = TODO_TURN_ALL_OFF; }
void WizSendTurnOneOn (char* mac) { todo = TODO_TURN_ONE_ON;  MacCopy(_mac, mac); }
void WizSendTurnOneOff(char* mac) { todo = TODO_TURN_ONE_OFF; MacCopy(_mac, mac); }
bool WizSendBusy      (         ) { return todo != TODO_NOTHING; }

int pollForPacketToSend(int type, int* pDataLength, char* pData)
{
    if (!todo) return  DO_NOTHING;
    if (type != ETH_IPV4) return DO_NOTHING; //WIZ only works under IPv4
    
    char* p = pData;
    char* pText = 0;
    int len = 0;
    int i = 0;
    int action = DO_NOTHING;
    switch (todo)
    {
        case TODO_REGISTER:
            pText  = "{\"method\":\"registration\",\"id\":100,\"params\":{\"phoneIp\":\"";
            while (*pText) *p++ = *pText++;
            
            len = Ip4AddrToString(DhcpLocalIp, *pDataLength - (p - pData), p); // 192.168.0.43
            p += len;
            
            pText = "\",\"phoneMac\":\"";
            while (*pText) *p++ = *pText++;
            
            for (i = 0; i < 6; i++)//0002f7f3c52b
            {
                sprintf(p, "%02X", MacLocal[i]);
                p += 2;
            }
            
            pText = "\",\"register\":true}}";
            while (*pText) *p++ = *pText++;
            
            action = BROADCAST;
            UserUdpDstPort = 38900;
            break;
        case TODO_TURN_ALL_ON:
            pText  = "{\"method\":\"setPilot\",\"id\":100,\"params\":{\"state\":1}}";
            while (*pText) *p++ = *pText++;
            action = BROADCAST;
            UserUdpDstPort = 38899;
            break;
        case TODO_TURN_ALL_OFF:
            pText  = "{\"method\":\"setPilot\",\"id\":100,\"params\":{\"state\":0}}";
            while (*pText) *p++ = *pText++;
            action = BROADCAST;
            UserUdpDstPort = 38899;
            break;
        case TODO_TURN_ONE_ON:
            pText  = "{\"method\":\"setPilot\",\"id\":100,\"params\":{\"state\":1}}";
            while (*pText) *p++ = *pText++;
            UserUdpDstPort = 38899;
            UserIp4 = Ar4GetIpFromMac(_mac);
            if (!UserIp4)
            {
                if (WizTrace)
                {
                    LogTimeF("WIZ - Could not get Ip4 address for MAC ");
                    MacLog(_mac);
                    Log("\r\n");
                }
                break;
            }
            action = UNICAST_USER;
            break;
        case TODO_TURN_ONE_OFF:
            pText  = "{\"method\":\"setPilot\",\"id\":100,\"params\":{\"state\":0}}";
            while (*pText) *p++ = *pText++;
            UserUdpDstPort = 38899;
            UserIp4 = Ar4GetIpFromMac(_mac);
            if (!UserIp4)
            {
                if (WizTrace)
                {
                    LogTimeF("WIZ - Could not get Ip4 address for MAC ");
                    MacLog(_mac);
                    Log("\r\n");
                }
                break;
            }
            action = UNICAST_USER;
            break;
        default:
            LogTimeF("WIZ - unknown todo %d\r\n", todo);
            break;
    }
    todo = TODO_NOTHING;
    if (!action) return DO_NOTHING;
    
    *pDataLength = p - pData;
    UserUdpSrcPort = 38899; //This is the port to which replies should be sent
    
    if (WizTrace)
    {
        LogTimeF("WIZ packet sent\r\n");
        LogF("%.*s\r\n", *pDataLength, pData);
    }
    return ActionMakeFromDestAndTrace(action, WizTrace);
}

void WizSendInit()
{
    UserPollForUdpPacketToSend = pollForPacketToSend;
}