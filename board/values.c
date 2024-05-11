#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "net/udp/tftp/tftp.h"
#include "net/udp/dns/dnslabel.h"
#include "clk/clk.h"
#include "clk/clktime.h"
#include "clk/clktm.h"
#include "lpc1768/mstimer/mstimer.h"
#include "1-wire/1-wire/ds18b20.h"
#include "board.h"
#include "log/log.h"
#include "net/net.h"
#include "settings/settings.h"

bool ValuesTrace = false;

static char    serverName[DNS_MAX_LABEL_LENGTH+1];
static char    fileName  [DNS_MAX_LABEL_LENGTH+1];
static int32_t writeSize;
static int32_t readInterval;
static int64_t startTime;
static int32_t count;

char*   ValuesGetServerName   (              ) { return         serverName;                    }
char*   ValuesGetFileName     (              ) { return         fileName;                      }
int     ValuesGetWriteSize    (              ) { return (int)   writeSize;                     }
int     ValuesGetReadInterval (              ) { return (int)   readInterval;                  }
void    ValuesGetStartTm      (struct tm* ptm) { ClkTimeToTmUtc(startTime, ptm);               }
int64_t ValuesGetStartTime    (              ) { return startTime >> CLK_TIME_ONE_SECOND_SHIFT;}
int     ValuesGetCount        (              ) { return (int)   count;                         }

void ValuesSetServerName    (char*   value) { DnsLabelCopy(serverName, value); SetValuesServerName  ( value); }
void ValuesSetFileName      (char*   value) { DnsLabelCopy(  fileName, value); SetValuesFileName    ( value); }
void ValuesSetWriteSize     (int     value) { writeSize     =          value ; SetValuesWriteSize   (&value); }
void ValuesSetReadInterval  (int     value) { readInterval  =          value ; SetValuesReadInterval(&value); }
static void    setStartTime (int64_t value) { startTime     =          value ; SetValuesStartTime   (&value); }
static void    setCount     (int     value) { count         =          value ; SetValuesCount       (&value); }

static void readValuesFromFram()
{
    GetValuesServerName  (         serverName);
    GetValuesFileName    (           fileName);
    GetValuesWriteSize   ((int*)   &writeSize);
    GetValuesReadInterval((int*)&readInterval);
    GetValuesStartTime   (         &startTime);
    GetValuesCount       ((int*)       &count);
}


static uint32_t readStartMs;

static int writeIndex;

static int nextByteOfWriteStream()
{
    int byteAfterData = count * 8;
    if (writeIndex >= byteAfterData)
    {
        setCount(0);
        return -1;
    }
    char c;
    GetValuesData(writeIndex, &c);
    writeIndex++;
    return c;
}

static void readValues()
{    
    uint64_t record = 0;
    uint16_t value;
    value = BoardGetDS18B20Value();
    value &= 0x0FFF;
    record = value; //0000 0000 0000 
    
    if (count <= 0)
    {
        count = 0;
        setStartTime(ClkNowTai());
    }
    
    SetValuesData(count, &record);
    setCount(count + 1);
}

static void writeValues()
{
    if (!serverName[0] ) return; //Do nothing if have no server name
    if (  !fileName[0] ) return; //Do nothing if have no file name
    if (TftpWriteStatus) return; //Do nothing if the TFTP client is busy

    strcpy(TftpServerName, serverName);
    struct tm tm;
    ClkTimeToTmUtc(startTime, &tm);
    int len = strftime(TftpFileName, DNS_MAX_LABEL_LENGTH+1, fileName, &tm);
    if (len == 0)
    {
        LogTimeF("Values - cannot make filename from template '%s'\r\n", count, fileName);
        return;
    }

    if (ValuesTrace)
    {
        if (NetTraceNewLine) Log("\r\n");
        LogTimeF("Values - requesting backup of %d values to %s\r\n", count, TftpFileName);
    }

    writeIndex = 0;
    TftpWriteStatus = TFTP_WRITE_STATUS_REQUEST; //This is reset by TFTP when finished
}

void ValuesMain()
{
    if (!readInterval) readStartMs = MsTimerCount;
    if (writeSize && count < writeSize && readInterval)
    {
        if (MsTimerRelative(readStartMs, readInterval * 1000))
        {
            readValues(); //Only read values if they are going to be backed up
            readStartMs = MsTimerCount;
        }
    }
    else
    {
        readStartMs = MsTimerCount;
    }
    if (writeSize && count >= writeSize) writeValues(); //Backup the values once the backup size is reached
}

void ValuesInit()
{
    readValuesFromFram();
    readStartMs = MsTimerCount;
    TftpGetNextByteFunction = nextByteOfWriteStream;
    if (count > 0) writeValues();     //Backup the values if there are any
}
