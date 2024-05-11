//Home script
'use strict';

let   boardTemperature    = '';

function parse()
{
    let lines = Ajax.response.split('\n');
    boardTemperature = Ajax.hexToSignedInt16(lines[0]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-board-html'); if (elem) elem.textContent =  OneWire.DS18B20ToString(boardTemperature);
}

Ajax.server     = '/home-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
