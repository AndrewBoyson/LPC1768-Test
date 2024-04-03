#include <stdbool.h>
#include <stdint.h>

extern bool     RadiatorGetWinter           (void); extern void RadiatorSetWinter          (bool value); extern void RadiatorChgWinter           (void);
extern bool     RadiatorGetOverride         (void); extern void RadiatorSetOverride        (bool value); extern void RadiatorChgOverride         (void);
extern bool     RadiatorGetHotWaterProtectOn(void);                                                      extern void RadiatorChgHotWaterProtectOn(void);

extern uint16_t RadiatorGetHallDS18B20Value(void);

extern int      RadiatorGetOverrideCancelHour  (void); extern void RadiatorSetOverrideCancelHour  (int value);
extern int      RadiatorGetOverrideCancelMinute(void); extern void RadiatorSetOverrideCancelMinute(int value);
extern int      RadiatorGetNightTemperature    (void); extern void RadiatorSetNightTemperature    (int value);
extern int      RadiatorGetFrostTemperature    (void); extern void RadiatorSetFrostTemperature    (int value);
extern int      RadiatorGetHotWaterProtectTemp (void); extern void RadiatorSetHotWaterProtectTemp (int value);

extern bool  RadiatorsOn;
extern bool  RadiatorPump;

extern int   RadiatorInit(void);
extern void  RadiatorMain(void);