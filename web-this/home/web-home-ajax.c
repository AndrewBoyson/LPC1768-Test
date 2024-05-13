#include  <stdint.h>
#include   <stdio.h>

#include "web/http/http.h"
#include "board/board.h"

void WebHomeAjax()
{
    HttpOk("text/plain; charset=UTF-8", "no-cache", NULL, NULL);
                        
    HttpAddInt16AsHex(BoardGetDS18B20Value()); HttpAddChar('\n');
}

