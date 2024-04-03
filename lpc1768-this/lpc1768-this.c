#include "lpc1768/rit.h"
#include "lpc1768/led.h"

void Lpc1768ThisInit()
{
    RitInit(1); //Start the Repetitive Interrupt Timer with a period of 1ms
}
void Lpc1768ThisMain()
{
}