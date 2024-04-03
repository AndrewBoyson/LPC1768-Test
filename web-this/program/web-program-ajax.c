#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "heating/boiler.h"
#include "heating/radiator.h"
#include "1-wire/1-wire/ds18b20.h"
#include "heating/program.h"
#include "1-wire/1-wire/1-wire-device.h"

void WebProgramAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
    
    HttpAddNibbleAsHex(ProgramTimerOutput    ); HttpAddChar('\n');
    HttpAddByteAsHex  (ProgramGetNewDayHour()); HttpAddChar('\n');
    HttpAddNibbleAsHex(ProgramGetDay(1)      ); HttpAddChar(',');
    HttpAddNibbleAsHex(ProgramGetDay(2)      ); HttpAddChar(',');
    HttpAddNibbleAsHex(ProgramGetDay(3)      ); HttpAddChar(',');
    HttpAddNibbleAsHex(ProgramGetDay(4)      ); HttpAddChar(',');
    HttpAddNibbleAsHex(ProgramGetDay(5)      ); HttpAddChar(',');
    HttpAddNibbleAsHex(ProgramGetDay(6)      ); HttpAddChar(',');
    HttpAddNibbleAsHex(ProgramGetDay(0)      ); HttpAddChar('\n');
    ProgramSendAjax();
    
}
