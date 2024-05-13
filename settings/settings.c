#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "clk/clkgov.h"
#include "fram/fram.h"
#include "log/log.h"
#include "net/net.h"
#include "net/link/link.h"
#include "net/udp/udp.h"
#include "net/udp/dns/dnsname.h"
#include "net/udp/dns/dnsquery.h"
#include "net/udp/dns/dnsreply.h"
#include "net/udp/dns/dnsserver.h"
#include "net/udp/dns/dnslabel.h"
#include "net/udp/ntp/ntp.h"
#include "net/udp/ntp/ntpclient.h"
#include "net/udp/dhcp/dhcp.h"
#include "net/ip6/icmp/ndp/ns.h"
#include "net/ip6/icmp/ndp/ra.h"
#include "net/ip6/icmp/ndp/rs.h"
#include "net/resolve/nr.h"
#include "1-wire/1-wire.h"
#include "net/ip4/icmp/echo4.h"
#include "net/ip6/icmp/echo6.h"
#include "net/ip6/icmp/dest6.h"
#include "net/resolve/ar4.h"
#include "net/resolve/ar6.h"
#include "net/arp/arp.h"
#include "net/ip4/ip4.h"
#include "net/ip6/ip6.h"
#include "net/tcp/tcp.h"
#include "web/web.h"
#include "net/udp/tftp/tftp.h"
#include "net/udp/dns/dns.h"
#include "wiz/wiz/wiz.h"
#include "wiz/wiz/wiz-list.h"
#include "wiz/wiz/wiz-sched.h"

static int iClkGovSlewDivisor      ;
static int iClkGovSlewMaxMs        ;
static int iClkGovPpbDivisor       ;
static int iClkGovPpbChangeMax     ;
static int iClkGovSyncedLimitNs    ;
static int iClkGovSyncedLimitPpb   ;
static int iClkGovSyncedHysterisNs ;
static int iClkGovSyncedHysterisPpb;
static int iClkGovMaxOffsetSecs    ;
static int iClkGov;

static int iOneWire;

static int iDnsSendRequestsViaIp4 ;
static int iNtpSendRequestsViaIp4 ;
static int iTftpSendRequestsViaIp4;

static int iNetHost;
static int iLogUart;
static int iNetStack;
static int iNetNewLine;
static int iNetVerbose;
static int iLink;
static int iDnsName;
static int iDnsQuery;
static int iDnsReply;
static int iDnsServer;
static int iNtp;
static int iDhcp;
static int iNsRecvSol;
static int iNsRecvAdv;
static int iNsSendSol;
static int iNr4;
static int iNr;
static int iNtpClient;
static int iEcho4;
static int iEcho6;
static int iDest6;
static int iRa;
static int iRs;
static int iAr4;
static int iAr6;
static int iArp;
static int iIp4;
static int iIp6;
static int iUdp;
static int iTcp;
static int iHttp;
static int iTftp;

//Values
static int iValuesServerName;
void SetValuesServerName(char* value) { FramWrite(iValuesServerName, DNS_MAX_LABEL_LENGTH, value); }
void GetValuesServerName(char* value) { FramRead (iValuesServerName, DNS_MAX_LABEL_LENGTH, value); value[DNS_MAX_LABEL_LENGTH] = 0; }

static int iValuesFileName;
void SetValuesFileName(char* value) { FramWrite(iValuesFileName, DNS_MAX_LABEL_LENGTH, value); }
void GetValuesFileName(char* value) { FramRead (iValuesFileName, DNS_MAX_LABEL_LENGTH, value); value[DNS_MAX_LABEL_LENGTH] = 0; }

static int iValuesWriteSize;
void SetValuesWriteSize(int* pValue) { FramWrite(iValuesWriteSize, 4, pValue); }
void GetValuesWriteSize(int* pValue) { FramRead (iValuesWriteSize, 4, pValue); }

static int iValuesReadInterval;
void SetValuesReadInterval(int* pValue) { FramWrite(iValuesReadInterval, 4, pValue); }
void GetValuesReadInterval(int* pValue) { FramRead (iValuesReadInterval, 4, pValue); }

static int iValuesStartTime;
void SetValuesStartTime(int64_t* pValue) { FramWrite(iValuesStartTime, 8, pValue); }
void GetValuesStartTime(int64_t* pValue) { FramRead (iValuesStartTime, 8, pValue); }

static int iValuesCount;
void SetValuesCount(int* pValue) { FramWrite(iValuesCount, 4, pValue); }
void GetValuesCount(int* pValue) { FramRead (iValuesCount, 4, pValue); }

static int iValuesData;
void GetValuesData(int index, char* pValue) { FramRead (iValuesData +     index, 1, pValue); }
void SetValuesData(int count, void* pValue) { FramWrite(iValuesData + 8 * count, 8, pValue); }

//Wiz settings
static int iWizTrace;
static int iWizMacs           [WIZ_LIST_MAX_ITEMS];
static int iWizNames          [WIZ_LIST_MAX_ITEMS];
static int iWizRooms          [WIZ_LIST_MAX_ITEMS];
static int iWizSchedNames     [WIZ_SCHED_MAX_ITEMS];
static int iWizSchedOnDatums  [WIZ_SCHED_MAX_ITEMS];
static int iWizSchedOnMinutes [WIZ_SCHED_MAX_ITEMS];
static int iWizSchedOffDatums [WIZ_SCHED_MAX_ITEMS];
static int iWizSchedOffMinutes[WIZ_SCHED_MAX_ITEMS];

void ChgTraceWiz                           () { WizTrace = !WizTrace; FramWrite(iWizTrace,                       1, &WizTrace); }
void SetWizListMac        (int i,    char* pMac    ) { FramWrite(iWizMacs           [i],                    6, pMac    ); }
void GetWizListMac        (int i,    char* pMac    ) { FramRead (iWizMacs           [i],                    6, pMac    ); }
void SetWizListName       (int i,    char* pText   ) { FramWrite(iWizNames          [i], WIZ_LIST_NAME_LENGTH, pText   ); }
void GetWizListName       (int i,    char* pText   ) { FramRead (iWizNames          [i], WIZ_LIST_NAME_LENGTH, pText   ); }
void SetWizListRoom       (int i,    char* pText   ) { FramWrite(iWizRooms          [i], WIZ_LIST_NAME_LENGTH, pText   ); }
void GetWizListRoom       (int i,    char* pText   ) { FramRead (iWizRooms          [i], WIZ_LIST_NAME_LENGTH, pText   ); }
void SetWizSchedName      (int i,    char* pText   ) { FramWrite(iWizSchedNames     [i], WIZ_LIST_NAME_LENGTH, pText   ); }
void GetWizSchedName      (int i,    char* pText   ) { FramRead (iWizSchedNames     [i], WIZ_LIST_NAME_LENGTH, pText   ); }
void SetWizSchedOnDatum   (int i,    char* pDatum  ) { FramWrite(iWizSchedOnDatums  [i],                    1, pDatum  ); }
void GetWizSchedOnDatum   (int i,    char* pDatum  ) { FramRead (iWizSchedOnDatums  [i],                    1, pDatum  ); }
void SetWizSchedOnMinutes (int i, int16_t* pMinutes) { FramWrite(iWizSchedOnMinutes [i],                    2, pMinutes); }
void GetWizSchedOnMinutes (int i, int16_t* pMinutes) { FramRead (iWizSchedOnMinutes [i],                    2, pMinutes); }
void SetWizSchedOffDatum  (int i,    char* pDatum  ) { FramWrite(iWizSchedOffDatums [i],                    1, pDatum  ); }
void GetWizSchedOffDatum  (int i,    char* pDatum  ) { FramRead (iWizSchedOffDatums [i],                    1, pDatum  ); }
void SetWizSchedOffMinutes(int i, int16_t* pMinutes) { FramWrite(iWizSchedOffMinutes[i],                    2, pMinutes); }
void GetWizSchedOffMinutes(int i, int16_t* pMinutes) { FramRead (iWizSchedOffMinutes[i],                    2, pMinutes); }

//Clock settings
void SetClockSlewDivisor      (int  value) { ClkGovSlewDivisor       = value; FramWrite(iClkGovSlewDivisor,       4, &ClkGovSlewDivisor      ); }
void SetClockSlewMaxMs        (int  value) { ClkGovSlewChangeMaxMs   = value; FramWrite(iClkGovSlewMaxMs,         4, &ClkGovSlewChangeMaxMs  ); }
void SetClockPpbDivisor       (int  value) { ClkGovFreqDivisor       = value; FramWrite(iClkGovPpbDivisor,        4, &ClkGovFreqDivisor      ); }
void SetClockPpbChangeMax     (int  value) { ClkGovFreqChangeMaxPpb  = value; FramWrite(iClkGovPpbChangeMax,      4, &ClkGovFreqChangeMaxPpb ); }
void SetClockSyncedLimitNs    (int  value) { ClkGovSlewSyncedLimNs   = value; FramWrite(iClkGovSyncedLimitNs,     4, &ClkGovSlewSyncedLimNs  ); }
void SetClockSyncedLimitPpb   (int  value) { ClkGovFreqSyncedLimPpb  = value; FramWrite(iClkGovSyncedLimitPpb,    4, &ClkGovFreqSyncedLimPpb ); }
void SetClockSyncedHysterisNs (int  value) { ClkGovSlewSyncedHysNs   = value; FramWrite(iClkGovSyncedHysterisNs,  4, &ClkGovSlewSyncedHysNs  ); }
void SetClockSyncedHysterisPpb(int  value) { ClkGovFreqSyncedHysPpb  = value; FramWrite(iClkGovSyncedHysterisPpb, 4, &ClkGovFreqSyncedHysPpb ); }
void SetClockMaxOffsetSecs    (int  value) { ClkGovSlewOffsetMaxSecs = value; FramWrite(iClkGovMaxOffsetSecs,     4, &ClkGovSlewOffsetMaxSecs); }
void ChgTraceSync             ()           { ClkGovTrace = !ClkGovTrace     ; FramWrite(iClkGov,                  1, &ClkGovTrace            ); }

//Log settings
void ChgLogUart               () {       LogUart         =       !LogUart;         FramWrite(iLogUart,    1,        &LogUart       ); }

//Heating settings
void ChgTraceOneWire          () {   OneWireTrace        =   !OneWireTrace       ; FramWrite(iOneWire,    1,   &OneWireTrace       ); }

//Net settings
void ChgDnsSendRequestsViaIp4    () {            DnsSendRequestsViaIp4 =             !DnsSendRequestsViaIp4; FramWrite( iDnsSendRequestsViaIp4,    1,             &DnsSendRequestsViaIp4); }
void ChgNtpSendRequestsViaIp4    () { NtpClientQuerySendRequestsViaIp4 =  !NtpClientQuerySendRequestsViaIp4; FramWrite( iNtpSendRequestsViaIp4,    1,  &NtpClientQuerySendRequestsViaIp4); }
void ChgTftpSendRequestsViaIp4   () {           TftpSendRequestsViaIp4 =            !TftpSendRequestsViaIp4; FramWrite(iTftpSendRequestsViaIp4,    1,            &TftpSendRequestsViaIp4); }

void SetTraceNetHost (char* text)
{
    int value = strtol(text, NULL, 16);
    NetTraceHost[1] =  value       & 0xFF;
    NetTraceHost[0] = (value >> 8) & 0xFF;
    FramWrite(iNetHost, 2, NetTraceHost);
}
void ChgTraceNetStack  () {       NetTraceStack   =       !NetTraceStack;   FramWrite(iNetStack,   1,        &NetTraceStack ); }
void ChgTraceNetNewLine() {       NetTraceNewLine =       !NetTraceNewLine; FramWrite(iNetNewLine, 1,       &NetTraceNewLine); }
void ChgTraceNetVerbose() {       NetTraceVerbose =       !NetTraceVerbose; FramWrite(iNetVerbose, 1,       &NetTraceVerbose); }
void ChgTraceLink      () {      LinkTrace        =      !LinkTrace;        FramWrite(iLink,       1,      &LinkTrace       ); }
void ChgTraceDnsName   () {   DnsNameTrace        =   !DnsNameTrace;        FramWrite(iDnsName,    1,   &DnsNameTrace       ); }
void ChgTraceDnsQuery  () {  DnsQueryTrace        =  !DnsQueryTrace;        FramWrite(iDnsQuery,   1,  &DnsQueryTrace       ); }
void ChgTraceDnsReply  () {  DnsReplyTrace        =  !DnsReplyTrace;        FramWrite(iDnsReply,   1,  &DnsReplyTrace       ); }
void ChgTraceDnsServer () { DnsServerTrace        = !DnsServerTrace;        FramWrite(iDnsServer,  1, &DnsServerTrace       ); }
void ChgTraceNtp       () {       NtpTrace        =       !NtpTrace;        FramWrite(iNtp,        1,       &NtpTrace       ); }
void ChgTraceDhcp      () {      DhcpTrace        =      !DhcpTrace;        FramWrite(iDhcp,       1,      &DhcpTrace       ); }
void ChgTraceNsRecvSol () {        NsTraceRecvSol =        !NsTraceRecvSol; FramWrite(iNsRecvSol,  1,        &NsTraceRecvSol); }
void ChgTraceNsRecvAdv () {        NsTraceRecvAdv =        !NsTraceRecvAdv; FramWrite(iNsRecvAdv,  1,        &NsTraceRecvAdv); }
void ChgTraceNsSendSol () {        NsTraceSendSol =        !NsTraceSendSol; FramWrite(iNsSendSol,  1,        &NsTraceSendSol); }
void ChgTraceNr4       () {       Nr4Trace        =       !Nr4Trace       ; FramWrite(iNr4,        1,       &Nr4Trace       ); }
void ChgTraceNr        () {        NrTrace        =        !NrTrace       ; FramWrite(iNr,         1,        &NrTrace       ); }
void ChgTraceNtpClient () { NtpClientTrace        = !NtpClientTrace       ; FramWrite(iNtpClient,  1, &NtpClientTrace       ); }
void ChgTraceEcho4     () {     Echo4Trace        =     !Echo4Trace       ; FramWrite(iEcho4,      1,     &Echo4Trace       ); }
void ChgTraceEcho6     () {     Echo6Trace        =     !Echo6Trace       ; FramWrite(iEcho6,      1,     &Echo6Trace       ); }
void ChgTraceDest6     () {     Dest6Trace        =     !Dest6Trace       ; FramWrite(iDest6,      1,     &Dest6Trace       ); }
void ChgTraceRa        () {        RaTrace        =        !RaTrace       ; FramWrite(iRa,         1,        &RaTrace       ); }
void ChgTraceRs        () {        RsTrace        =        !RsTrace       ; FramWrite(iRs,         1,        &RsTrace       ); }
void ChgTraceAr4       () {       Ar4Trace        =       !Ar4Trace       ; FramWrite(iAr4,        1,       &Ar4Trace       ); }
void ChgTraceAr6       () {       Ar6Trace        =       !Ar6Trace       ; FramWrite(iAr6,        1,       &Ar6Trace       ); }
void ChgTraceArp       () {       ArpTrace        =       !ArpTrace       ; FramWrite(iArp,        1,       &ArpTrace       ); }
void ChgTraceIp4       () {       Ip4Trace        =       !Ip4Trace       ; FramWrite(iIp4,        1,       &Ip4Trace       ); }
void ChgTraceIp6       () {       Ip6Trace        =       !Ip6Trace       ; FramWrite(iIp6,        1,       &Ip6Trace       ); }
void ChgTraceUdp       () {       UdpTrace        =       !UdpTrace       ; FramWrite(iUdp,        1,       &UdpTrace       ); }
void ChgTraceTcp       () {       TcpTrace        =       !TcpTrace       ; FramWrite(iTcp,        1,       &TcpTrace       ); }
void ChgTraceHttp      () {       WebTrace        =       !WebTrace       ; FramWrite(iHttp,       1,       &WebTrace       ); }
void ChgTraceTftp      () {      TftpTrace        =      !TftpTrace       ; FramWrite(iTftp,       1,      &TftpTrace       ); }

static int iServerName;
static int iInitialInterval;
static int iNormalInterval;
static int iRetryInterval;
static int iOffsetMs;
static int iMaxDelayMs;

void SetNtpClientServerName      ( char* value) { DnsLabelCopy(NtpClientQueryServerName,      value); FramWrite(iServerName,      DNS_MAX_LABEL_LENGTH,  NtpClientQueryServerName      ); }
void SetNtpClientInitialInterval ( int   value) { NtpClientQueryInitialInterval    = (int32_t)value ; FramWrite(iInitialInterval,                    4, &NtpClientQueryInitialInterval ); }
void SetNtpClientNormalInterval  ( int   value) { NtpClientQueryNormalInterval     = (int32_t)value ; FramWrite(iNormalInterval,                     4, &NtpClientQueryNormalInterval  ); }
void SetNtpClientRetryInterval   ( int   value) { NtpClientQueryRetryInterval      = (int32_t)value ; FramWrite(iRetryInterval,                      4, &NtpClientQueryRetryInterval   ); }
void SetNtpClientOffsetMs        ( int   value) { NtpClientReplyOffsetMs           = (int32_t)value ; FramWrite(iOffsetMs,                           4, &NtpClientReplyOffsetMs        ); }
void SetNtpClientMaxDelayMs      ( int   value) { NtpClientReplyMaxDelayMs         = (int32_t)value ; FramWrite(iMaxDelayMs,                         4, &NtpClientReplyMaxDelayMs      ); }

static int iBoardRom;

void SetBoardRom               (char*     value) { FramWrite(iBoardRom,              8,  value); }
void GetBoardRom               (char*     value) { FramRead (iBoardRom,              8,  value); }

int SettingsInit()
{
    int address;
    char b;
    //int8_t  def1;
    //int16_t def2;
    int32_t def4;
    int64_t def8;
    
    if (FramInit()) return -1; //Reserves 1 FRAM byte to detect if empty
    FramAllocate(4);           //Reserves 4 FRAM bytes where the watchdog used to hold the last program position
    
    def4 =    0; address = FramLoad( DNS_MAX_LABEL_LENGTH+1,  NtpClientQueryServerName,      NULL ); if (address < 0) return -1; iServerName      = address;
    def4 =    1; address = FramLoad(                      4, &NtpClientQueryInitialInterval, &def4); if (address < 0) return -1; iInitialInterval = address;
    def4 =  600; address = FramLoad(                      4, &NtpClientQueryNormalInterval,  &def4); if (address < 0) return -1; iNormalInterval  = address;
    def4 =   60; address = FramLoad(                      4, &NtpClientQueryRetryInterval,   &def4); if (address < 0) return -1; iRetryInterval   = address; 
    def4 =    0; address = FramLoad(                      4, &NtpClientReplyOffsetMs,        &def4); if (address < 0) return -1; iOffsetMs        = address; 
    def4 =   50; address = FramLoad(                      4, &NtpClientReplyMaxDelayMs,      &def4); if (address < 0) return -1; iMaxDelayMs      = address; 
    
    //Boiler
    address = FramAllocate(8); if (address < 0) return -1;
    address = FramAllocate(8); if (address < 0) return -1;
    address = FramAllocate(8); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(2); if (address < 0) return -1;
    address = FramAllocate(2); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
              FramAllocate(1); //Spare byte
    address = FramAllocate(1); if (address < 0) return -1;
              FramAllocate(1); //Spare byte
    address = FramAllocate(2); if (address < 0) return -1;
    
    //Radiator
    address = FramAllocate(1); if (address < 0) return -1;
              FramAllocate(1); //Spare byte
    address = FramAllocate(8); if (address < 0) return -1; iBoardRom             = address;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;
    address = FramAllocate(2); if (address < 0) return -1;
    address = FramAllocate(2); if (address < 0) return -1;
    address = FramAllocate(7); if (address < 0) return -1;
    
    address = FramAllocate(24); if (address < 0) return -1;
    address = FramAllocate(1); if (address < 0) return -1;

    address = FramAllocate(8); if (address < 0) return -1;
    
    def4 =       10; address = FramLoad( 4, &ClkGovSlewDivisor,       &def4);    if (address < 0) return -1; iClkGovSlewDivisor       = address;
    def4 =       20; address = FramLoad( 4, &ClkGovSlewChangeMaxMs,   &def4);    if (address < 0) return -1; iClkGovSlewMaxMs         = address;
    def4 =     1000; address = FramLoad( 4, &ClkGovFreqDivisor,       &def4);    if (address < 0) return -1; iClkGovPpbDivisor        = address;
    def4 = 10000000; address = FramLoad( 4, &ClkGovFreqChangeMaxPpb,  &def4);    if (address < 0) return -1; iClkGovPpbChangeMax      = address;
    def4 =   100000; address = FramLoad( 4, &ClkGovSlewSyncedLimNs,   &def4);    if (address < 0) return -1; iClkGovSyncedLimitNs     = address;
    def4 =   100000; address = FramLoad( 4, &ClkGovFreqSyncedLimPpb,  &def4);    if (address < 0) return -1; iClkGovSyncedLimitPpb    = address;
    def4 =    10000; address = FramLoad( 4, &ClkGovSlewSyncedHysNs,   &def4);    if (address < 0) return -1; iClkGovSyncedHysterisNs  = address;
    def4 =     1000; address = FramLoad( 4, &ClkGovFreqSyncedHysPpb,  &def4);    if (address < 0) return -1; iClkGovSyncedHysterisPpb = address;
    def4 =        3; address = FramLoad( 4, &ClkGovSlewOffsetMaxSecs, &def4);    if (address < 0) return -1; iClkGovMaxOffsetSecs     = address;
    address = FramLoad( 1, &b, NULL);   ClkGovTrace                     = b; if (address < 0) return -1; iClkGov      = address;
    address = FramLoad( 1, &b, NULL);  OneWireTrace                     = b; if (address < 0) return -1; iOneWire     = address;
    address = FramLoad( 1, &b, NULL);             DnsSendRequestsViaIp4 = b; if (address < 0) return -1; iDnsSendRequestsViaIp4  = address;
    address = FramLoad( 1, &b, NULL);  NtpClientQuerySendRequestsViaIp4 = b; if (address < 0) return -1; iNtpSendRequestsViaIp4  = address;
    address = FramLoad( 1, &b, NULL);            TftpSendRequestsViaIp4 = b; if (address < 0) return -1; iTftpSendRequestsViaIp4 = address;
    address = FramLoad( 1, &b, NULL);       LogUart          = b; if (address < 0) return -1; iLogUart     = address;
    address = FramLoad( 2,                  NetTraceHost, NULL) ; if (address < 0) return -1; iNetHost     = address;
    address = FramLoad( 1, &b, NULL);       NetTraceStack    = b; if (address < 0) return -1; iNetStack    = address;
    address = FramLoad( 1, &b, NULL);       NetTraceNewLine  = b; if (address < 0) return -1; iNetNewLine  = address;
    address = FramLoad( 1, &b, NULL);       NetTraceVerbose  = b; if (address < 0) return -1; iNetVerbose  = address;
    address = FramLoad( 1, &b, NULL);      LinkTrace         = b; if (address < 0) return -1; iLink        = address;
    address = FramLoad( 1, &b, NULL);   DnsNameTrace         = b; if (address < 0) return -1; iDnsName     = address;
    address = FramLoad( 1, &b, NULL);  DnsQueryTrace         = b; if (address < 0) return -1; iDnsQuery    = address;
    address = FramLoad( 1, &b, NULL);  DnsReplyTrace         = b; if (address < 0) return -1; iDnsReply    = address;
    address = FramLoad( 1, &b, NULL); DnsServerTrace         = b; if (address < 0) return -1; iDnsServer   = address;
    address = FramLoad( 1, &b, NULL);       NtpTrace         = b; if (address < 0) return -1; iNtp         = address;
    address = FramLoad( 1, &b, NULL);      DhcpTrace         = b; if (address < 0) return -1; iDhcp        = address;
    address = FramLoad( 1, &b, NULL);        NsTraceRecvSol  = b; if (address < 0) return -1; iNsRecvSol   = address;
    address = FramLoad( 1, &b, NULL);        NsTraceRecvAdv  = b; if (address < 0) return -1; iNsRecvAdv   = address;
    address = FramLoad( 1, &b, NULL);        NsTraceSendSol  = b; if (address < 0) return -1; iNsSendSol   = address;
    address = FramLoad( 1, &b, NULL);       Nr4Trace         = b; if (address < 0) return -1; iNr4         = address;
    address = FramLoad( 1, &b, NULL);        NrTrace         = b; if (address < 0) return -1; iNr          = address;
    address = FramLoad( 1, &b, NULL); NtpClientTrace         = b; if (address < 0) return -1; iNtpClient   = address;
    address = FramLoad( 1, &b, NULL);     Echo4Trace         = b; if (address < 0) return -1; iEcho4       = address;
    address = FramLoad( 1, &b, NULL);     Echo6Trace         = b; if (address < 0) return -1; iEcho6       = address;
    address = FramLoad( 1, &b, NULL);     Dest6Trace         = b; if (address < 0) return -1; iDest6       = address;
    address = FramLoad( 1, &b, NULL);        RaTrace         = b; if (address < 0) return -1; iRa          = address;
    address = FramLoad( 1, &b, NULL);        RsTrace         = b; if (address < 0) return -1; iRs          = address;
    address = FramLoad( 1, &b, NULL);       Ar4Trace         = b; if (address < 0) return -1; iAr4         = address;
    address = FramLoad( 1, &b, NULL);       Ar6Trace         = b; if (address < 0) return -1; iAr6         = address;
    address = FramLoad( 1, &b, NULL);       ArpTrace         = b; if (address < 0) return -1; iArp         = address;
    address = FramLoad( 1, &b, NULL);       Ip4Trace         = b; if (address < 0) return -1; iIp4         = address;
    address = FramLoad( 1, &b, NULL);       Ip6Trace         = b; if (address < 0) return -1; iIp6         = address;
    address = FramLoad( 1, &b, NULL);       UdpTrace         = b; if (address < 0) return -1; iUdp         = address;
    address = FramLoad( 1, &b, NULL);       TcpTrace         = b; if (address < 0) return -1; iTcp         = address;
    address = FramLoad( 1, &b, NULL);       WebTrace         = b; if (address < 0) return -1; iHttp        = address;
    address = FramLoad( 1, &b, NULL);      TftpTrace         = b; if (address < 0) return -1; iTftp        = address;

    address = FramAllocate(DNS_MAX_LABEL_LENGTH); if (address < 0) return -1; iValuesServerName   = address;
    FramAllocate(1); //Spare
    address = FramAllocate(DNS_MAX_LABEL_LENGTH); if (address < 0) return -1; iValuesFileName     = address;
    FramAllocate(1); //Spare
    address = FramAllocate(                   4); if (address < 0) return -1; iValuesWriteSize    = address;
    address = FramAllocate(                   4); if (address < 0) return -1; iValuesReadInterval = address;
    address = FramAllocate(                   8); if (address < 0) return -1; iValuesStartTime    = address;
    address = FramAllocate(                   4); if (address < 0) return -1; iValuesCount        = address;
    if (FramEmpty)
    {
        b    =   0; FramWrite(iValuesServerName  , 1, &b   );
        b    =   0; FramWrite(iValuesFileName    , 1, &b   );
        def4 = 100; FramWrite(iValuesWriteSize   , 4, &def4);
        def4 =  15; FramWrite(iValuesReadInterval, 4, &def4);
        def8 =   0; FramWrite(iValuesStartTime   , 8, &def8);
        def4 =   0; FramWrite(iValuesCount       , 4, &def4);
    }
    address = FramLoad( 1, &b, NULL);       WizTrace         = b; if (address < 0) return -1; iWizTrace    = address;
    
    for (int i = 0; i < WIZ_LIST_MAX_ITEMS; i++)
    {
        address = FramAllocate(                   6); if (address < 0) return -1; iWizMacs [i] = address;
        address = FramAllocate(WIZ_LIST_NAME_LENGTH); if (address < 0) return -1; iWizNames[i] = address;
        address = FramAllocate(WIZ_LIST_NAME_LENGTH); if (address < 0) return -1; iWizRooms[i] = address;
    }
    for (int i = 0; i < WIZ_SCHED_MAX_ITEMS; i++)
    {
        address = FramAllocate(WIZ_LIST_NAME_LENGTH); if (address < 0) return -1; iWizSchedNames     [i] = address;
        address = FramAllocate(                   1); if (address < 0) return -1; iWizSchedOnDatums  [i] = address;
        address = FramAllocate(                   2); if (address < 0) return -1; iWizSchedOnMinutes [i] = address;
        address = FramAllocate(                   1); if (address < 0) return -1; iWizSchedOffDatums [i] = address;
        address = FramAllocate(                   2); if (address < 0) return -1; iWizSchedOffMinutes[i] = address;
    }
    
    iValuesData = FramUsed;
    
    return 0;
    
}
