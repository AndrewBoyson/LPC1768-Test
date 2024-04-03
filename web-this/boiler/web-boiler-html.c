#include "web/http/http.h"
#include "web-this/web-nav-this.h"
#include "web/web-add.h"

void WebBoilerHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Boiler", "settings.css", "boiler.js");
    WebAddNav(BOILER_PAGE);
    WebAddH1("Boiler");
    
    WebAddH2("Tank outputs");
    WebAddAjaxLed("Boiler call", "ajax-blr-call-toggle");
    
    WebAddH2("Tank inputs");
    WebAddAjaxLabelledSuffix("Tank",               "ajax-tank-html",       "&deg;C");

    WebAddH2("Tank parameters");
    WebAddAjaxInput("Tank 'off' setpoint (deg)", 2, "ajax-tank-set-point",     "tanksetpoint"    );
    WebAddAjaxInput("Tank 'on' temp drop (deg)", 2, "ajax-tank-hysteresis",    "tankhysteresis"  );

    WebAddH2("Pump outputs");
    WebAddAjaxLed("Boiler pump", "ajax-blr-pump-toggle");
    WebAddAjaxLabelledSuffix("Boiler pump speed (<span id='ajax-min-speed-text'></span>-100)",     "ajax-blr-pump-speed-html", "%");
    WebAddAjaxLabelledSuffix("Boiler pump pwm &nbsp;(84-10)",  "ajax-blr-pump-pwm-html"  , "%");
    
    WebAddH2("Pump inputs");
    WebAddAjaxLabelledSuffix("Boiler output",        "ajax-blr-out-html",  "&deg;C");
    WebAddAjaxLabelledSuffix("Boiler input",         "ajax-blr-rtn-html",  "&deg;C");
    WebAddAjaxLabelledSuffix("Boiler input aligned", "ajax-blr-aln-html",  "&deg;C");
    WebAddAjaxLabelledSuffix("Boiler &Delta;T",      "ajax-blr-rise-html", "&deg;C");

    WebAddH2("Pump parameters");
    WebAddAjaxInput("Full speed circuit time (sec)" , 2, "ajax-full-speed-secs"   , "fullspeedsecs"     );
    WebAddAjaxInput("Minimum speed (%)"             , 2, "ajax-min-speed-value"   , "blrriseat0"        );
    WebAddAjaxInput("Mid speed pwm"                 , 2, "ajax-pump-rise-at-50"   , "blrriseat50"       );
    
    WebAddH2("Pump parameters during boiler call");
    WebAddAjaxInputToggle("Boiler call enable", "ajax-blr-enable-toggle",      "boilercallenable");
    WebAddAjaxInput("Calling pump speed (%, A or T)", 2, "ajax-pump-speed-calling", "pumpspeedcalling"  );
    WebAddAjaxInput("Boiler output target (deg)"    , 2, "ajax-blr-output-target" , "boileroutputtarget");
    WebAddAjaxInput("Full speed &Delta;T"           , 3, "ajax-pump-rise-at-100"  , "blrriseat100"      );

    WebAddH2("Pump parameters during run on");
    WebAddAjaxInput("Run on min &Delta;T"           , 3, "ajax-blr-run-on-deg",     "boilerresidual"    );
    WebAddAjaxInput("Run on max time (sec)"         , 2, "ajax-blr-run-on-time",    "boilerrunon"       );
    WebAddAjaxInput("Ramp down time (sec)"          , 2, "ajax-pump-speed-run-on",  "pumpspeedrunon"    );

    WebAddEnd();
    
}