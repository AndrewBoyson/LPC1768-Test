#include <stdbool.h>


#include "lpc1768/gpio.h"
#include "radiator.h"
#include "program.h"
#include "lpc1768/mstimer/mstimer.h"
#include "hall-pb.h"

#define HALL_LED_DIR FIO0DIR(10) // P0.10 == p28;
#define HALL_LED_PIN FIO0PIN(10)
#define HALL_LED_SET FIO0SET(10)
#define HALL_LED_CLR FIO0CLR(10)

#define    POSITIVE_FLASH_MS   100
#define    NEGATIVE_FLASH_MS   500

void HallLedInit()
{
    HALL_LED_DIR = 1; //Set the direction to 1 == output
}
void HallLedMain()
{
    static uint32_t hallLedFlashMsTimer = 0;
    if (HallPbOverrideMode)
    {
        if (RadiatorsOn) HALL_LED_SET; else HALL_LED_CLR;
        hallLedFlashMsTimer = MsTimerCount;
    }
    else
    {
        int flashRate = RadiatorGetHotWaterProtectOn() ? POSITIVE_FLASH_MS : NEGATIVE_FLASH_MS;
        if (MsTimerRepetitive(&hallLedFlashMsTimer, flashRate))
        {
            static bool flash = false;
            flash = !flash;
            if (flash) HALL_LED_SET; else HALL_LED_CLR;
        }
    }
}