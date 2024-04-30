#include "lpc1768/crt.h"
#include "lpc1768/lpc1768.h"
#include "lpc1768/led.h"
#include "clk/clk.h"
#include "log/log.h"
#include "net/net.h"
#include "net/link/jack.h"
#include "1-wire/1-wire/1-wire.h"
#include "1-wire/1-wire/1-wire-bus.h"
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
                LogInit(ClkNowTmUtc, 0);
                ClkInit();
    if (   SettingsInit()) goto end; //Uses log. Fram is initialised here

				JackLinkLedDirPtr  = FIO1DIR_ALIAS_PTR(31); //P1.31 ==> p20 output
				JackLinkLedSetPtr  = FIO1SET_ALIAS_PTR(31);
				JackLinkLedClrPtr  = FIO1CLR_ALIAS_PTR(31);
				JackSpeedLedDirPtr = FIO1DIR_ALIAS_PTR(30); //P1.30 ==> p19 output
				JackSpeedLedSetPtr = FIO1SET_ALIAS_PTR(30);
				JackSpeedLedClrPtr = FIO1CLR_ALIAS_PTR(30);
                NetInit("Test");
				
                WebInit("Test");
				
			OneWireBusDirPtr = FIO0DIR_ALIAS_PTR(4); //pin 0.4 DIP 30
			OneWireBusPinPtr = FIO0PIN_ALIAS_PTR(4);
			OneWireBusSetPtr = FIO0SET_ALIAS_PTR(4);
			OneWireBusClrPtr = FIO0CLR_ALIAS_PTR(4);
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