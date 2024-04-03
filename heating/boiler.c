#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "lpc1768/gpio.h"
#include "lpc1768/mstimer/mstimer.h"
#include "1-wire/1-wire/ds18b20.h"
#include "settings/settings.h"
#include "lpc1768/pwm/pwm.h"
#include "log/log.h"

#define BOILER_PUMP_DIR FIO2DIR(4) // P2.4 == p22
#define BOILER_PUMP_PIN FIO2PIN(4)
#define BOILER_PUMP_SET FIO2SET(4)
#define BOILER_PUMP_CLR FIO2CLR(4)

#define BOILER_CALL_DIR FIO2DIR(5) // P2.5 == p21
#define BOILER_CALL_PIN FIO2PIN(5)
#define BOILER_CALL_SET FIO2SET(5)
#define BOILER_CALL_CLR FIO2CLR(5)

#define PUMP_SPEED_CALLING_AUTO_ONLY  -1
#define PUMP_SPEED_CALLING_AUTO_TWEAK -2

#define MAX_SPEED 100

static char*     tankRom;
static char*   outputRom;
static char*   returnRom;

static int8_t  fullSpeedSecs;

static int8_t  tankSetPoint;
static int16_t tankHysteresis;
static int16_t _runOnDelta16ths;
static uint8_t runOnTime2s;

static int8_t  pumpSpeedCalling;
static int8_t  _rampDownTime;
static int8_t  boilerTarget;

static int8_t  _minSpeed;
static int8_t  _midSpeedPwm;
static int16_t _fullSpeedDeltaT16ths;

//Set in main scan
static int16_t _boilerOutput16ths   = DS18B20_ERROR_VALUE_NOT_SET;
static int16_t _boilerReturn16ths   = DS18B20_ERROR_VALUE_NOT_SET;
static int16_t _boilerRtnDel16ths   = DS18B20_ERROR_VALUE_NOT_SET;
static int16_t _boilerDeltaT16ths   = DS18B20_ERROR_VALUE_NOT_SET;
static bool    _boilerDeltaTisValid = false;

int16_t  BoilerGetTankDS18B20Value  () { return DS18B20ValueFromRom(tankRom);   } 
int16_t  BoilerGetOutputDS18B20Value() { return _boilerOutput16ths;    } 
int16_t  BoilerGetReturnDS18B20Value() { return _boilerReturn16ths;    }
int16_t  BoilerGetRtnDelDS18B20Value() { return _boilerRtnDel16ths;    }
int16_t  BoilerGetDeltaTDS18B20Value() { return _boilerDeltaT16ths;    } 
int      BoilerGetFullSpeedSecs     () { return fullSpeedSecs;         }
int      BoilerGetTankSetPoint      () { return tankSetPoint;          }
int      BoilerGetTankHysteresis    () { return tankHysteresis;        } 
int      BoilerGetRunOnDeltaT       () { return _runOnDelta16ths;      }
int      BoilerGetRunOnTime         () { return runOnTime2s << 1;      }
int      BoilerGetPumpSpeedCalling  () { return pumpSpeedCalling;      }
int      BoilerGetRampDownTime      () { return _rampDownTime;         }
int      BoilerGetOutputTarget      () { return boilerTarget;          }
int      BoilerGetMinSpeed          () { return _minSpeed;             }
int      BoilerGetMidSpeedPwm       () { return _midSpeedPwm;          }
int      BoilerGetFullSpeedDeltaT   () { return _fullSpeedDeltaT16ths; }

static void setTankRom          (char* value) { memcpy(  tankRom,       value, 8);  SetTankRom               (   tankRom            ); }
static void setOutputRom        (char* value) { memcpy(outputRom,       value, 8);  SetOutputRom             ( outputRom            ); }
static void setReturnRom        (char* value) { memcpy(returnRom,       value, 8);  SetReturnRom             ( returnRom            ); }
void BoilerSetFullSpeedSecs     (int   value) { fullSpeedSecs         = value;      SetBoilerFullSpeedSecs   (&fullSpeedSecs        ); }
void BoilerSetTankSetPoint      (int   value) { tankSetPoint          = value;      SetTankSetPoint          (&tankSetPoint         ); }
void BoilerSetTankHysteresis    (int   value) { tankHysteresis        = value;      SetTankHysteresis        (&tankHysteresis       ); }
void BoilerSetRunOnDeltaT       (int   value) { _runOnDelta16ths      = value;      SetBoilerRunOnDeltaT     (&_runOnDelta16ths     ); }
void BoilerSetRunOnTime         (int   value) { runOnTime2s           = value >> 1; SetBoilerRunOnTime2s     (&runOnTime2s          ); }
void BoilerSetPumpSpeedCalling  (int   value) { pumpSpeedCalling      = value;      SetBoilerPumpSpeedCalling(&pumpSpeedCalling     ); }
void BoilerSetRampDownTime      (int   value) { _rampDownTime         = value;      SetBoilerRampDownTime    (&_rampDownTime        ); }
void BoilerSetOutputTarget      (int   value) { boilerTarget          = value;      SetBoilerTarget          (&boilerTarget         ); }
void BoilerSetMinSpeed          (int   value) { _minSpeed             = value;      SetBoilerMinSpeed        (&_minSpeed            ); }
void BoilerSetMidSpeedPwm       (int   value) { _midSpeedPwm          = value;      SetBoilerMidSpeedPwm     (&_midSpeedPwm         ); }
void BoilerSetFullSpeedDeltaT   (int   value) { _fullSpeedDeltaT16ths = value;      SetBoilerFullSpeedDeltaT (&_fullSpeedDeltaT16ths); }

static int calculateBetweenTwoPoints(int x, int xA, int xB, int yA, int yB)
{
    float m = (float)(yB - yA) / (xB - xA);
    return  yA + m * (x - xA);
}
static int calculateSpeedFromDeltaT(int deltaT16ths)
{
    if (deltaT16ths < _fullSpeedDeltaT16ths) return MAX_SPEED;   //Needed in case deltaT16ths is negative or zero
    int speed = MAX_SPEED * _fullSpeedDeltaT16ths / deltaT16ths; //eg for 20 deg ==> 100 * (10 << 4) / (20 << 4) == 50
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    if (speed < _minSpeed) speed = _minSpeed;
    return speed;
}
static int calculateDeltaTFromSpeed(int speed)
{
    int deltaT16ths = MAX_SPEED * _fullSpeedDeltaT16ths / speed; //eg for speed = 50 ==> 100 * (10 << 4) / 50 == 20 << 4
    return deltaT16ths;
}

int BoilerInit()
{
      tankRom = DS18B20Roms + 8 * DS18B20RomCount;
    DS18B20RomSetters[DS18B20RomCount] = setTankRom;
    DS18B20RomNames[DS18B20RomCount] = "Tank";
    DS18B20RomCount++;
    
    outputRom = DS18B20Roms + 8 * DS18B20RomCount;
    DS18B20RomSetters[DS18B20RomCount] = setOutputRom;
    DS18B20RomNames[DS18B20RomCount] = "BlrOut";
    DS18B20RomCount++;
    
    returnRom = DS18B20Roms + 8 * DS18B20RomCount;
    DS18B20RomSetters[DS18B20RomCount] = setReturnRom;
    DS18B20RomNames[DS18B20RomCount] = "BlrRtn";
    DS18B20RomCount++;
    
    int address;
    uint8_t def1;
    int16_t def2;
    int32_t def4;
    GetTankRom               (   tankRom            );
    GetOutputRom             ( outputRom            );
    GetReturnRom             ( returnRom            );
    GetBoilerFullSpeedSecs   (&fullSpeedSecs        );
    GetTankSetPoint          (&tankSetPoint         );
    GetTankHysteresis        (&tankHysteresis       );
    GetBoilerRunOnDeltaT     (&_runOnDelta16ths     );
    GetBoilerRunOnTime2s     (&runOnTime2s          );
    GetBoilerPumpSpeedCalling(&pumpSpeedCalling     );
    GetBoilerRampDownTime    (&_rampDownTime        );
    GetBoilerTarget          (&boilerTarget         );
    GetBoilerMinSpeed        (&_minSpeed            );
    GetBoilerMidSpeedPwm     (&_midSpeedPwm         );
    GetBoilerFullSpeedDeltaT (&_fullSpeedDeltaT16ths);
    
    BOILER_PUMP_DIR = 1; //Set the direction to 1 == output
    BOILER_CALL_DIR = 1; //Set the direction to 1 == output

    PwmInit(400, 100);

    return 0;
}
bool BoilerCallEnable = true;
bool BoilerCall = false;
static void controlBoilerCall()
{
    if (BoilerCallEnable)
    {
        int tankTemp16ths = DS18B20ValueFromRom(tankRom);
        if (DS18B20IsValidValue(tankTemp16ths)) //Ignore values which are likely to be wrong
        {
            int  tankUpper16ths = tankSetPoint   << 4;
            int hysteresis16ths = tankHysteresis << 4;
            int  tankLower16ths = tankUpper16ths - hysteresis16ths;
        
            if (tankTemp16ths >= tankUpper16ths) BoilerCall = false;
            if (tankTemp16ths <= tankLower16ths) BoilerCall = true;
        }
    }
    else
    {
        BoilerCall = false;
    }
}
bool BoilerPump = false;
static void controlBoilerPump()
{
    static uint32_t msTimerBoilerPumpRunOn = 0;
    if (BoilerCall)
    {
        BoilerPump = true;
        msTimerBoilerPumpRunOn = MsTimerCount;
    }
    else
    {
        if (MsTimerRelative(msTimerBoilerPumpRunOn,    runOnTime2s * 2000)) BoilerPump = false;
        if (_boilerDeltaTisValid && _boilerDeltaT16ths < _runOnDelta16ths ) BoilerPump = false;
    }
}
int BoilerPumpFlow  = MAX_SPEED;
int BoilerPumpSpeed = MAX_SPEED;
int BoilerPumpPwm   = 0;
static int _autoSpeed = 0;
static void calculateAutoSpeed()
{
    if (!DS18B20IsValidValue(_boilerReturn16ths)) return;
    
    int target16ths = (int)boilerTarget << 4;
    int targetRise16ths = target16ths - _boilerReturn16ths; //eg 65 - eg 45 = 20*16 16ths
    
    _autoSpeed = calculateSpeedFromDeltaT(targetRise16ths);
}
static void controlBoilerPumpSpeed()
{
    static uint32_t msTimerReduction = 0;
    calculateAutoSpeed();
    if (BoilerCall)
    {
        if (pumpSpeedCalling < 0) BoilerPumpSpeed = _autoSpeed;        //Auto
        else                      BoilerPumpSpeed = pumpSpeedCalling;  //Manual
        msTimerReduction = MsTimerCount;
    }
    else
    {
        if (BoilerPumpSpeed > _minSpeed)
        {
            int msPerUnit = 1000 * _rampDownTime / (MAX_SPEED - _minSpeed);
            if (MsTimerRepetitive(&msTimerReduction, msPerUnit)) BoilerPumpSpeed--;
        }
        else
        {
            BoilerPumpSpeed = _minSpeed;
        }
    }
    if (BoilerPumpSpeed < _minSpeed) BoilerPumpSpeed = _minSpeed;
    if (BoilerPumpSpeed > MAX_SPEED) BoilerPumpSpeed = MAX_SPEED;
}
static int speedToPwm(int speed)
{
    #define MAX_SPEED_PWM 10
    #define MIN_SPEED_PWM 84
    /*
    PWM input signal [%] Pump status
    ≤ 10 Maximum speed
    > 10 / ≤ 84 Variable speed from minimum to maximum
    speed
    > 84 / ≤ 91 Minimum speed
    > 91/95 Hysteresis area: on/off
    > 95 / ≤ 100 Standby mode: off
    
    Max speed 100 is at fitted = 74; pwm = 10
    Min speed   0 is at fitted =  0; pwm = 84
    */
    if (speed <= _minSpeed) return MIN_SPEED_PWM;
    if (speed >= MAX_SPEED) return MAX_SPEED_PWM;
    int midSpeed = (_minSpeed + MAX_SPEED) / 2;
    if (speed < midSpeed) return calculateBetweenTwoPoints(speed, _minSpeed,  midSpeed, MIN_SPEED_PWM,   _midSpeedPwm);
    else                  return calculateBetweenTwoPoints(speed,  midSpeed, MAX_SPEED,  _midSpeedPwm,  MAX_SPEED_PWM);
    //int pwm = calculateBetweenTwoPoints(BoilerPumpSpeed, _minSpeed, MAX_SPEED, 84, 10);
    //if (pwm < 10) pwm = 10;
    //if (pwm > 84) pwm = 84;
    //BoilerPumpPwm = pwm;
}
#define TIME_BEFORE_TWEAK_SECS 120
static void tweakDeltaTs()
{
    if (pumpSpeedCalling != PUMP_SPEED_CALLING_AUTO_TWEAK) return;
    
    static uint32_t msTimerBoilerHeating = 0;
    if (!BoilerCall) msTimerBoilerHeating = MsTimerCount;
    if (!MsTimerRelative(msTimerBoilerHeating, TIME_BEFORE_TWEAK_SECS * 1000)) return;
    
    if (!_boilerDeltaTisValid) return;
    
    static int speedLastScan = -1;
    
    if (speedLastScan < MAX_SPEED && BoilerPumpSpeed == MAX_SPEED)
    {
        if (_fullSpeedDeltaT16ths > _boilerDeltaT16ths) _fullSpeedDeltaT16ths--;
        if (_fullSpeedDeltaT16ths < _boilerDeltaT16ths) _fullSpeedDeltaT16ths++;
    }
    
    speedLastScan = BoilerPumpSpeed;
}

#define TIME_BEFORE_DELTA_T_ALARM_SECS 300
#define DELTA_T_LIMIT 3
static void checkDeltaTs()
{   
    static uint32_t msTimerDeltaTNonConform = 0;
    if (!BoilerCall)
    {
        msTimerDeltaTNonConform = MsTimerCount;
        return;
    }
    
    int expectedDeltaT16ths = calculateDeltaTFromSpeed(BoilerPumpSpeed);
    
    bool deltaTisOk = _boilerDeltaTisValid &&
                      _boilerDeltaT16ths > (expectedDeltaT16ths - (DELTA_T_LIMIT << 4)) &&
                      _boilerDeltaT16ths < (expectedDeltaT16ths + (DELTA_T_LIMIT << 4));
    
    static bool hadAlarm = false;
    if (deltaTisOk) msTimerDeltaTNonConform = MsTimerCount;
    bool haveAlarm = MsTimerRelative(msTimerDeltaTNonConform, TIME_BEFORE_DELTA_T_ALARM_SECS * 1000);
    if (haveAlarm && !hadAlarm)
    {
        //BoilerCallEnable = false;
        LogTimeF("Boiler disabled - delta T was outside %d degree window for %d seconds", DELTA_T_LIMIT, TIME_BEFORE_DELTA_T_ALARM_SECS);
        Log(": actual ");
        DS18B20Log(_boilerDeltaT16ths);
        Log(", expected ");
        DS18B20Log(expectedDeltaT16ths);
        Log("\r\n");
    }
    hadAlarm = haveAlarm;
}
#define NUMBER_OF_STEPS 10
static int16_t _returns16ths[NUMBER_OF_STEPS]; //0 is last, 9th is first
static void delayLine()
{
    static uint32_t msTimerDelay = 0;
    if (BoilerPump)
    {
        int msTotal = 1000 * fullSpeedSecs * MAX_SPEED / BoilerPumpSpeed; //speed 10 ==> 10000; speed 100 ==> 1000
        int msPerStep = msTotal / NUMBER_OF_STEPS;
        if (MsTimerRelative(msTimerDelay, msPerStep))
        {
            for (int i = 0; i < NUMBER_OF_STEPS - 1; i++) _returns16ths[i] = _returns16ths[i + 1];
            _returns16ths[NUMBER_OF_STEPS - 1] = _boilerReturn16ths;
            msTimerDelay = MsTimerCount;
            //LogTimeF("Ms per step = %d, delayed boiler return = ", msPerStep);
            //DS18B20Log(_returns16ths[0]);
            //Log("\r\n");
        }
    }
    else
    {
        msTimerDelay = MsTimerCount;
        for (int i = 0; i < NUMBER_OF_STEPS; i++) _returns16ths[i] = DS18B20_ERROR_VALUE_NOT_SET;
    }
}

void BoilerMain()
{
    delayLine();
    _boilerOutput16ths   = DS18B20ValueFromRom(outputRom);
    _boilerReturn16ths   = DS18B20ValueFromRom(returnRom);
    _boilerRtnDel16ths   = _returns16ths[0];
    _boilerDeltaTisValid = DS18B20IsValidValue(_boilerOutput16ths) && DS18B20IsValidValue(_boilerRtnDel16ths);
    if (_boilerDeltaTisValid) _boilerDeltaT16ths   = _boilerOutput16ths - _boilerRtnDel16ths;
    else                      _boilerDeltaT16ths   = DS18B20_ERROR_VALUE_NOT_SET;
    
    controlBoilerCall();
    if (BoilerCall) BOILER_CALL_SET;
    else            BOILER_CALL_CLR;
    
    controlBoilerPump();
    if (BoilerPump) BOILER_PUMP_SET;
    else            BOILER_PUMP_CLR;
    
    controlBoilerPumpSpeed();
    BoilerPumpPwm = speedToPwm(BoilerPumpSpeed);
    PwmSet(BoilerPumpPwm);
    
    tweakDeltaTs();
    checkDeltaTs();
}