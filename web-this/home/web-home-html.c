#include "web/http/http.h"
#include "web/web-add.h"

void WebHomeHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Home", NULL, "radiator.js");
    
    HttpAddText("<a href='/radiator' class='hamburger'>\r\n");
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
    WebAddAjaxLabelledSuffix("Hall temperature"  , "ajax-hall-html", "&deg;C");
    HttpAddText("<br/>\r\n");
    WebAddAjaxInputToggle   ("Radiators"         , "ajax-radiators-on-toggle", "htg-chg-override");
    HttpAddText("<br/>\r\n");
    WebAddAjaxInputToggle   ("Hot water priority", "ajax-hw-prot-toggle"     , "htg-chg-hw-prot" );
    HttpAddText("<br/>\r\n");
    WebAddAjaxInputToggle   ("Winter mode"       , "ajax-mode-toggle"        , "htg-chg-mode"    );
    HttpAddText("<br/>\r\n");

    WebAddEnd();
}

