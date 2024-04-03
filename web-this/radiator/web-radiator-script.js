//Radiator script
'use strict';

let   hallTemperature    = '';
let   tankTemperature    = '';
let programTimerOutput   = false;
let radiatorMode         = false;
let radiatorOverride     = false;
let radiatorPump         = false;
let hotWaterProtectOn    = false;
let radiatorsOn          = false;
let overrideCancelHour = '';
let overrideCancelMinute = '';
let nightSetPoint        = '';
let frostSetPoint        = '';
let hotWaterProtectTemp  = '';

function parse()
{
    let lines = Ajax.response.split('\n');
    hallTemperature      = Ajax.hexToSignedInt16(lines[0]);
    tankTemperature      = Ajax.hexToSignedInt16(lines[1]);
    programTimerOutput   = Ajax.hexToBit        (lines[2],  0);
    radiatorMode         = Ajax.hexToBit        (lines[2],  1);
    radiatorOverride     = Ajax.hexToBit        (lines[2],  2);
    radiatorPump         = Ajax.hexToBit        (lines[2],  3);
    hotWaterProtectOn    = Ajax.hexToBit        (lines[2],  4);
    radiatorsOn          = Ajax.hexToBit        (lines[2],  5);
    overrideCancelHour   = Ajax.hexToSignedInt8 (lines[3]);
    overrideCancelMinute = Ajax.hexToSignedInt8 (lines[4]);
    nightSetPoint        = Ajax.hexToSignedInt16(lines[5]);
    frostSetPoint        = Ajax.hexToSignedInt16(lines[6]);
    hotWaterProtectTemp  = Ajax.hexToSignedInt8 (lines[7]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-hall-html'             ); if (elem) elem.textContent =  OneWire.DS18B20ToString(hallTemperature);
    elem = Ajax.getElementOrNull('ajax-tank-html'             ); if (elem) elem.textContent =  OneWire.DS18B20ToString(tankTemperature);
    elem = Ajax.getElementOrNull('ajax-program-toggle'        ); if (elem) elem.setAttribute('dir', programTimerOutput ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-mode-toggle'           ); if (elem) elem.setAttribute('dir', radiatorMode       ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-override-toggle'       ); if (elem) elem.setAttribute('dir', radiatorOverride   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-radiator-pump-toggle'  ); if (elem) elem.setAttribute('dir', radiatorPump       ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-hw-prot-toggle'        ); if (elem) elem.setAttribute('dir', hotWaterProtectOn  ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-radiators-on-toggle'   ); if (elem) elem.setAttribute('dir', radiatorsOn        ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-override-cancel-minute'); if (elem)
    {
        elem.value = String(overrideCancelHour*100 + overrideCancelMinute).padStart(4, '0');
    }
    elem = Ajax.getElementOrNull('ajax-night-set-point'      ); if (elem) elem.value = nightSetPoint;
    elem = Ajax.getElementOrNull('ajax-frost-set-point'      ); if (elem) elem.value = frostSetPoint;
    elem = Ajax.getElementOrNull('ajax-hw-prot-temp'         ); if (elem) elem.value = hotWaterProtectTemp;
}

Ajax.server     = '/radiator-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
