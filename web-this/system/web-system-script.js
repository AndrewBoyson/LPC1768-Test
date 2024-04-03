//System script
'use strict';

let serverName   = '';
let fileName     = '';
let readInterval = 0;
let writeSize    = 0;
let count        = 0;
let startTime    = null;
let framUsed     = 0;

function parse()
{
    let lines = Ajax.response.split('\n');
    serverName   =                       lines[0];
    fileName     =                       lines[1];
    readInterval = Ajax.hexToSignedInt16(lines[2]);
    writeSize    = Ajax.hexToSignedInt16(lines[3]);
    count        = Ajax.hexToSignedInt16(lines[4]);
    startTime    =     new Date(parseInt(lines[5], 16) * 1000);
    framUsed     = Ajax.hexToSignedInt16(lines[6]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-server-name'  ); if (elem) elem.value = serverName;
    elem = Ajax.getElementOrNull('ajax-file-name'    ); if (elem) elem.value = fileName;
    elem = Ajax.getElementOrNull('ajax-read-interval'); if (elem) elem.value = readInterval;
    elem = Ajax.getElementOrNull('ajax-write-size'   ); if (elem) elem.value = writeSize;
    elem = Ajax.getElementOrNull('ajax-count'        ); if (elem) elem.textContent = count;
    elem = Ajax.getElementOrNull('ajax-start-time'   ); if (elem) elem.textContent = startTime.toISOString().substr(0, 19).split('T').join(' ') + ' UTC';
    elem = Ajax.getElementOrNull('ajax-fram-used'    ); if (elem) elem.textContent = framUsed;
}

Ajax.server     = '/system-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
