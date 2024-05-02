#include <stdbool.h>
#include <stdint.h>

#include "lpc1768/register.h"
#include "radiator.h"
#include "program.h"
#include "lpc1768/mstimer/mstimer.h"
#include "lpc1768/msrit.h"

#define HALL_PB_PIN FIO0PIN_ALIAS(05) // P0.05 == p29;

#define        DEBOUNCE_MS    20
#define      LONG_PRESS_MS  2000
#define INACTIVE_REVERT_MS 30000

static int ms  = 0;

static void (*msRitHook)();
    
static void msRitHandler()
{
    if (msRitHook) msRitHook(); //Call the RIT function chain before this
    
    if (HALL_PB_PIN) { if (ms < DEBOUNCE_MS) ms++; }
    else             { if (ms > 0          ) ms--; }
}

static bool getHallPbPressed()
{
    static bool pressed = false;
    if (ms >= DEBOUNCE_MS) pressed = false;
    if (ms <=           0) pressed = true;
    return pressed;
}

bool HallPbOverrideMode = true;

void HallPbInit()
{
    msRitHook = MsRitHook;
    MsRitHook = msRitHandler;
}
void HallPbMain()
{
    static uint32_t hallButtonPushedMsTimer    = 0;
    static uint32_t hallButtonNotPushedMsTimer = 0;
    
    static bool buttonWasPressed     = false;
    static bool buttonWasLongPressed = false;
    
    bool buttonIsPressed = getHallPbPressed();
    bool buttonIsLongPressed;
    if (buttonIsPressed)
    {
        buttonIsLongPressed = MsTimerRelative(hallButtonPushedMsTimer, LONG_PRESS_MS);
        hallButtonNotPushedMsTimer = MsTimerCount;
    }
    else
    {
        buttonIsLongPressed = false;
        hallButtonPushedMsTimer = MsTimerCount;
    }
    
    if (buttonIsLongPressed && !buttonWasLongPressed) HallPbOverrideMode = !HallPbOverrideMode;
    if (MsTimerRelative(hallButtonNotPushedMsTimer, INACTIVE_REVERT_MS)) HallPbOverrideMode = true;
    
    if (!buttonIsPressed && buttonWasPressed && !buttonWasLongPressed)
    {
        if (HallPbOverrideMode) RadiatorChgOverride();
        else                    RadiatorChgHotWaterProtectOn();
    }
    buttonWasPressed     = buttonIsPressed;
    buttonWasLongPressed = buttonIsLongPressed;
}