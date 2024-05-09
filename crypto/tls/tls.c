#include <stdbool.h>

#include "tls.h"
#include "tls-connection.h"
#include "web/http/http.h"

bool TlsTrace = false;

void TlsMain()
{
    TlsRequestMain();
}
void TlsInit()
{
}
void TlsReset(int connectionId)
{
    TlsConnectionReset(connectionId);
    HttpReset(connectionId);
}
