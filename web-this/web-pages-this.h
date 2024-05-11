#include <stdint.h>
#include <stdbool.h>

extern void        WebHomeHtml         (void        );
extern void        WebHomeAjax         (void        );
extern void        WebHomeScript       (void        );
extern const char* WebHomeScriptDate                 ;
extern const char* WebHomeScriptTime                 ;

extern void        WebSystemHtml       (void        );
extern void        WebSystemQuery      (char* pQuery);
extern void        WebSystemAjax       (void        );
extern void        WebSystemScript     (void        );
extern const char* WebSystemScriptDate               ;
extern const char* WebSystemScriptTime               ;

extern void        WebOneWireHtml      (void        );
extern void        WebOneWireQuery     (char* pQuery);
extern void        WebOneWireAjax      (void        );
extern void        WebOneWireScript    (void        );
extern const char* WebOneWireScriptDate              ;
extern const char* WebOneWireScriptTime              ;

extern void        WebWizHtml          (void        );
extern void        WebWizQuery         (char* pQuery);
extern void        WebWizAjax          (void        );
extern void        WebWizScript        (void        );
extern const char* WebWizScriptDate                  ;
extern const char* WebWizScriptTime                  ;
