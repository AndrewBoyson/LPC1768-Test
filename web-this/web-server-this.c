#include "web/http/http.h"
#include "web-pages-this.h"
#include "web/web.h"

#define DO_HOME_HTML       DO_THIS +  0
#define DO_PROGRAM_HTML    DO_THIS +  1
#define DO_PROGRAM_AJAX    DO_THIS +  2
#define DO_PROGRAM_SCRIPT  DO_THIS +  3
#define DO_RADIATOR_HTML   DO_THIS +  4
#define DO_RADIATOR_AJAX   DO_THIS +  5
#define DO_RADIATOR_SCRIPT DO_THIS +  6
#define DO_BOILER_HTML     DO_THIS +  7
#define DO_BOILER_AJAX     DO_THIS +  8
#define DO_BOILER_SCRIPT   DO_THIS +  9
#define DO_SYSTEM_HTML     DO_THIS + 10
#define DO_SYSTEM_AJAX     DO_THIS + 11
#define DO_SYSTEM_SCRIPT   DO_THIS + 12
#define DO_1WIRE_HTML      DO_THIS + 13
#define DO_1WIRE_AJAX      DO_THIS + 14
#define DO_1WIRE_SCRIPT    DO_THIS + 15
#define DO_WIZ_HTML        DO_THIS + 16
#define DO_WIZ_AJAX        DO_THIS + 17
#define DO_WIZ_SCRIPT      DO_THIS + 18

int WebServerThisDecideWhatToDo(char *pPath, char* pLastModified)
{
    if (HttpSameStr(pPath, "/"             )) return DO_HOME_HTML;
    if (HttpSameStr(pPath, "/program"      )) return DO_PROGRAM_HTML;
    if (HttpSameStr(pPath, "/program-ajax" )) return DO_PROGRAM_AJAX;
    if (HttpSameStr(pPath, "/radiator"     )) return DO_RADIATOR_HTML;
    if (HttpSameStr(pPath, "/radiator-ajax")) return DO_RADIATOR_AJAX;
    if (HttpSameStr(pPath, "/boiler"       )) return DO_BOILER_HTML;
    if (HttpSameStr(pPath, "/boiler-ajax"  )) return DO_BOILER_AJAX;
    if (HttpSameStr(pPath, "/system"       )) return DO_SYSTEM_HTML;
    if (HttpSameStr(pPath, "/system-ajax"  )) return DO_SYSTEM_AJAX;
    if (HttpSameStr(pPath, "/1wire"        )) return DO_1WIRE_HTML;
    if (HttpSameStr(pPath, "/1wire-ajax"   )) return DO_1WIRE_AJAX;
    if (HttpSameStr(pPath, "/wiz"          )) return DO_WIZ_HTML;
    if (HttpSameStr(pPath, "/wiz-ajax"     )) return DO_WIZ_AJAX;
    
    if (HttpSameStr(pPath, "/program.js"   )) return HttpSameDate(WebProgramScriptDate,  WebProgramScriptTime,  pLastModified) ? DO_NOT_MODIFIED : DO_PROGRAM_SCRIPT;
    if (HttpSameStr(pPath, "/radiator.js"  )) return HttpSameDate(WebRadiatorScriptDate, WebRadiatorScriptTime, pLastModified) ? DO_NOT_MODIFIED : DO_RADIATOR_SCRIPT;
    if (HttpSameStr(pPath, "/boiler.js"    )) return HttpSameDate(WebBoilerScriptDate,   WebBoilerScriptTime,   pLastModified) ? DO_NOT_MODIFIED : DO_BOILER_SCRIPT;
    if (HttpSameStr(pPath, "/system.js"    )) return HttpSameDate(WebSystemScriptDate,   WebSystemScriptTime,   pLastModified) ? DO_NOT_MODIFIED : DO_SYSTEM_SCRIPT;
    if (HttpSameStr(pPath, "/1wire.js"     )) return HttpSameDate(WebOneWireScriptDate,  WebOneWireScriptTime,  pLastModified) ? DO_NOT_MODIFIED : DO_1WIRE_SCRIPT;
    if (HttpSameStr(pPath, "/wiz.js"       )) return HttpSameDate(WebWizScriptDate,      WebWizScriptTime,      pLastModified) ? DO_NOT_MODIFIED : DO_WIZ_SCRIPT;

    return DO_NOT_FOUND;
}

bool WebServerThisHandleQuery(int todo, char* pQuery)
{
    switch (todo)
    {
        case DO_PROGRAM_AJAX:  WebProgramQuery (pQuery); return true;
        case DO_PROGRAM_HTML:  WebProgramQuery (pQuery); return true;
        case DO_RADIATOR_HTML: WebRadiatorQuery(pQuery); return true;
        case DO_RADIATOR_AJAX: WebRadiatorQuery(pQuery); return true;
        case DO_BOILER_HTML:   WebBoilerQuery  (pQuery); return true;
        case DO_BOILER_AJAX:   WebBoilerQuery  (pQuery); return true;
        case DO_SYSTEM_HTML:   WebSystemQuery  (pQuery); return true;
        case DO_SYSTEM_AJAX:   WebSystemQuery  (pQuery); return true;
        case DO_1WIRE_HTML:    WebOneWireQuery (pQuery); return true;
        case DO_1WIRE_AJAX:    WebOneWireQuery (pQuery); return true;
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
        case DO_PROGRAM_HTML:    WebProgramHtml   (); return true;
        case DO_PROGRAM_AJAX:    WebProgramAjax   (); return true;
        case DO_PROGRAM_SCRIPT:  WebProgramScript (); return true;
        case DO_RADIATOR_HTML:   WebRadiatorHtml  (); return true;
        case DO_RADIATOR_AJAX:   WebRadiatorAjax  (); return true;
        case DO_RADIATOR_SCRIPT: WebRadiatorScript(); return true;
        case DO_BOILER_HTML:     WebBoilerHtml    (); return true;
        case DO_BOILER_AJAX:     WebBoilerAjax    (); return true;
        case DO_BOILER_SCRIPT:   WebBoilerScript  (); return true;
        case DO_SYSTEM_HTML:     WebSystemHtml    (); return true;
        case DO_SYSTEM_AJAX:     WebSystemAjax    (); return true;
        case DO_SYSTEM_SCRIPT:   WebSystemScript  (); return true;
        case DO_1WIRE_HTML:      WebOneWireHtml   (); return true;
        case DO_1WIRE_AJAX:      WebOneWireAjax   (); return true;
        case DO_1WIRE_SCRIPT:    WebOneWireScript (); return true;
        case DO_WIZ_HTML:        WebWizHtml       (); return true;
        case DO_WIZ_AJAX:        WebWizAjax       (); return true;
        case DO_WIZ_SCRIPT:      WebWizScript     (); return true;
    }
    return false;
}
