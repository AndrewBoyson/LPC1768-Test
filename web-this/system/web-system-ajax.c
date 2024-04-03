#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "heating/values.h"
#include "fram/fram.h"

void WebSystemAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    HttpAddText      (ValuesGetServerName()  ); HttpAddChar('\n');
    HttpAddText      (ValuesGetFileName()    ); HttpAddChar('\n');
    HttpAddInt16AsHex(ValuesGetReadInterval()); HttpAddChar('\n');
    HttpAddInt16AsHex(ValuesGetWriteSize()   ); HttpAddChar('\n');
    HttpAddInt16AsHex(ValuesGetCount()       ); HttpAddChar('\n');
    HttpAddInt64AsHex(ValuesGetStartTime()   ); HttpAddChar('\n');
    HttpAddInt16AsHex(FramUsed               );
}

