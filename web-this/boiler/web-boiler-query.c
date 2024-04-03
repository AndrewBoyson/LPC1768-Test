#include <stdlib.h>
#include <string.h>
#include "web/http/http.h"
#include "heating/boiler.h"

void WebBoilerQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
        
        
        if (HttpSameStr(pName, "boilercallenable"))
        {
            BoilerCallEnable = !BoilerCallEnable;
        }
        
        if (HttpSameStr(pName, "fullspeedsecs"  ))
        {
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetFullSpeedSecs(value);
            return;
        }
        if (HttpSameStr(pName, "tanksetpoint"  ))
        {
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetTankSetPoint(value);
            return;
        }
        if (HttpSameStr(pName, "tankhysteresis"))
        {
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetTankHysteresis(value);
            return;
        }
        if (HttpSameStr(pName, "boilerresidual"))
        {
            double value = HttpQueryValueAsDouble(pValue);
            BoilerSetRunOnDeltaT((int)(value * 16));
            return;
        }
        if (HttpSameStr(pName, "boilerrunon"   ))
        {
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetRunOnTime(value);
            return;
        }
        if (HttpSameStr(pName, "pumpspeedcalling"))
        {
            if (!pValue) return;
            switch (pValue[0])
            {
                case  0 : return;
                case 'a':
                case 'A': BoilerSetPumpSpeedCalling(-1); return;
                case 't':
                case 'T': BoilerSetPumpSpeedCalling(-2); return;
            }
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetPumpSpeedCalling(value);
            return;
        }
        if (HttpSameStr(pName, "pumpspeedrunon"))
        {
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetRampDownTime(value);
            return;
        }
        if (HttpSameStr(pName, "boileroutputtarget"))
        {
            int value = HttpQueryValueAsInt(pValue);
            BoilerSetOutputTarget(value);
            return;
        }
        if (HttpSameStr(pName, "blrriseat0"))
        {
            double value = HttpQueryValueAsDouble(pValue);
            BoilerSetMinSpeed(value);
            return;
        }
        if (HttpSameStr(pName, "blrriseat50"))
        {
            double value = HttpQueryValueAsDouble(pValue);
            BoilerSetMidSpeedPwm(value);
            return;
        }
        if (HttpSameStr(pName, "blrriseat100"))
        {
            double value = HttpQueryValueAsDouble(pValue);
            BoilerSetFullSpeedDeltaT((int)(value * 16));
            return;
        }
    }
}
