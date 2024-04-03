#include <string.h>
#include <stdlib.h>

#include "web/http/http.h"
//#include "lpc1768/reset/watchdog.h"
//#include "1-wire/1-wire/1-wire-device.h"
//#include "heating/boiler.h"
//#include "heating/radiator.h"
#include "heating/values.h"
//#include "ip4.h"
//#include "settings.h"
//#include "clktime.h"

void WebSystemQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        HttpQueryUnencode(pValue);

        int value = HttpQueryValueAsInt(pValue);

        if (HttpSameStr(pName, "tftpserver"    )) ValuesSetServerName        (pValue          );
        if (HttpSameStr(pName, "tftpfilename"  )) ValuesSetFileName          (pValue          );
        if (HttpSameStr(pName, "tftpwriteint"  )) ValuesSetWriteSize         (value           );
        if (HttpSameStr(pName, "tftpreadint"   )) ValuesSetReadInterval      (value           );        
    }
}
