#include "web/http/http.h"
#include "web-pages-this.h"
#include "web/web.h"

#define DO_HOME_HTML       DO_THIS + 0
#define DO_HOME_AJAX       DO_THIS + 1
#define DO_HOME_SCRIPT     DO_THIS + 2
#define DO_SYSTEM_HTML     DO_THIS + 3
#define DO_SYSTEM_AJAX     DO_THIS + 4
#define DO_SYSTEM_SCRIPT   DO_THIS + 5
#define DO_WIZ_HTML        DO_THIS + 6
#define DO_WIZ_AJAX        DO_THIS + 7
#define DO_WIZ_SCRIPT      DO_THIS + 8

int WebServerThisDecideWhatToDo(char *pPath, char* pLastModified)
{
    if (HttpSameStr(pPath, "/"             )) return DO_HOME_HTML;
    if (HttpSameStr(pPath, "/home-ajax"    )) return DO_HOME_AJAX;
    if (HttpSameStr(pPath, "/system"       )) return DO_SYSTEM_HTML;
    if (HttpSameStr(pPath, "/system-ajax"  )) return DO_SYSTEM_AJAX;
    if (HttpSameStr(pPath, "/wiz"          )) return DO_WIZ_HTML;
    if (HttpSameStr(pPath, "/wiz-ajax"     )) return DO_WIZ_AJAX;
    
    if (HttpSameStr(pPath, "/home.js"      )) return HttpSameDate(WebHomeScriptDate,     WebHomeScriptTime,     pLastModified) ? DO_NOT_MODIFIED : DO_HOME_SCRIPT;
    if (HttpSameStr(pPath, "/system.js"    )) return HttpSameDate(WebSystemScriptDate,   WebSystemScriptTime,   pLastModified) ? DO_NOT_MODIFIED : DO_SYSTEM_SCRIPT;
    if (HttpSameStr(pPath, "/wiz.js"       )) return HttpSameDate(WebWizScriptDate,      WebWizScriptTime,      pLastModified) ? DO_NOT_MODIFIED : DO_WIZ_SCRIPT;

    return DO_NOT_FOUND;
}

bool WebServerThisHandleQuery(int todo, char* pQuery)
{
    switch (todo)
    {
        case DO_SYSTEM_HTML:   WebSystemQuery  (pQuery); return true;
        case DO_SYSTEM_AJAX:   WebSystemQuery  (pQuery); return true;
        case DO_WIZ_HTML:      WebWizQuery     (pQuery); return true;
        case DO_WIZ_AJAX:      WebWizQuery     (pQuery); return true;
    }
    return false;
}
bool WebServerThisPost(int todo, int contentLength, int contentStart, int size, char* pRequestStream, uint32_t positionInRequestStream, bool* pComplete)
{
    return false;
}
bool WebServerThisReply(int todo)
{
    switch (todo)
    {
        case DO_HOME_HTML:       WebHomeHtml      (); return true;
        case DO_HOME_AJAX:       WebHomeAjax      (); return true;
        case DO_HOME_SCRIPT:     WebHomeScript    (); return true;
        case DO_SYSTEM_HTML:     WebSystemHtml    (); return true;
        case DO_SYSTEM_AJAX:     WebSystemAjax    (); return true;
        case DO_SYSTEM_SCRIPT:   WebSystemScript  (); return true;
        case DO_WIZ_HTML:        WebWizHtml       (); return true;
        case DO_WIZ_AJAX:        WebWizAjax       (); return true;
        case DO_WIZ_SCRIPT:      WebWizScript     (); return true;
    }
    return false;
}
