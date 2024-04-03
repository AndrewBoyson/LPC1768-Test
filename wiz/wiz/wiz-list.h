
extern void  WizListMain(void);
extern void  WizListInit(void);
extern void  WizListHttp(void);

extern char* WizListNameFromMac(char* mac );
extern char* WizListNameToMac  (char* name);
extern void  WizListStatusAdd  (char* mac, char* rssi, char* state, char* sceneId, char* dimming);

extern void  WizListSetMac (int i, char* mac );
extern void  WizListSetName(int i, char* name);
extern void  WizListSetRoom(int i, char* room);
extern void  WizListGetMac (int i, char* mac );
extern void  WizListGetName(int i, char* name);
extern void  WizListGetRoom(int i, char* room);
extern int   WizListGetIndexFromMac(char* mac);

extern void  WizListMoveUp  (int i);
extern void  WizListMoveDown(int i);

extern void  WizListTurnIndexOn (int i);
extern void  WizListTurnIndexOff(int i);
extern void  WizListTurnAllOn   (void);
extern void  WizListTurnAllOff  (void);
extern void  WizListTurnRoomOn  (char* room);
extern void  WizListTurnRoomOff (char* room);
extern void  WizListTurnMacOn   (char* mac);
extern void  WizListTurnMacOff  (char* mac);

extern void  WizListReceivedSuccess(char* mac);

#define WIZ_LIST_NAME_LENGTH 12
#define WIZ_LIST_MAX_ITEMS   15
