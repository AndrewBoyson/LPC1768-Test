#include <stdbool.h>

extern void SetClockSlewDivisor      (int  value);
extern void SetClockSlewMaxMs        (int  value);
extern void SetClockPpbDivisor       (int  value);
extern void SetClockPpbChangeMax     (int  value);
extern void SetClockSyncedLimitNs    (int  value);
extern void SetClockSyncedLimitPpb   (int  value);
extern void SetClockSyncedHysterisNs (int  value);
extern void SetClockSyncedHysterisPpb(int  value);
extern void SetClockMaxOffsetSecs    (int  value);
extern void ChgDnsSendRequestsViaIp4 (void);
extern void ChgNtpSendRequestsViaIp4 (void);
extern void ChgTftpSendRequestsViaIp4(void);

extern void ChgTraceSync      (void);
extern void ChgTraceOneWire   (void);

extern void ChgTraceWiz          (void);
extern void SetWizListMac        (int i,    char* pMac    );
extern void GetWizListMac        (int i,    char* pMac    );
extern void SetWizListName       (int i,    char* pText   );
extern void GetWizListName       (int i,    char* pText   );
extern void SetWizListRoom       (int i,    char* pText   );
extern void GetWizListRoom       (int i,    char* pText   );
extern void SetWizSchedName      (int i,    char* pText   );
extern void GetWizSchedName      (int i,    char* pText   );
extern void SetWizSchedOnDatum   (int i,    char* pDatum  );
extern void GetWizSchedOnDatum   (int i,    char* pDatum  );
extern void SetWizSchedOnMinutes (int i, int16_t* pMinutes);
extern void GetWizSchedOnMinutes (int i, int16_t* pMinutes);
extern void SetWizSchedOffDatum  (int i,    char* pDatum  );
extern void GetWizSchedOffDatum  (int i,    char* pDatum  );
extern void SetWizSchedOffMinutes(int i, int16_t* pMinutes);
extern void GetWizSchedOffMinutes(int i, int16_t* pMinutes);

extern void SetTraceNetHost   (char* pValue);
extern void ChgLogUart        (void);
extern void ChgTraceNetStack  (void);
extern void ChgTraceNetNewLine(void);
extern void ChgTraceNetVerbose(void);
extern void ChgTraceLink      (void);
extern void ChgTraceDnsName   (void);
extern void ChgTraceDnsQuery  (void);
extern void ChgTraceDnsReply  (void);
extern void ChgTraceDnsServer (void);
extern void ChgTraceNtp       (void);
extern void ChgTraceDhcp      (void);
extern void ChgTraceNsRecvSol (void);
extern void ChgTraceNsRecvAdv (void);
extern void ChgTraceNsSendSol (void);
extern void ChgTraceNr4       (void);
extern void ChgTraceNr        (void);
extern void ChgTraceNtpClient (void);
extern void ChgTraceEcho4     (void);
extern void ChgTraceEcho6     (void);
extern void ChgTraceDest6     (void);
extern void ChgTraceRa        (void);
extern void ChgTraceRs        (void);
extern void ChgTraceAr4       (void);
extern void ChgTraceAr6       (void);
extern void ChgTraceArp       (void);
extern void ChgTraceIp4       (void);
extern void ChgTraceIp6       (void);
extern void ChgTraceUdp       (void);
extern void ChgTraceTcp       (void);
extern void ChgTraceHttp      (void);
extern void ChgTraceTftp      (void);


extern void SetNtpClientServerName      ( char* value);
extern void SetNtpClientInitialInterval ( int   value);
extern void SetNtpClientNormalInterval  ( int   value);
extern void SetNtpClientRetryInterval   ( int   value);
extern void SetNtpClientOffsetMs        ( int   value);
extern void SetNtpClientMaxDelayMs      ( int   value);

extern void SetBoardRom          (char*     value);
extern void GetBoardRom          (char*     value);

extern void SetValuesServerName  (char*     value);
extern void GetValuesServerName  (char*     value);
extern void SetValuesFileName    (char*     value);
extern void GetValuesFileName    (char*     value);
extern void SetValuesWriteSize   (int*     pValue);
extern void GetValuesWriteSize   (int*     pValue);
extern void SetValuesReadInterval(int*     pValue);
extern void GetValuesReadInterval(int*     pValue);
extern void SetValuesStartTime   (int64_t* pValue);
extern void GetValuesStartTime   (int64_t* pValue);
extern void SetValuesCount       (int*     pValue);
extern void GetValuesCount       (int*     pValue);
extern void GetValuesData(int index, char* pValue);
extern void SetValuesData(int count, void* pValue);

extern int  SettingsInit(void);