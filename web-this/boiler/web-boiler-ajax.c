#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "heating/boiler.h"
#include "heating/radiator.h"
#include "1-wire/1-wire/ds18b20.h"
#include "heating/program.h"
#include "1-wire/1-wire/1-wire-device.h"

void WebBoilerAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);

    HttpAddInt16AsHex(BoilerGetTankDS18B20Value()  ); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetOutputDS18B20Value()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetReturnDS18B20Value()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetRtnDelDS18B20Value()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetDeltaTDS18B20Value()); HttpAddChar('\n');
    
    int nibble = 0;
    if (BoilerCall           ) nibble |= 1;
    if (BoilerPump           ) nibble |= 2;
    if (BoilerCallEnable     ) nibble |= 4;
    HttpAddNibbleAsHex (nibble); HttpAddChar('\n');
    
    HttpAddInt16AsHex(BoilerGetFullSpeedSecs     ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetTankSetPoint      ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetTankHysteresis    ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetRunOnDeltaT       ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetRunOnTime         ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerPumpSpeed);               HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerPumpPwm);                 HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetPumpSpeedCalling  ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetRampDownTime      ()); HttpAddChar('\n');
    HttpAddByteAsHex (BoilerGetOutputTarget      ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetMinSpeed          ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetMidSpeedPwm       ()); HttpAddChar('\n');
    HttpAddInt16AsHex(BoilerGetFullSpeedDeltaT   ()); HttpAddChar('\n');
}

