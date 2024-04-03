#include <stdio.h>

#include "web/http/http.h"
#include "web-this/web-nav-this.h"
#include "web/web-add.h"

static void addButton(char* name, int index, char* value, char* label)
{
    HttpAddF("<button style='width:0.8em; margin:0; margin-left:0.1em; padding:0;' onclick='AjaxSendNameValue(\"%s%d\", \"%s\")'>%s</button>\r\n", name, index, value, label);
}

static void addLight(int index)
{   
    HttpAddText("<div style='display:flex; align-items:center;'>\r\n");
    HttpAddF   ("  <input type='text' style='width:6.5em;' id='ajax-name-%d' onchange='AjaxSendNameValue(\"lnam%d\", this.value)'>\r\n", index, index);
    HttpAddF   ("  <input type='text' style='width:4.5em;' id='ajax-room-%d' onchange='AjaxSendNameValue(\"lroo%d\", this.value)'>\r\n", index, index);
    HttpAddF   ("  <div style='margin-left:0.1em;' class='led' id='ajax-led-%d' dir='ltr'></div>\r\n", index);
    addButton("lswi", index, "1", "1");
    addButton("lswi", index, "0", "0");
    //HttpAddF   ("  <code style='width:5em; text-align:right;' id='ajax-mac-%d'></code>\r\n", index);
    HttpAddF   ("  <div style='width:2em; text-align:right;' id='ajax-age-%d'></div>\r\n", index);
    HttpAddF   ("  <div style='width:2em; text-align:right;' id='ajax-signal-%d'></div>\r\n", index);
    HttpAddF   ("  <div style='width:2em; text-align:right;' id='ajax-dimmer-%d'></div>\r\n", index);
    addButton("ldel", index, "1", "x");
    addButton("lmov", index, "up", "^");
    addButton("lmov", index, "down", "v");
    HttpAddText("</div>\r\n");
}

static void addSchedule(int index)
{
    HttpAddText("<div style='display:flex; align-items:center;'>\r\n");
    HttpAddF   ("  <input type='text' style='width:4.5em;' id='sched-name-%d' onchange='AjaxSendNameValue(\"snam%d\", this.value)'>\r\n", index, index);
    HttpAddF   ("  <input type='text' style='width:3.2em;' id='sched-on-%d' onchange='AjaxSendNameValue(\"s-on%d\", this.value)'>\r\n", index, index);
    HttpAddF   ("  <input type='text' style='width:3.2em;' id='sched-off-%d' onchange='AjaxSendNameValue(\"soff%d\", this.value)'>\r\n", index, index);
    HttpAddF   ("  <div style='width:3.2em; text-align:right;' id='act-on-%d'></div>\r\n", index);
    HttpAddF   ("  <div style='width:3.2em; text-align:right;' id='act-off-%d'></div>\r\n", index);
    //HttpAddF   ("  <div style='width:3.2em; text-align:right;' id='duration-%d'></div>\r\n", index);
    addButton("sswi", index, "1", "1");
    addButton("sswi", index, "0", "0");
    addButton("sdel", index, "1", "x");
    addButton("smov", index, "up", "^");
    addButton("smov", index, "down", "v");
    HttpAddText("</div>\r\n");
}

void WebWizHtml()
{
    HttpOk("text/html; charset=UTF-8", "no-cache", NULL, NULL);
    WebAddHeader("Wiz", "settings.css", "wiz.js");
    WebAddNav(WIZ_PAGE);
    WebAddH1("Wiz");
    
    WebAddH2("Trace");
    WebAddAjaxInputToggle("Wiz trace", "ajax-wiz-trace", "wiztrace");
    
    WebAddH2("Lights");
    for (int i = 0; i < 15; i++) addLight(i);
    WebAddAjaxButton("Switch all on" , "turn-all-on" );
    WebAddAjaxButton("Switch all off", "turn-all-off");
    
    WebAddH2("Local times");
    WebAddAjaxLabelled("Local (L)",   "ajax-now-local");
    WebAddAjaxLabelled("Sunrise (R)", "ajax-sun-rise" );
    WebAddAjaxLabelled("Sunset (S)",  "ajax-sun-set"  );
    
    WebAddH2("Schedule");
    for (int i = 0; i < 5; i++) addSchedule(i);
    
    WebAddEnd();
}
