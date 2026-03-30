// plugins/satellite/SatelliteCatalog.cpp
#include "SatControlTile.h"
#include "Sat.h"
#include "SatLocationTile.h"
#include "SatTracker.h"
#include "SatTimeUtils.h"
#include "SatelliteCatalog.h"

bool trackingActive = false;
static unsigned long lastTrackingUpdate = 0;
static unsigned long lastSlewCommand = 0;

void satControlTile(String &data)
{
    char temp[240];

    snprintf_P(temp, sizeof(temp), html_tile_text_beg, "22em", "18em", L_CONTROLS);
    data.concat(temp);
    data.concat(F("<br /><hr>"));

    data.concat(F(
        "<style>"
        ".track-panel { text-align: left; font-family: monospace; font-size: 12px; }"
        ".track-line { margin: 8px 0; padding: 4px; border-bottom: 1px solid " COLOR_BORDER "; }"
        ".track-label { display: inline-block; width: 100px; font-weight: bold; color: " COLOR_FOREGROUND "; }"
        ".track-value { color: " COLOR_CONTROL "; font-weight: bold; }"
        ".ctrl-btn { background-color: " COLOR_CONTROL_BACKGROUND "; border: 1px solid " COLOR_BORDER "; color: " COLOR_FOREGROUND "; padding: 6px 16px; margin: 5px; border-radius: 4px; cursor: pointer; font-weight: bold; width: 100px; }"
        ".ctrl-btn:hover { background-color: " COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND "; color: " COLOR_COLLAPSIBLE "; border-color: " COLOR_CONTROL "; }"
        ".ctrl-btn:disabled { background-color: #331111; cursor: not-allowed; color: #663333; }"
        ".small-btn { background-color: " COLOR_CONTROL_BACKGROUND "; border: 1px solid " COLOR_BORDER "; color: " COLOR_FOREGROUND "; padding: 4px 8px; margin: 2px; border-radius: 3px; cursor: pointer; font-size: 10px; }"
        ".small-btn:hover { background-color: " COLOR_COLLAPSIBLE_ACTIVE_BACKGROUND "; }"
        ".input-field { background-color: " COLOR_BACKGROUND "; border: 1px solid " COLOR_BORDER "; color: " COLOR_FOREGROUND "; padding: 4px; border-radius: 3px; width: 140px; font-size: 11px; }"
        ".warning-msg { color: #ffaa66; font-size: 10px; text-align: center; margin-top: 5px; padding: 4px; background-color: #332200; border-radius: 3px; }"
        "</style>"
        
        "<div class='track-panel'>"
        "<div class='track-line'>"
        "<span class='track-label'>Seguimiento:</span>"
        "<span id='track_status_display' class='track-value'> INACTIVO</span>"
        "<span id='track_progress' style='margin-left: 10px; font-size: 10px;'></span>"
        "</div>"
        
        "<div class='track-line'>"
        "<span class='track-label'>Posición:</span>"
        "<span id='mount_ra' class='track-value'>---</span> "
        "<span id='mount_dec' class='track-value'>---</span>"
        "</div>"
        
        "<div class='track-line'>"
        "<span class='track-label'>Objetivo:</span>"
        "<span id='target_ra' class='track-value'>---</span> "
        "<span id='target_dec' class='track-value'>---</span>"
        "</div>"
        
        "<div class='track-line'>"
        "<span class='track-label'>Error:</span>"
        "<span id='error_ra' class='track-value'>---</span> "
        "<span id='error_dec' class='track-value'>---</span>"
        "</div>"
        
        "<div class='track-line'>"
        "<span class='track-label'>Satélite:</span>"
        "<span id='sat_elevation' class='track-value'>---</span>"
        "<span id='sat_status' class='track-value'></span>"
        "</div>"
        
        "<div id='track_warning' class='warning-msg'></div>"
        
        "<div style='text-align: center; margin-top: 15px;'>"
        "<button onclick='gotoSat()' id='goto_btn' class='ctrl-btn'>APUNTAR</button>"
        "<button id='track_btn' onclick='toggleTracking()' class='ctrl-btn'>SEGUIR</button>"
        "</div>"
        
        "<hr>"
        
        "<div style='text-align: center; margin-top: 8px;'>"
        "<input type='text' id='sat_name' placeholder='Nombre del satélite' class='input-field'>"
        "<button onclick='saveSatellite()' class='small-btn'>Guardar</button>"
        "</div>"
        "</div>"
        
        "<script>"
        "function gotoSat() {"
        "  fetch('/sat-ajax.txt', {method:'POST', body:'cmd=goto'})"
        "    .then(r => r.text())"
        "    .then(msg => {"
        "      if(msg && msg.includes('WARNING')) {"
        "        alert(msg);"
        "      } else if(msg && !msg.includes('ERROR')) {"
        "        alert('Apuntando al satélite\\n' + msg);"
        "      } else {"
        "        alert('Error: ' + msg);"
        "      }"
        "    })"
        "    .catch(err => alert('Error de comunicación'));"
        "}"
        ""
        "function toggleTracking() {"
        "  var btn = document.getElementById('track_btn');"
        "  if(btn.innerHTML == 'SEGUIR') {"
        "    fetch('/sat-ajax.txt', {method:'POST', body:'cmd=track_start'})"
        "      .then(r => r.text())"
        "      .then(msg => {"
        "        btn.innerHTML = 'PARAR';"
        "        document.getElementById('goto_btn').disabled = true;"
        "        document.getElementById('track_status_display').innerHTML = ' ACTIVO';"
        "      });"
        "  } else {"
        "    fetch('/sat-ajax.txt', {method:'POST', body:'cmd=track_stop'})"
        "      .then(r => r.text())"
        "      .then(msg => {"
        "        btn.innerHTML = 'SEGUIR';"
        "        document.getElementById('goto_btn').disabled = false;"
        "        document.getElementById('track_status_display').innerHTML = ' INACTIVO';"
        "      });"
        "  }"
        "}"
        ""
        "function saveSatellite() {"
        "  var name = document.getElementById('sat_name').value;"
        "  fetch('/sat-ajax.txt', {method:'POST', body:'add_sat=1&sat_name=' + encodeURIComponent(name)})"
        "    .then(r => r.text())"
        "    .then(msg => alert(msg));"
        "}"
        "</script>"
    ));

    data.concat(FPSTR(html_tile_end));
}

void satControlTileAjax(String &data)
{
    if (trackingActive) {
        data.concat("track_status_display| ACTIVO\n");
    } else {
        data.concat("track_status_display| INACTIVO\n");
    }
    
    char raReply[20], decReply[20];
    double mountRA = 0, mountDec = 0;
    char mountRAStr[20], mountDecStr[20];
    
    if (onStep.command(":GR#", raReply)) {
        int raH, raM, raS;
        if (sscanf(raReply, "%02d:%02d:%02d", &raH, &raM, &raS) == 3) {
            mountRA = raH + raM/60.0 + raS/3600.0;
            sprintf(mountRAStr, "α %02dh%02dm%02ds", raH, raM, raS);
        } else {
            strcpy(mountRAStr, "---");
        }
    } else {
        strcpy(mountRAStr, "---");
    }
    
    if (onStep.command(":GD#", decReply)) {
        char sign;
        int decD, decM, decS;
        if (sscanf(decReply, "%c%02d*%02d:%02d", &sign, &decD, &decM, &decS) == 4) {
            mountDec = decD + decM/60.0 + decS/3600.0;
            if (sign == '-') mountDec = -mountDec;
            sprintf(mountDecStr, "δ %c%02d°%02d'%02d\"", sign, decD, decM, decS);
        } else {
            strcpy(mountDecStr, "---");
        }
    } else {
        strcpy(mountDecStr, "---");
    }
    
    data.concat("mount_ra|" + String(mountRAStr) + "\n");
    data.concat("mount_dec|" + String(mountDecStr) + "\n");
    
    if (satTracker.hasSatellite()) {
        double lat = state.latitude;
        double lon = getRealLongitude();
        unsigned long now = getUnixTimestampFromState();
        
        if (now == 0) {
            data.concat("target_ra|---\n");
            data.concat("target_dec|---\n");
            data.concat("error_ra|---\n");
            data.concat("error_dec|---\n");
            data.concat("sat_elevation|---\n");
            data.concat("sat_status|---\n");
            data.concat("track_progress|\n");
            data.concat("track_warning|\n");
            return;
        }
        
        SatPosition pos = satTracker.computePosition(lat, lon, 0, now);
        
        if (pos.valid) {
            int raH = (int)pos.ra;
            int raM = (int)((pos.ra - raH) * 60);
            int raS = (int)(((pos.ra - raH) * 60 - raM) * 60);
            char targetRAStr[20];
            sprintf(targetRAStr, "α %02dh%02dm%02ds", raH, raM, raS);
            
            double decAbs = fabs(pos.dec);
            int decD = (int)decAbs;
            int decM = (int)((decAbs - decD) * 60);
            int decS = (int)(((decAbs - decD) * 60 - decM) * 60);
            char targetDecStr[20];
            sprintf(targetDecStr, "δ %c%02d°%02d'%02d\"", (pos.dec >= 0) ? '+' : '-', decD, decM, decS);
            
            data.concat("target_ra|" + String(targetRAStr) + "\n");
            data.concat("target_dec|" + String(targetDecStr) + "\n");
            
            double diffRA = pos.ra - mountRA;
            double diffDec = pos.dec - mountDec;
            
            if (diffRA > 12) diffRA -= 24;
            if (diffRA < -12) diffRA += 24;
            
            char errorRAStr[20], errorDecStr[20];
            sprintf(errorRAStr, "Δα %+05.2fm", diffRA * 60);
            sprintf(errorDecStr, "Δδ %+05.1f'", diffDec * 60);
            
            data.concat("error_ra|" + String(errorRAStr) + "\n");
            data.concat("error_dec|" + String(errorDecStr) + "\n");
            
            char elevStr[20];
            sprintf(elevStr, "EL %+03.0f°", pos.elevation);
            data.concat("sat_elevation|" + String(elevStr) + "\n");
            
            if (trackingActive) {
                data.concat("track_progress| siguiendo\n");
            } else {
                data.concat("track_progress|\n");
            }
            
            if (pos.elevation <= 0) {
                data.concat("track_warning| Satellite below horizon. Cannot track.\n");
                data.concat("sat_status| Bajo horizonte\n");
            } else if (pos.elevation < 10) {
                data.concat("track_warning| Satellite very low (" + String(pos.elevation, 1) + "°). Tracking may be inaccurate.\n");
                data.concat("sat_status| Muy bajo\n");
            } else if (pos.elevation > MIN_ELEVATION) {
                data.concat("track_warning|\n");
                data.concat("sat_status| ✓ Visible\n");
            }
            
            if (pos.elevation <= 0) {
                int nextPass = -1;
                for (int t = 5; t <= 180; t += 5) {
                    SatPosition future = satTracker.computePosition(lat, lon, 0, now + t * 60);
                    if (future.valid && future.elevation > MIN_ELEVATION) {
                        nextPass = t;
                        break;
                    }
                }
                if (nextPass > 0) {
                    data.concat("sat_status| +" + String(nextPass) + " min\n");
                }
            }
        } else {
            data.concat("target_ra|---\n");
            data.concat("target_dec|---\n");
            data.concat("error_ra|---\n");
            data.concat("error_dec|---\n");
            data.concat("sat_elevation|---\n");
            data.concat("sat_status|---\n");
            data.concat("track_progress|\n");
            data.concat("track_warning|\n");
        }
    } else {
        data.concat("target_ra|---\n");
        data.concat("target_dec|---\n");
        data.concat("error_ra|---\n");
        data.concat("error_dec|---\n");
        data.concat("sat_elevation|---\n");
        data.concat("sat_status| Sin satélite\n");
        data.concat("track_progress|\n");
        data.concat("track_warning| No satellite loaded\n");
    }
}

void satControlTileLoop()
{
    if (!trackingActive) return;
    if (!satTracker.hasSatellite()) return;
    
    unsigned long now = millis();
    if (now - lastSlewCommand < SLEW_COOLDOWN) return;
    
    if (now - lastTrackingUpdate >= TRACKING_INTERVAL) {
        lastTrackingUpdate = now;
        
        double lat = state.latitude;
        double lon = getRealLongitude();
        unsigned long utcNow = getUnixTimestampFromState();
        
        if (utcNow == 0) return;
        
        char raReply[20], decReply[20];
        double mountRA = 0, mountDec = 0;
        
        if (onStep.command(":GR#", raReply)) {
            int raH, raM, raS;
            if (sscanf(raReply, "%02d:%02d:%02d", &raH, &raM, &raS) == 3) {
                mountRA = raH + raM/60.0 + raS/3600.0;
            }
        }
        
        if (onStep.command(":GD#", decReply)) {
            char sign;
            int decD, decM, decS;
            if (sscanf(decReply, "%c%02d*%02d:%02d", &sign, &decD, &decM, &decS) == 4) {
                mountDec = decD + decM/60.0 + decS/3600.0;
                if (sign == '-') mountDec = -mountDec;
            }
        }
        
        SatPosition pos = satTracker.computePosition(lat, lon, 0, utcNow);
        
        if (pos.valid && pos.elevation > MIN_ELEVATION) {
            double diffRA = pos.ra - mountRA;
            double diffDec = pos.dec - mountDec;
            
            if (diffRA > 12) diffRA -= 24;
            if (diffRA < -12) diffRA += 24;
            
            if (fabs(diffRA) > RA_TOLERANCE || fabs(diffDec) > DEC_TOLERANCE) {
                int raH = (int)pos.ra;
                int raM = (int)((pos.ra - raH) * 60);
                int raS = (int)(((pos.ra - raH) * 60 - raM) * 60);
                
                double decAbs = fabs(pos.dec);
                int decD = (int)decAbs;
                int decM = (int)((decAbs - decD) * 60);
                int decS = (int)(((decAbs - decD) * 60 - decM) * 60);
                char decSign = (pos.dec >= 0) ? '+' : '-';
                
                char cmdRA[30], cmdDec[30];
                sprintf(cmdRA, ":Sr %02d:%02d:%02d#", raH, raM, raS);
                sprintf(cmdDec, ":Sd %c%02d:%02d:%02d#", decSign, decD, decM, decS);
                
                char response[80];
                onStep.command(cmdRA, response);
                delay(50);
                onStep.command(cmdDec, response);
                delay(50);
                onStep.command(":MS#", response);
                
                lastSlewCommand = now;
            }
        } else if (pos.valid && pos.elevation <= MIN_ELEVATION) {
            trackingActive = false;
        }
    }
}