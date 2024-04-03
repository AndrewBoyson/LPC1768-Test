#include "web/http/http.h"

//Use http://tomeko.net/online_tools/cpp_text_escape.php   to convert from text to c-multiline
//Use http://tomeko.net/online_tools/cpp_text_unescape.php to convert from c-multiline to text

static const char* script =
#include "web/common/web-ajax-class.inc"
#include "web-system-script.inc"
;
const char* WebSystemScriptDate = __DATE__;
const char* WebSystemScriptTime = __TIME__;

void WebSystemScript()
{
    HttpOk("application/javascript; charset=UTF-8", "max-age=3600", WebSystemScriptDate, WebSystemScriptTime);
    HttpAddText(script);
}
