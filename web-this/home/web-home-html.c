#include "web/http/http.h"
#include "web/web-add.h"

void WebHomeHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Home", NULL, "home.js");
    
    HttpAddText("<a href='/system' class='hamburger'>\r\n");
        HttpAddText("<div class='bar'  ></div>\r\n");
        HttpAddText("<div class='space'></div>\r\n");
        HttpAddText("<div class='bar'  ></div>\r\n");
        HttpAddText("<div class='space'></div>\r\n");
        HttpAddText("<div class='bar'  ></div>\r\n");
    HttpAddText("</a>\r\n");
    HttpAddText("<br/>\r\n");
    HttpAddText("<br/>\r\n");
    
    HttpAddText("<div id='ajax-date-local'></div>\r\n");
    HttpAddText("<br/>\r\n");
    WebAddAjaxLabelledSuffix("Board temperature"  , "ajax-board-html", "&deg;C");

    WebAddEnd();
}

