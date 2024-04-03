#include <stdbool.h>
#include <stdint.h>

extern int     BoilerGetFullSpeedSecs     (void); extern void BoilerSetFullSpeedSecs     (int value);
extern int     BoilerGetFullSpeedDeltaT   (void); extern void BoilerSetFullSpeedDeltaT   (int value);
extern int     BoilerGetTankSetPoint      (void); extern void BoilerSetTankSetPoint      (int value);
extern int     BoilerGetTankHysteresis    (void); extern void BoilerSetTankHysteresis    (int value);
extern int     BoilerGetRunOnDeltaT       (void); extern void BoilerSetRunOnDeltaT       (int value);
extern int     BoilerGetRunOnTime         (void); extern void BoilerSetRunOnTime         (int value);
extern int     BoilerGetPumpSpeedCalling  (void); extern void BoilerSetPumpSpeedCalling  (int value);
extern int     BoilerGetRampDownTime      (void); extern void BoilerSetRampDownTime      (int value);
extern int     BoilerGetOutputTarget      (void); extern void BoilerSetOutputTarget      (int value);

extern int     BoilerGetMinSpeed          (void); extern void BoilerSetMinSpeed          (int value);
extern int     BoilerGetMidSpeedPwm       (void); extern void BoilerSetMidSpeedPwm       (int value);

extern int16_t BoilerGetTankDS18B20Value  (void);
extern int16_t BoilerGetOutputDS18B20Value(void);
extern int16_t BoilerGetReturnDS18B20Value(void);
extern int16_t BoilerGetRtnDelDS18B20Value(void);
extern int16_t BoilerGetDeltaTDS18B20Value(void);

extern int     BoilerPumpFlow;
extern int     BoilerPumpSpeed;
extern int     BoilerPumpPwm;

extern bool BoilerCallEnable;
extern bool BoilerCall;
extern bool BoilerPump;

extern int  BoilerInit(void);
extern void BoilerMain(void);




