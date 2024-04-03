//Program script
'use strict';

let programOut = false;
let program1   = '';
let program2   = '';
let program3   = '';
let monProgram = 0;
let tueProgram = 0;
let wedProgram = 0;
let thuProgram = 0;
let friProgram = 0;
let satProgram = 0;
let sunProgram = 0;
let newDayHour = 0;

function parseDayPrograms(line)
{
    let fields = line.split(',');
    monProgram = parseInt(fields[0]) + 1;
    tueProgram = parseInt(fields[1]) + 1;
    wedProgram = parseInt(fields[2]) + 1;
    thuProgram = parseInt(fields[3]) + 1;
    friProgram = parseInt(fields[4]) + 1;
    satProgram = parseInt(fields[5]) + 1;
    sunProgram = parseInt(fields[6]) + 1;
}
function parse()
{
    let lines = Ajax.response.split('\n');
    programOut =        Ajax.hexToBit(lines[0], 0);
    newDayHour = Ajax.hexToSignedInt8(lines[1]);
    parseDayPrograms                 (lines[2]);
    program1   =                      lines[3];
    program2   =                      lines[4];
    program3   =                      lines[5];
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-program-toggle' ); if (elem) elem.setAttribute('dir', programOut ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-program-1'      ); if (elem) elem.value = program1;
    elem = Ajax.getElementOrNull('ajax-program-2'      ); if (elem) elem.value = program2;
    elem = Ajax.getElementOrNull('ajax-program-3'      ); if (elem) elem.value = program3;
    elem = Ajax.getElementOrNull('ajax-mon-program'    ); if (elem) elem.value = monProgram;
    elem = Ajax.getElementOrNull('ajax-tue-program'    ); if (elem) elem.value = tueProgram;
    elem = Ajax.getElementOrNull('ajax-wed-program'    ); if (elem) elem.value = wedProgram;
    elem = Ajax.getElementOrNull('ajax-thu-program'    ); if (elem) elem.value = thuProgram;
    elem = Ajax.getElementOrNull('ajax-fri-program'    ); if (elem) elem.value = friProgram;
    elem = Ajax.getElementOrNull('ajax-sat-program'    ); if (elem) elem.value = satProgram;
    elem = Ajax.getElementOrNull('ajax-sun-program'    ); if (elem) elem.value = sunProgram;
    elem = Ajax.getElementOrNull('ajax-new-day-hour'   ); if (elem) elem.value = newDayHour;
}

Ajax.server     = '/program-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
