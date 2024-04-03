#include "web/http/http.h"
#include "web-this/web-nav-this.h"
#include "web/web-add.h"

void WebRadiatorHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Radiator", "settings.css", "radiator.js");
    WebAddNav(RADIATOR_PAGE);
    WebAddH1("Radiator");

    WebAddH2("Output");
    WebAddAjaxLed           ("Radiator pump",     "ajax-radiator-pump-toggle");
    
    WebAddH2("Inputs");
    WebAddAjaxLabelledSuffix("Hall temperature",  "ajax-hall-html", "&deg;C");
    WebAddAjaxLabelledSuffix("Tank temperature",  "ajax-tank-html", "&deg;C");
    WebAddAjaxLed           ("Programmer output", "ajax-program-toggle");
    
    WebAddH2("Parameters");
    WebAddAjaxInputToggle   ("Mode Summer|Winter",              "ajax-mode-toggle"           , "htg-chg-mode" );
    WebAddAjaxInput         ("Winter (night)",               2, "ajax-night-set-point"       , "nighttemp");
    WebAddAjaxInput         ("Summer (frost)",               2, "ajax-frost-set-point"       , "frosttemp");
    WebAddAjaxInputToggle   ("Override",                        "ajax-override-toggle"       , "htg-chg-override");
    WebAddAjaxInput         ("Override cancel time",         4, "ajax-override-cancel-minute", "overridecancelminute");
    WebAddAjaxInputToggle   ("Hot water priority",              "ajax-hw-prot-toggle"        , "htg-chg-hw-prot" );
    WebAddAjaxInput         ("Hot water priority tank min",  2, "ajax-hw-prot-temp"          , "hwprotecttemp");
    
    WebAddEnd();

}
