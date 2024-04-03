#include "web-this/web-nav-this.h"
#include "web/http/http.h"
#include "web/web-add.h"

void WebProgramHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Program", "settings.css", "program.js");
    WebAddNav(PROGRAM_PAGE);
    WebAddH1("Program");
    WebAddH2("Output");
    WebAddAjaxLed("Programmer output",  "ajax-program-toggle");
    
    WebAddH2("Program on and off times");
    WebAddAjaxInput("1", 15, "ajax-program-1", "program1");
    WebAddAjaxInput("2", 15, "ajax-program-2", "program2");
    WebAddAjaxInput("3", 15, "ajax-program-3", "program3");
    
    WebAddH2("Day to program association");
    WebAddAjaxInput("Mon", 2, "ajax-mon-program", "mon");
    WebAddAjaxInput("Tue", 2, "ajax-tue-program", "tue");
    WebAddAjaxInput("Wed", 2, "ajax-wed-program", "wed");
    WebAddAjaxInput("Thu", 2, "ajax-thu-program", "thu");
    WebAddAjaxInput("Fri", 2, "ajax-fri-program", "fri");
    WebAddAjaxInput("Sat", 2, "ajax-sat-program", "sat");
    WebAddAjaxInput("Sun", 2, "ajax-sun-program", "sun");
    
    WebAddH2("New day start time");
    WebAddAjaxInput("Hour", 2, "ajax-new-day-hour", "newdayhour");

    WebAddEnd();
}
