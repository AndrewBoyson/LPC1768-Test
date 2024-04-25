#include "lpc1768/crt.h"
#include "lpc1768/lpc1768.h"
#include "lpc1768/led.h"
#include "clock/clk/clk.h"
#include "log/log.h"
#include "fram/fram.h"
#include "net/net.h"
#include "1-wire/1-wire/1-wire.h"
#include "1-wire/1-wire/1-wire-device.h"
#include "heating/heating.h"
#include "settings/settings.h"
#include "web/web.h"
#include "heating/values.h"
#include "wiz/wiz/wiz.h"
#include "lpc1768/debug.h"

int main()
{
				CrtInit();
            Lpc1768Init();
                ClkInit();
                LogInit(ClkNowTmUtc, 0);
    if (   SettingsInit()) goto end; //Uses log. Fram is initialised here
                NetInit("Test");
                WebInit("Test");
            OneWireInit();
             DeviceInit();
            HeatingInit();
    if (     ValuesInit()) goto end;
                WizInit();
				
    while (1)
    {
                LogMain();
                ClkMain();
                NetMain();
             ValuesMain();
        if (OneWireMain()) break;
        if ( DeviceMain()) break;
            HeatingMain();
            Lpc1768Main();
                WizMain();
    }

end:
    Led1Set(1); Led2Set(1); Led3Set(1); Led4Set(1);
	while (1) {;}
    return 0;
}