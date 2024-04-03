#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "heating/boiler.h"
#include "heating/radiator.h"
#include "1-wire/1-wire/ds18b20.h"
#include "heating/program.h"
#include "1-wire/1-wire/1-wire-device.h"

void WebRadiatorAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
                        
    HttpAddInt16AsHex(RadiatorGetHallDS18B20Value()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetTankDS18B20Value()  ); HttpAddChar('\n');
    
    int byte = 0;
    if (ProgramTimerOutput            ) byte |= 0x01;
    if (RadiatorGetWinter()           ) byte |= 0x02;
    if (RadiatorGetOverride()         ) byte |= 0x04;
    if (RadiatorPump                  ) byte |= 0x08;
    if (RadiatorGetHotWaterProtectOn()) byte |= 0x10;
    if (RadiatorsOn                   ) byte |= 0x20;
    HttpAddByteAsHex(byte);    HttpAddChar('\n');
    
    HttpAddByteAsHex (RadiatorGetOverrideCancelHour()  ); HttpAddChar('\n');
    HttpAddByteAsHex (RadiatorGetOverrideCancelMinute()); HttpAddChar('\n');
    HttpAddInt16AsHex(RadiatorGetNightTemperature()    ); HttpAddChar('\n');
    HttpAddInt16AsHex(RadiatorGetFrostTemperature()    ); HttpAddChar('\n');
    HttpAddByteAsHex (RadiatorGetHotWaterProtectTemp() ); HttpAddChar('\n');
}

