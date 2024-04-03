#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

static const char* script =
#include "web/common/web-ajax-class.inc"
#include "1-wire/web/web-1wire-class.inc"
#include "web-radiator-script.inc"
;
const char* WebRadiatorScriptDate = __DATE__;
const char* WebRadiatorScriptTime = __TIME__;

void WebRadiatorScript()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebRadiatorScriptDate, WebRadiatorScriptTime);
    HttpAddText(script);
}
