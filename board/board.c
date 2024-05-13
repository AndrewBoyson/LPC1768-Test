#include <stdint.h>
#include <string.h>

#include "1-wire/ds18b20.h"
#include "settings/settings.h"

static char* boardRom;

uint16_t BoardGetDS18B20Value() { return DS18B20ValueFromRom(boardRom); }

static void  setBoardRom(char* v) { memcpy(boardRom,  v, 8); SetBoardRom(boardRom); }

char* addRom(void(*setter)(char*), char* name)
{
    char* pRom = DS18B20Roms + 8 * DS18B20RomCount;
    DS18B20RomSetters[DS18B20RomCount] = setter;
    DS18B20RomNames[DS18B20RomCount] = name;
    DS18B20RomCount++;
	return pRom;
}

void BoardInit()
{
	boardRom = DS18B20AddRom(setBoardRom, "Board");
    GetBoardRom(boardRom);
}
void BoardMain()
{
}