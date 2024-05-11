#include "web/web-add.h"
#include "web-nav-this.h"

void WebNavThis(int page)
{
    WebAddNavItem(page ==     HOME_PAGE, "/",         "Home"    );
    WebAddNavItem(page ==   SYSTEM_PAGE, "/system",   "System"  );
    WebAddNavItem(page == ONE_WIRE_PAGE, "/1wire",    "1-Wire"  );
    WebAddNavItem(page ==      WIZ_PAGE, "/wiz",      "Wiz"     );
}
