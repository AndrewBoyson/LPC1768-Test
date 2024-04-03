#include "lpc1768/startup.h"
#include "lpc1768/lpc1768.h"
#include "lpc1768-this/periphs.h"
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
#include "lpc1768/reset/restart.h"
#include "lpc1768-this/restart-this.h"
#include "crypto/crypto.h"
#include "wiz/wiz/wiz.h"
#include "lpc1768/debug.h"

int main()
{
    RestartZone = RESTART_ZONE_INIT;
				Startup();
            PeriphsInit();
                LedInit();
            Lpc1768Init();
                ClkInit();
                LogInit(ClkNowTmUtc, 0);
    if (   SettingsInit()) goto end; //Uses log. Fram is initialised here
             CryptoInit();           //Uses log
                NetInit();
                WebInit();
            OneWireInit();
             DeviceInit();
            HeatingInit();
    if (     ValuesInit()) goto end;
                WizInit();
				
    while (1)
    {
        RestartZone = RESTART_ZONE_LOG;              LogMain();
        RestartZone = RESTART_ZONE_CLOCK;            ClkMain();
        RestartZone = RESTART_ZONE_NET;              NetMain();
        RestartZone = RESTART_ZONE_VALUES;        ValuesMain();
        RestartZone = RESTART_ZONE_ONE_WIRE; if (OneWireMain()) break;
        RestartZone = RESTART_ZONE_DEVICE;   if ( DeviceMain()) break;
        RestartZone = RESTART_ZONE_HEATING;      HeatingMain();
        RestartZone = RESTART_ZONE_LPC1768;      Lpc1768Main();
        RestartZone = RESTART_ZONE_CRYPTO;        CryptoMain();
                                                     WizMain();
    }

end:
    Led1Set(1); Led2Set(1); Led3Set(1); Led4Set(1);
	while (1) {;}
    return 0;
}