#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "1-wire/1-wire/ds18b20.h"
#include "settings/settings.h"

static char* hotWaterRom;

uint16_t HotWaterGetDS18B20Value(){ return DS18B20ValueFromRom(hotWaterRom); }

static void setHotWaterRom(char* value) { memcpy(hotWaterRom,  value, 8); SetHotWaterRom(hotWaterRom); }

int HotWaterInit()
{
    hotWaterRom = DS18B20Roms + 8 * DS18B20RomCount;
    DS18B20RomSetters[DS18B20RomCount] = setHotWaterRom;
    DS18B20RomNames[DS18B20RomCount] = "HotWater";
    DS18B20RomCount++;
    
    GetHotWaterRom(hotWaterRom);
    
    return 0;
}