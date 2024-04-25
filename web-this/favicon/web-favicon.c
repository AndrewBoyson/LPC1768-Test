#include "web/http/http.h"

//Use http://tomeko.net/online_tools/file_to_hex.php to convert a favicon.ico into hex readable as an array
static const char bytes[] = {
#include "web-this/favicon/web-favicon.inc"
};

const char* WebFaviconDate = __DATE__;
const char* WebFaviconTime = __TIME__;
const int   WebFaviconSize = sizeof(bytes);

void WebFavicon()
{
    HttpOk("image/x-icon", "max-age=3600", WebFaviconDate, WebFaviconTime);
    HttpAddData(bytes, WebFaviconSize);
}
