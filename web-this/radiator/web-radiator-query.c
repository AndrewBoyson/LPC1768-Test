#include <string.h>
#include <stdlib.h>

#include "web/http/http.h"
#include "heating/radiator.h"

void WebRadiatorQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        int value = HttpQueryValueAsInt(pValue);
        
        if (HttpSameStr(pName, "overridecancelminute"))
        {
            int hour   = value / 100;
            int minute = value % 100;
            RadiatorSetOverrideCancelHour  (hour);
            RadiatorSetOverrideCancelMinute(minute);
        }
        
        if (HttpSameStr(pName, "htg-chg-mode"        )) RadiatorChgWinter();
        if (HttpSameStr(pName, "htg-chg-override"    )) RadiatorChgOverride();
        if (HttpSameStr(pName, "htg-chg-hw-prot"     )) RadiatorChgHotWaterProtectOn();
        
        if (HttpSameStr(pName, "nighttemp"           )) RadiatorSetNightTemperature(value);
        if (HttpSameStr(pName, "frosttemp"           )) RadiatorSetFrostTemperature(value);
        if (HttpSameStr(pName, "hwprotecttemp"       )) RadiatorSetHotWaterProtectTemp(value);
    }
}

