#include <stdint.h>
#include <stdbool.h>

extern int  WebServerThisDecideWhatToDo(char *pPath, char* pLastModified);
extern bool WebServerThisHandleQuery   (int todo, char* pQuery);
extern bool WebServerThisPost          (int todo, int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete);
extern bool WebServerThisReply         (int todo);
