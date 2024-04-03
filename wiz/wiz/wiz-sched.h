
extern void WizSchedInit(void);
extern void WizSchedMain(void);
extern void WizSchedParseEvent(char* text, char* pDatum, int16_t* pMinutes);
extern void WizSchedHttp (void);

extern int WizSchedMinutesUtcToLocal(int minutes);
extern int WizSchedMinutesLocalToUtc(int minutes);

extern void WizSchedSetName    (int i, char*             name);
extern void WizSchedSetOnEvent (int i, char datum, int16_t minutes);
extern void WizSchedSetOffEvent(int i, char datum, int16_t minutes);

extern void WizSchedMoveUp  (int i);
extern void WizSchedMoveDown(int i);

#define WIZ_SCHED_MAX_ITEMS 5