#include "rsa/rsa.h"
#include "tls/tls.h"
#include "pki/ser-cer.h"
#include "pki/pri-key.h"

void CryptoInit()
{
        RsaInit();
        TlsInit();
     SerCerInit();
     PriKeyInit();
}
void CryptoMain()
{
        RsaMain();
        TlsMain();
}