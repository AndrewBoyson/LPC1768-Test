#include "lpc1768/crt.h"
#include "lpc1768/lpc1768.h"
#include "lpc1768/led.h"
#include "clk/clk.h"
#include "log/log.h"
#include "net/net.h"
#include "1-wire/1-wire.h"
#include "board/board.h"
#include "settings/settings.h"
#include "web/web.h"
#include "wiz/wiz/wiz.h"
#include "crypto/crypto.h"
#include "crypto/pki/ser-cer.h"
#include "crypto/pki/pri-key.h"

int main()
{
				CrtInit();
            Lpc1768Init();
                LogInit(ClkNowTmUtc, 0);
                ClkInit();
    if (   SettingsInit()) goto end; //Uses log. Fram is initialised here
                NetInit("Test", "p1.31", "p1.30");
                WebInit("Test");
            OneWireInit("p0.26"); //P0.26 p18
			  BoardInit();
                WizInit();
				
			SerCerSourcePtr =
			#include "web-this/ser-cer.inc"
			;
			
			PriKeySourcePtr =
			#include "web-this/pri-key.inc"
			;
			CryptoInit();
				
    while (1)
    {
			LogMain();
			ClkMain();
			NetMain();
		OneWireMain();
		  BoardMain();
		Lpc1768Main();
			WizMain();
		 CryptoMain();
    }

end:
    Led1Set(1); Led2Set(1); Led3Set(1); Led4Set(1);
	while (1) {;}
    return 0;
}