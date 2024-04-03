#include "hall-pb.h"
#include "hall-led.h"
#include "program.h"
#include "radiator.h"
#include "boiler.h"
#include "hot-water.h"

void HeatingMain()
{
    HallPbMain();
    HallLedMain();
    ProgramMain();
    RadiatorMain();
    BoilerMain();
}
void HeatingInit()
{
    HallPbInit();
    HallLedInit();
    BoilerInit();
    RadiatorInit();
    ProgramInit();
    HotWaterInit();
}
