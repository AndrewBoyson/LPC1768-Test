#include <string.h>
#include <stdlib.h>

#include "web/http/http.h"
#include "board/values.h"

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
