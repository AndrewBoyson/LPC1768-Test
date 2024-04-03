//Wiz script
'use strict';

let lights   = "";
let wizTrace = false;
let nowlocal = "";
let nowutc   = "";
let sunrise  = "";
let sunset   = "";
let schedule = "";
let macs    = new Array();
let names   = new Array();
let rooms   = new Array();
let ages    = new Array();
let signals = new Array();
let ons     = new Array();
let dimmers = new Array();
let schednames = new Array();
let schedons   = new Array();
let schedoffs  = new Array();
let actons     = new Array();
let actoffs    = new Array();
let durations  = new Array();

function makehm(minutes)
{
    let isNegative = minutes < 0;
    if (isNegative) minutes = -minutes;
    minutes = Math.floor(minutes / 60).toString().padStart(2, '0') + 'h' + (minutes % 60).toString().padStart(2, '0');
    if (isNegative) minutes = '-' + minutes;
    return minutes;
}

function parseGeneral(topic)
{
    let lines = topic.split('\n');
    wizTrace = lines[0] != '0';
}
function addLight(line)
{
    let fields = line.split('\t');
    macs   .push(fields[0].substr(-8));
    names  .push(fields[1]);
    rooms  .push(fields[2]);
    ages   .push(fields[3]);
    signals.push(fields[4]);
    ons    .push(fields[5]);
    dimmers.push(fields[6]);
}
function parseLights(topic)
{
    macs    = new Array();
    names   = new Array();
    rooms   = new Array();
    ages    = new Array();
    signals = new Array();
    ons     = new Array();
    dimmers = new Array();
    topic.split('\n').forEach(addLight);
}
function parseDaylight(topic)
{
    let lines = topic.split('\n');
    nowlocal = Ajax.hexToSignedInt16(lines[0]);
    nowutc   = Ajax.hexToSignedInt16(lines[1]);
    sunrise  = Ajax.hexToSignedInt16(lines[2]);
    sunset   = Ajax.hexToSignedInt16(lines[3]);
}
function addSchedule(line)
{
    let fields = line.split('\t');
    schednames.push(fields[0]);
    schedons  .push(fields[1]);
    schedoffs .push(fields[2]);
    actons    .push(fields[3]);
    actoffs   .push(fields[4]);
    durations .push(fields[5]);
}
function parseSchedules(topic)
{
    schednames = new Array();
    schedons   = new Array();
    schedoffs  = new Array();
    actons     = new Array();
    actoffs    = new Array();
    durations  = new Array();
    topic.split('\n').forEach(addSchedule);
}
function parse()
{
    let topics = Ajax.response.split('\f');
    parseGeneral  (topics[0]);
    lights       = topics[1];
    parseLights   (topics[1]);
    parseDaylight (topics[2]);
    schedule     = topics[3];
    parseSchedules(topics[3]);
}
function display()
{
    let elem;
    elem = Ajax.getElementOrNull('ajax-wiz-lights'  ); if (elem) elem.textContent = lights;
    elem = Ajax.getElementOrNull('ajax-wiz-trace'   ); if (elem) elem.setAttribute('dir', wizTrace ? 'rtl' : 'ltr');
    elem = Ajax.getElementOrNull('ajax-now-local'   ); if (elem) elem.textContent = makehm(nowlocal);
    elem = Ajax.getElementOrNull('ajax-now-utc'     ); if (elem) elem.textContent = makehm(nowutc  );
    elem = Ajax.getElementOrNull('ajax-sun-rise'    ); if (elem) elem.textContent = makehm(sunrise );
    elem = Ajax.getElementOrNull('ajax-sun-set'     ); if (elem) elem.textContent = makehm(sunset  );
    elem = Ajax.getElementOrNull('ajax-wiz-schedule'); if (elem) elem.textContent = schedule;
    for (let i = 0; i < 15; i++)
    {
        elem = Ajax.getElementOrNull('ajax-mac-'    + i); if (elem) elem.textContent =   macs   [i] != null ? macs   [i] : '';
        elem = Ajax.getElementOrNull('ajax-name-'   + i); if (elem) elem.value       =   names  [i] != null ? names  [i] : '';
        elem = Ajax.getElementOrNull('ajax-room-'   + i); if (elem) elem.value       =   rooms  [i] != null ? rooms  [i] : '';
        elem = Ajax.getElementOrNull('ajax-age-'    + i); if (elem) elem.textContent =   ages   [i] != null ? ages   [i] : '';
        elem = Ajax.getElementOrNull('ajax-signal-' + i); if (elem) elem.textContent =   signals[i] != null ? signals[i] : '';
        elem = Ajax.getElementOrNull('ajax-on-'     + i); if (elem) elem.textContent =       ons[i] != null ? ons    [i] : '';
        elem = Ajax.getElementOrNull('ajax-led-'    + i); if (elem) elem.setAttribute('dir', ons[i] == '1' ? 'rtl' : 'ltr')  ;
        elem = Ajax.getElementOrNull('ajax-dimmer-' + i); if (elem) elem.textContent =   dimmers[i] != null ? dimmers[i] : '';
    }
    for (let i = 0; i < 5; i++)
    {
        elem = Ajax.getElementOrNull('sched-name-' + i); if (elem) elem.value       = schednames[i] != null ? schednames[i] : '';
        elem = Ajax.getElementOrNull('sched-on-'   + i); if (elem) elem.value       = schedons  [i] != null ? schedons  [i] : '';
        elem = Ajax.getElementOrNull('sched-off-'  + i); if (elem) elem.value       = schedoffs [i] != null ? schedoffs [i] : '';
        elem = Ajax.getElementOrNull('act-on-'     + i); if (elem) elem.textContent = actons    [i] != null ? actons    [i] : '';
        elem = Ajax.getElementOrNull('act-off-'    + i); if (elem) elem.textContent = actoffs   [i] != null ? actoffs   [i] : '';
        elem = Ajax.getElementOrNull('duration-'   + i); if (elem) elem.textContent = durations [i] != null ? durations [i] : '';
    }
}

Ajax.server     = '/wiz-ajax';
Ajax.onResponse = function() { parse(); display(); };
Ajax.init();
