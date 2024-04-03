//Boiler script
'use strict';

let dateTime           = '';
let   tankTemperature  = '';
let blrOutTemperature  = '';
let blrRtnTemperature  = '';
let blrAlnTemperature  = '';
let blrDeltaT          = '';
let boilerEnable       = false;
let boilerCall         = false;
let boilerPump         = false;
let fullSpeedSecs      = '';
let tankSetPoint       = '';
let tankHysteresis     = '';
let blrRunOnDeg        = '';
let blrRunOnTime       = '';
let blrPumpSpeed       = '';
let blrPumpPwm         = '';
let pumpSpeedCalling   = '';
let pumpSpeedRunOn     = '';
let blrOutputTarget    = '';
let minSpeed           = '';
let riseAt50           = '';
let riseAt100          = '';

function parse()
{
    let lines = Ajax.response.split('\n');
      tankTemperature = Ajax.hexToSignedInt16(lines[ 0]);
    blrOutTemperature = Ajax.hexToSignedInt16(lines[ 1]);
    blrRtnTemperature = Ajax.hexToSignedInt16(lines[ 2]);
    blrAlnTemperature = Ajax.hexToSignedInt16(lines[ 3]);
    blrDeltaT         = Ajax.hexToSignedInt16(lines[ 4]);
    boilerCall        = Ajax.hexToBit        (lines[ 5], 0);
    boilerPump        = Ajax.hexToBit        (lines[ 5], 1);
    boilerEnable      = Ajax.hexToBit        (lines[ 5], 2);
    fullSpeedSecs     = Ajax.hexToSignedInt16(lines[ 6]);
    tankSetPoint      = Ajax.hexToSignedInt16(lines[ 7]);
    tankHysteresis    = Ajax.hexToSignedInt16(lines[ 8]);
    blrRunOnDeg       = Ajax.hexToSignedInt16(lines[ 9]);
    blrRunOnTime      = Ajax.hexToSignedInt16(lines[10]);
    blrPumpSpeed      = Ajax.hexToSignedInt16(lines[11]);
    blrPumpPwm        = Ajax.hexToSignedInt16(lines[12]);
    pumpSpeedCalling  = Ajax.hexToSignedInt16(lines[13]);
    if (pumpSpeedCalling == -1) pumpSpeedCalling = 'A';
    if (pumpSpeedCalling == -2) pumpSpeedCalling = 'T';
    pumpSpeedRunOn    = Ajax.hexToSignedInt16(lines[14]);
    blrOutputTarget   = Ajax.hexToSignedInt16(lines[15]);
    minSpeed          = Ajax.hexToSignedInt16(lines[16]);
    riseAt50          = Ajax.hexToSignedInt16(lines[17]);
    riseAt100         = Ajax.hexToSignedInt16(lines[18]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-tank-html'      ); if (elem) elem.textContent = OneWire.DS18B20ToString(tankTemperature);
    elem = Ajax.getElementOrNull('ajax-blr-out-html'   ); if (elem) elem.textContent = OneWire.DS18B20ToString(blrOutTemperature);
    elem = Ajax.getElementOrNull('ajax-blr-rtn-html'   ); if (elem) elem.textContent = OneWire.DS18B20ToString(blrRtnTemperature);
    elem = Ajax.getElementOrNull('ajax-blr-aln-html'   ); if (elem) elem.textContent = OneWire.DS18B20ToString(blrAlnTemperature);
    elem = Ajax.getElementOrNull('ajax-blr-rise-html'  ); if (elem) elem.textContent = OneWire.DS18B20ToString(blrDeltaT);
    
    elem = Ajax.getElementOrNull('ajax-blr-pump-speed-html'); if (elem) elem.textContent = blrPumpSpeed;
    elem = Ajax.getElementOrNull('ajax-blr-pump-pwm-html'  ); if (elem) elem.textContent = blrPumpPwm;
    
    elem = Ajax.getElementOrNull('ajax-blr-call-toggle'   ); if (elem) elem.setAttribute('dir', boilerCall   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-blr-pump-toggle'   ); if (elem) elem.setAttribute('dir', boilerPump   ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-blr-enable-toggle' ); if (elem) elem.setAttribute('dir', boilerEnable ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-full-speed-secs'   ); if (elem) elem.value = fullSpeedSecs;
    elem = Ajax.getElementOrNull('ajax-tank-set-point'    ); if (elem) elem.value = tankSetPoint;
    elem = Ajax.getElementOrNull('ajax-tank-hysteresis'   ); if (elem) elem.value = tankHysteresis;
    elem = Ajax.getElementOrNull('ajax-blr-run-on-deg'    ); if (elem) elem.value = OneWire.DS18B20ToString(blrRunOnDeg);
    elem = Ajax.getElementOrNull('ajax-blr-run-on-time'   ); if (elem) elem.value = blrRunOnTime;
    
    elem = Ajax.getElementOrNull('ajax-pump-speed-calling'); if (elem) elem.value = pumpSpeedCalling;
    elem = Ajax.getElementOrNull('ajax-pump-speed-run-on' ); if (elem) elem.value = pumpSpeedRunOn;
    elem = Ajax.getElementOrNull('ajax-blr-output-target' ); if (elem) elem.value = blrOutputTarget;
    
    elem = Ajax.getElementOrNull('ajax-min-speed-value'   ); if (elem) elem.value = minSpeed;
    elem = Ajax.getElementOrNull('ajax-min-speed-text'    ); if (elem) elem.textContent = minSpeed;
    elem = Ajax.getElementOrNull('ajax-pump-rise-at-50'   ); if (elem) elem.value = riseAt50;
    elem = Ajax.getElementOrNull('ajax-pump-rise-at-100'  ); if (elem) elem.value = OneWire.DS18B20ToString(riseAt100);
}

Ajax.server     = '/boiler-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
