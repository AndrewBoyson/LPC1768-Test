#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "lpc1768/gpio.h"
#include "program.h"
#include "1-wire/1-wire/ds18b20.h"
#include "settings/settings.h"
#include "boiler.h"
#include "radiator.h"
#include "clock/clk/clktime.h"
#include "clock/clk/clk.h"
#include "lpc1768/led.h"

#define RADIATOR_PUMP_DIR FIO2DIR(03) // P2.03 == p23;
#define RADIATOR_PUMP_PIN FIO2PIN(03)
#define RADIATOR_PUMP_SET FIO2SET(03)
#define RADIATOR_PUMP_CLR FIO2CLR(03)

static bool      htgOverride = false;
static char      htgWinter;
static char*     hallRom;
static uint8_t   overrideCancelHour;
static uint8_t   overrideCancelMinute;
static  int16_t  nightTemperature;
static  int16_t  frostTemperature;
static char      hotWaterProtectOn;
static  int8_t   hotWaterProtectTemp;

bool     RadiatorGetWinter              () { return (bool)htgWinter;              } 
bool     RadiatorGetOverride            () { return       htgOverride;            } 
bool     RadiatorGetHotWaterProtectOn   () { return (bool)hotWaterProtectOn;      }
uint16_t RadiatorGetHallDS18B20Value    () { return DS18B20ValueFromRom(hallRom); }
int      RadiatorGetOverrideCancelHour  () { return  (int)overrideCancelHour;     }
int      RadiatorGetOverrideCancelMinute() { return  (int)overrideCancelMinute;   }
int      RadiatorGetNightTemperature    () { return  (int)nightTemperature;       } 
int      RadiatorGetFrostTemperature    () { return  (int)frostTemperature;       } 
int      RadiatorGetHotWaterProtectTemp () { return  (int)hotWaterProtectTemp;    }

static void  setWinter              ( bool  v) {                             htgWinter            =     (char)v; SetRadiatorWinter      (&htgWinter           ); }
static void  setHallRom             ( char* v) {                             memcpy(hallRom,  v, 8);             SetHallRom             ( hallRom             ); }
void RadiatorSetOverrideCancelHour  ( int   v) { if (v > 23 || v < 0) v = 0; overrideCancelHour   =  (uint8_t)v; SetOverrideCancelHour  (&overrideCancelHour  ); }
void RadiatorSetOverrideCancelMinute( int   v) { if (v > 59 || v < 0) v = 0; overrideCancelMinute =  (uint8_t)v; SetOverrideCancelMinute(&overrideCancelMinute); }
void RadiatorSetNightTemperature    ( int   v) { if (v > 99 || v < 0) v = 0; nightTemperature     =  (int16_t)v; SetNightTemperature    (&nightTemperature    ); }
void RadiatorSetFrostTemperature    ( int   v) { if (v > 99 || v < 0) v = 0; frostTemperature     =  (int16_t)v; SetFrostTemperature    (&frostTemperature    ); }

static bool getProgramRequest()
{
    return htgWinter && ProgramTimerOutput;
}
static bool getTooCold()
{
    //See if the temperature is too low
    int  hallTemp16ths = DS18B20ValueFromRom(hallRom);
    int nightTemp16ths = nightTemperature << 4;
    int frostTemp16ths = frostTemperature << 4;
    
    static bool nightTooCold = false; //This is static to ride through invalid temperature reads
    static bool frostTooCold = false; //This is static to ride through invalid temperature reads
    
    if (DS18B20IsValidValue(hallTemp16ths))
    {
        if (hallTemp16ths < frostTemp16ths) frostTooCold = true;
        if (hallTemp16ths > frostTemp16ths) frostTooCold = false;
        if (hallTemp16ths < nightTemp16ths) nightTooCold = true; //Set   at 289 (18.06) rather than 288 (18.00)
        if (hallTemp16ths > nightTemp16ths) nightTooCold = false;//Reset at 287 (17.94). This prevent it following the flashing.
    }
    
    return (htgWinter && nightTooCold) || frostTooCold;
}
static bool getTankTooColdForShowers()
{
    return hotWaterProtectOn && BoilerGetTankDS18B20Value() < ((int16_t)hotWaterProtectTemp << 4); //Prevent the heating from robbing all the hot water for showers
}
static void autoCancelOverride()
{
    
    //Remove override at 11pm
    if (ClkTimeIsSet())
    {
        struct tm tm;
        ClkNowTmLocal(&tm);
        static bool cancelWasDue = false;
        bool cancelIsDue = tm.tm_hour == overrideCancelHour && tm.tm_min == overrideCancelMinute;
        if (cancelIsDue && !cancelWasDue && htgOverride) htgOverride = false;
        cancelWasDue = cancelIsDue;
    }
    
    //Remove override if no longer required
    bool thisProgramRequest = getProgramRequest();
    static bool previousProgramRequest = false;
    if (previousProgramRequest != thisProgramRequest && htgOverride) htgOverride = false;
    previousProgramRequest = thisProgramRequest;
}
bool RadiatorsOn = false;
bool RadiatorPump = false;
static void makeOutputs()
{
    bool programRequest = getProgramRequest();
    RadiatorsOn = htgOverride ? !programRequest : programRequest ;
    //RadiatorPump = RadiatorsOn && !getTankTooColdForShowers();
    RadiatorPump = (RadiatorsOn || getTooCold()) && !getTankTooColdForShowers();
}

void RadiatorSetWinter(bool value) //Summer is false, Winter is true
{
    if (htgWinter == (char)value) return; //Ignore no change
    
    bool programRequestBeforeSettingWinter = getProgramRequest();
    setWinter(value);                     //Change to the new value
    bool programRequestAfterSettingWinter = getProgramRequest();
    
    
    if (htgOverride) //Only deal with an override that is already set; if it wasn't set don't change it
    {
        if (htgWinter) //Summer -> Winter
        {
            if (programRequestAfterSettingWinter != programRequestBeforeSettingWinter) htgOverride = false; //Adjust the override to leave the heat as it was - off or on.
        }
        else //Winter -> Summer
        {
            htgOverride = false; //turn off the heat.
        }
    }
    makeOutputs();
}
void RadiatorSetOverride(bool value)
{
    htgOverride = value;
    makeOutputs();
}

void RadiatorChgWinter  (){ RadiatorSetWinter  (!RadiatorGetWinter  ()); }
void RadiatorChgOverride(){ RadiatorSetOverride(!RadiatorGetOverride()); }
void RadiatorChgHotWaterProtectOn  ()       {                             hotWaterProtectOn   = !hotWaterProtectOn; SetHotWaterProtectOn  (&hotWaterProtectOn  ); }
void RadiatorSetHotWaterProtectTemp( int v) { if (v > 99 || v < 0) v = 0; hotWaterProtectTemp = (int8_t)v;          SetHotWaterProtectTemp(&hotWaterProtectTemp); }
int RadiatorInit()
{
    hallRom = DS18B20Roms + 8 * DS18B20RomCount;
    DS18B20RomSetters[DS18B20RomCount] = setHallRom;
    DS18B20RomNames[DS18B20RomCount] = "Hall";
    DS18B20RomCount++;

    GetRadiatorWinter      (&htgWinter           );
    GetHallRom             ( hallRom             );
    GetOverrideCancelHour  (&overrideCancelHour  );
    GetOverrideCancelMinute(&overrideCancelMinute);
    GetNightTemperature    (&nightTemperature    );
    GetFrostTemperature    (&frostTemperature    );
    GetHotWaterProtectOn   (&hotWaterProtectOn   );
    GetHotWaterProtectTemp (&hotWaterProtectTemp );
    
    RADIATOR_PUMP_DIR = 1; //Set the direction to 1 == output
    
    return 0;
}
void RadiatorMain()
{
    autoCancelOverride();
    makeOutputs();
    
    if (RadiatorPump) RADIATOR_PUMP_SET;
    else              RADIATOR_PUMP_CLR;

}