#include "lpc1768/reset/restart.h"
#include "restart-this.h"

const char* RestartThisGetZoneString()
{
    switch (RestartGetLastCause())
    {
        case RESTART_ZONE_NTP_CLIENT: return "NTP"    ;
        case RESTART_ZONE_VALUES    : return "Values" ;
        case RESTART_ZONE_ONE_WIRE  : return "1-wire" ;
        case RESTART_ZONE_DEVICE    : return "Device" ;
        case RESTART_ZONE_HEATING   : return "Heating";
        default                     : return "Unknown";
    }
}
