#include <string.h>
#include <stdlib.h>
#include "web/http/http.h"
#include "heating/program.h"

void WebProgramQuery(char* pQuery)
{
    while (pQuery)
    {
        char* pName;
        char* pValue;
        pQuery = HttpQuerySplit(pQuery, &pName, &pValue);
                
        HttpQueryUnencode(pValue);
        int value = HttpQueryValueAsInt(pValue);
        int program = value;
        if (program < 1) program = 1;
        if (program > 3) program = 3;
        program--;
            
        if (HttpSameStr(pName, "program1"  )) ProgramParse (0, pValue);
        if (HttpSameStr(pName, "program2"  )) ProgramParse (1, pValue);
        if (HttpSameStr(pName, "program3"  )) ProgramParse (2, pValue);
        if (HttpSameStr(pName, "mon"       )) ProgramSetDay(1, program);
        if (HttpSameStr(pName, "tue"       )) ProgramSetDay(2, program);
        if (HttpSameStr(pName, "wed"       )) ProgramSetDay(3, program);
        if (HttpSameStr(pName, "thu"       )) ProgramSetDay(4, program);
        if (HttpSameStr(pName, "fri"       )) ProgramSetDay(5, program);
        if (HttpSameStr(pName, "sat"       )) ProgramSetDay(6, program);
        if (HttpSameStr(pName, "sun"       )) ProgramSetDay(0, program);
        if (HttpSameStr(pName, "newdayhour")) ProgramSetNewDayHour(value);
        
    }
}
