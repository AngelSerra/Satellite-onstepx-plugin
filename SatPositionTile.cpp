// pages/sat/SatPositionTile.cpp
#include "SatPositionTile.h"
#include "SatTracker.h"
#include "SatLocationTile.h"
#include "SatTimeUtils.h"
#include "../KeyValue.h"
#include "../htmlHeaders.h"
#include "../htmlMessages.h"

void satPositionTile(String &data)
{
    char temp[240];

    snprintf_P(temp, sizeof(temp), html_tile_text_beg, "22em", "34em", "Posición del Satélite");
    data.concat(temp);
    data.concat(F("<br /><hr>"));

    data.concat(F(
        "<style>"
        ".sat-data-table { width: 100%; border-collapse: collapse; font-size: 11px; }"
        ".sat-data-table tr:nth-child(even) { background-color: " COLOR_LIGHT_BACKGROUND "; }"
        ".sat-data-table tr:nth-child(odd) { background-color: " COLOR_BACKGROUND "; }"
        ".sat-data-table td { padding: 4px 6px; border-bottom: 1px solid " COLOR_BORDER "; }"
        ".sat-data-table td:first-child { width: 120px; font-weight: bold; color: " COLOR_FOREGROUND "; }"
        ".sat-data-table td:last-child { font-family: monospace; color: " COLOR_CONTROL "; font-weight: bold; }"
        ".sat-title { text-align: center; font-size: 1.2em; font-weight: bold; margin: 8px 0; color: " COLOR_CONTROL "; }"
        ".sat-status { text-align: center; margin: 5px 0 12px 0; padding: 4px; border-radius: 4px; font-size: 11px; }"
        ".sat-status-below { background-color: #330000; color: " COLOR_ALERT "; }"
        ".sat-status-low { background-color: #332200; color: #ffaa66; }"
        ".sat-status-visible { background-color: #003300; color: #88ff88; }"
        ".next-pass-warning { color: " COLOR_ALERT "; font-weight: bold; }"
        ".next-pass-normal { color: " COLOR_CONTROL "; }"
        ".settings-btn { background-color: " COLOR_CONTROL_BACKGROUND "; color: " COLOR_FOREGROUND "; cursor: pointer; padding: 6px 12px; width: auto; border: 1px solid " COLOR_BORDER "; text-align: left; outline: none; font-size: 12px; margin-top: 12px; border-radius: 4px; display: inline-block; }"
        ".active, .settings-btn:hover { background-color: " COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND "; color: " COLOR_COLLAPSIBLE "; }"
        ".settings-content { padding: 12px; display: none; overflow: hidden; background-color: " COLOR_COLLAPSIBLE_CONTENT_BACKGROUND "; margin-top: 5px; border-radius: 4px; }"
        ".tle-input { background-color: " COLOR_BACKGROUND "; border: 1px solid " COLOR_BORDER "; color: " COLOR_FOREGROUND "; padding: 5px; width: 95%; margin: 5px 0; font-family: monospace; font-size: 10px; }"
        ".norad-btn { background-color: " COLOR_CONTROL "; color: white; border: none; padding: 4px 8px; margin: 2px; border-radius: 3px; cursor: pointer; font-size: 10px; }"
        ".norad-btn:hover { background-color: #902020; }"
        "</style>"

        "<div class='sat-title' id='sat_name_display'>---</div>"
        "<div class='sat-status' id='sat_status_display'>---</div>"

        "<table class='sat-data-table'>"
        "<tr><td>NORAD ID:</td><td id='norad_id'>---</td></tr>"
        "<tr><td>Local Time:</td><td id='local_time'>---</td></tr>"
        "<tr><td>UTC:</td><td id='sat_utc'>---</td></tr>"
        "<tr><td>Latitude:</td><td id='gp_lat'>---</td></tr>"
        "<tr><td>Longitude:</td><td id='gp_lon'>---</td></tr>"
        "<tr><td>Altitude:</td><td id='sat_alt'>--- km</td></tr>"
        "<tr><td>Speed:</td><td id='sat_vel'>--- km/s</td></tr>"
        "<tr><td>Azimuth:</td><td id='sat_az'>---°</td></tr>"
        "<tr><td>Elevation:</td><td id='sat_el'>---°</td></tr>"
        "<tr><td>RA:</td><td id='sat_ra'>---</td></tr>"
        "<tr><td>DEC:</td><td id='sat_dec'>---</td></tr>"
        "<tr><td>LST:</td><td id='lst'>---</td></tr>"
        "<tr><td>Period:</td><td id='period'>--- min</td></tr>"
        "<tr><td>Next Pass:</td><td id='next_pass'>---</td></tr>"
        "<tr><td>Max Elev:</td><td id='max_el'>---°</td></tr>"
        "</table>"

        "<div style='text-align: left; margin-top: 10px;'>"
        "<button type='button' class='settings-btn' onclick='toggleSettings()'>Ajustes...</button>"
        "</div>"

        "<div id='settingsContent' class='settings-content'>"
        "  <div style='margin-bottom: 15px;'>"
        "    <b>Obtener de Celestrak</b><br />"
        "    <input type='text' id='norad_id_input' placeholder='NORAD ID (ej: 25544)' class='tle-input' style='width: 150px;'>"
        "    <button onclick='loadByNorad()' class='norad-btn'>Cargar</button>"
        "  </div>"
        "  <hr>"
        "  <div style='margin-bottom: 15px;'>"
        "    <b>Archivo TLE</b><br />"
        "    <input type='file' id='tle_file' accept='.txt' style='width: 180px; font-size: 10px;'>"
        "    <button onclick='loadFromFile()' class='norad-btn'>Cargar Archivo</button>"
        "  </div>"
        "  <div id='load_status' style='margin-top: 8px; padding: 4px; background: #333; color: #ff0; font-size: 10px; text-align: center;'></div>"
        "  <hr>"
        "  <div style='font-size: 10px; text-align: left;'>"
        "    <b>NORAD ID comunes:</b><br />"
        "    <button onclick='loadNorad(25544)' class='norad-btn'>ISS: 25544</button> "
        "    <button onclick='loadNorad(25338)' class='norad-btn'>NOAA 15</button> "
        "    <button onclick='loadNorad(28654)' class='norad-btn'>NOAA 18</button><br />"
        "    <button onclick='loadNorad(33591)' class='norad-btn'>NOAA 19</button> "
        "    <button onclick='loadNorad(25730)' class='norad-btn'>Metop-A</button> "
        "    <button onclick='loadNorad(38771)' class='norad-btn'>Metop-B</button>"
        "  </div>"
        "</div>"

        "<script>"
        "function toggleSettings(){var c=document.getElementById('settingsContent');c.style.display=(c.style.display==='block')?'none':'block';}"
        "function loadNorad(n){document.getElementById('norad_id_input').value=n;loadByNorad();}"
        "async function loadByNorad(){var n=document.getElementById('norad_id_input').value.trim();if(!n){document.getElementById('load_status').innerHTML=' Introduzca NORAD ID';return;}document.getElementById('load_status').innerHTML=' Descargando...';try{var r=await fetch('https://celestrak.com/NORAD/elements/gp.php?CATNR='+n);var t=await r.text();var L=t.split('\\n');if(L.length>=3&&L[1].length>10)sendTLE(L[0].trim(),L[1].trim(),L[2].trim());else document.getElementById('load_status').innerHTML=' Satélite no encontrado';}catch(e){document.getElementById('load_status').innerHTML=' Error: '+e;}}"
        "function loadFromFile(){var f=document.getElementById('tle_file').files[0];if(!f){document.getElementById('load_status').innerHTML=' Seleccione un archivo';return;}var R=new FileReader();R.onload=function(e){var L=e.target.result.split('\\n');if(L.length>=3)sendTLE(L[0].trim(),L[1].trim(),L[2].trim());else document.getElementById('load_status').innerHTML=' Archivo inválido';};R.readAsText(f);}"
        "async function sendTLE(n,l1,l2){document.getElementById('load_status').innerHTML=' Cargando satélite...';var F=new URLSearchParams();F.append('tle_load','1');F.append('name',n);F.append('tle1',l1);F.append('tle2',l2);try{var r=await fetch('/sat-ajax.txt',{method:'POST',body:F});var m=await r.text();document.getElementById('load_status').innerHTML=m;if(m.includes('Satellite loaded'))setTimeout(()=>location.reload(),1500);}catch(e){document.getElementById('load_status').innerHTML=' Error: '+e;}}"
        "</script>"
    ));

    data.concat(FPSTR(html_tile_end));
}

void satPositionTileAjax(String &data)
{
    if (!satTracker.hasSatellite()) {
        data.concat("sat_name_display|---\n");
        data.concat("sat_status_display|---\n");
        data.concat("norad_id|---\n");
        data.concat("local_time|---\n");
        data.concat("sat_utc|---\n");
        data.concat("gp_lat|---\n");
        data.concat("gp_lon|---\n");
        data.concat("sat_alt|---\n");
        data.concat("sat_vel|---\n");
        data.concat("sat_az|---\n");
        data.concat("sat_el|---\n");
        data.concat("sat_ra|---\n");
        data.concat("sat_dec|---\n");
        data.concat("lst|---\n");
        data.concat("period|---\n");
        data.concat("next_pass|---\n");
        data.concat("max_el|---\n");
        return;
    }

    double lat = state.latitude;
    double lon = getRealLongitude();
    unsigned long now = getUnixTimestampFromState();   // <-- UTC real

    SatPosition pos = satTracker.computePosition(lat, lon, 0, now);

    // Nombre
    data.concat("sat_name_display|" + String(satTracker.getSatelliteName()) + "\n");

    // Estado
    String statusClass, statusText;
    if (pos.elevation > 10) { statusClass="sat-status-visible"; statusText="VISIBLE"; }
    else if (pos.elevation > 0) { statusClass="sat-status-low"; statusText="LOW HORIZON"; }
    else { statusClass="sat-status-below"; statusText="BELOW HORIZON"; }
    data.concat("sat_status_display|<div class='" + statusClass + "'>" + statusText + "</div>\n");

    // NORAD → solo nombre
    data.concat("norad_id|" + String(satTracker.getSatelliteName()) + "\n");

    // ======= HORA LOCAL REAL =======
    int y, m, d, hh, mm, ss;
    getLocalTimeFromUnix(now, y, m, d, hh, mm, ss);
    char localBuf[32];
    snprintf(localBuf, sizeof(localBuf), "%04d/%02d/%02d %02d:%02d:%02d", y, m, d, hh, mm, ss);
    data.concat("local_time|" + String(localBuf) + "\n");

    // ======= UTC REAL =======
    getUTCFromUnix(now, y, m, d, hh, mm, ss);
    char utcBuf[32];
    snprintf(utcBuf, sizeof(utcBuf), "%04d/%02d/%02d %02d:%02d:%02d", y, m, d, hh, mm, ss);
    data.concat("sat_utc|" + String(utcBuf) + "\n");

    // Datos básicos
    data.concat("gp_lat|" + String(pos.lat, 6) + "°\n");
    data.concat("gp_lon|" + String(pos.lon, 6) + "°\n");
    data.concat("sat_alt|" + String(pos.altitude, 2) + " km\n");
    data.concat("sat_vel|" + String(pos.velocity, 3) + " km/s\n");
    data.concat("sat_az|" + String(pos.azimuth, 2) + "°\n");
    data.concat("sat_el|" + String(pos.elevation, 2) + "°\n");

    // RA/DEC
    int ra_h = (int)pos.ra;
    int ra_m = (int)((pos.ra - ra_h) * 60);
    int ra_s = (int)(((pos.ra - ra_h) * 60 - ra_m) * 60);
    data.concat("sat_ra|" + String(ra_h) + "h " + String(ra_m) + "m " + String(ra_s) + "s\n");

    double dec_abs = fabs(pos.dec);
    int dec_d = (int)dec_abs;
    int dec_m = (int)((dec_abs - dec_d) * 60);
    int dec_s = (int)(((dec_abs - dec_d) * 60 - dec_m) * 60);
    String dec_str = (pos.dec < 0 ? "-" : "") + String(dec_d) + "° " + String(dec_m) + "' " + String(dec_s) + "\"";
    data.concat("sat_dec|" + dec_str + "\n");

    // LST
    String lst_str="---";
    if (pos.lst > 0 && pos.lst < 24) {
        int lst_h=(int)pos.lst;
        int lst_m=(int)((pos.lst - lst_h) * 60);
        int lst_s=(int)(((pos.lst - lst_h) * 60 - lst_m) * 60);
        lst_str = String(lst_h)+"h "+String(lst_m)+"m "+String(lst_s)+"s";
    }
    data.concat("lst|" + lst_str + "\n");

    // Periodo
    if (pos.period > 0) data.concat("period|" + String(pos.period, 0) + " min\n");
    else data.concat("period|---\n");

    // --- Cálculo preciso del próximo pase ---
    int nextPassStart = -1;
    double nextPassMaxEl = -90;
    bool inPass = false;

    for (int t = 1; t <= 1440; t++) {
        SatPosition future = satTracker.computePosition(lat, lon, 0, now + t * 60);
        if (!future.valid) continue;

        double el = future.elevation;

        if (!inPass && el > 0) {
            inPass = true;
            nextPassStart = t;
            nextPassMaxEl = el;
        }
        else if (inPass) {
            if (el > nextPassMaxEl) nextPassMaxEl = el;
            if (el <= 0) break;
        }
    }

    if (nextPassStart > 0) {
        int mins = nextPassStart;
        int hours = mins / 60;
        int rem = mins % 60;

        String timeStr;
        if (hours > 0) timeStr = String(hours) + "h " + String(rem) + "m";
        else timeStr = String(mins) + " min";

        data.concat("next_pass|" + timeStr + " (max " + String(nextPassMaxEl, 0) + "°)\n");
    } else {
        data.concat("next_pass|>24 hours\n");
    }

    // Max elev actual
    data.concat("max_el|" + String(pos.elevation, 1) + "°\n");
}
