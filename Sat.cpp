// pages/sat/Sat.cpp
#include "SatTimeUtils.h"
#include "Sat.h"
#include "../Page.h"
#include "../Pages.common.h"
#include "../KeyValue.h"
#include "SatTracker.h"
#include "SatLocationTile.h"
#include "SatPositionTile.h"
#include "SatControlTile.h"
#include "SatelliteCatalog.h"

void processSatGet();
void satControlTileLoop();

void handleSat()
{
    char temp[240] = "";

    state.updateMount(true);

    SERIAL_ONSTEP.setTimeout(webTimeout);
    onStep.serialRecvFlush();

    processSatGet();

    www.setContentLength(CONTENT_LENGTH_UNKNOWN);
    www.sendHeader("Cache-Control", "no-cache");
    www.send(200, "text/html", String());

    String data = FPSTR(html_head_begin);
    data.concat(FPSTR(html_main_css_begin));
    www.sendContentAndClear(data);
    data.concat(FPSTR(html_main_css_core));
    data.concat(FPSTR(html_main_css_control));
    www.sendContentAndClear(data);
    data.concat(FPSTR(html_main_css_buttons));
    data.concat(FPSTR(html_main_css_collapse));
    data.concat(FPSTR(html_main_css_end));
    data.concat(FPSTR(html_head_end));
    www.sendContentAndClear(data);

    data.concat(FPSTR(html_body_begin));
    www.sendContentAndClear(data);
    pageHeader(PAGE_SAT);
    data.concat(FPSTR(html_onstep_page_begin));

    if (!status.onStepFound) {
        data.concat(FPSTR(html_bad_comms_message));
        data.concat(FPSTR(html_page_and_body_end));
        www.sendContentAndClear(data);
        www.sendContent("");
        return;
    }

    data.concat(F("<script>var ajaxPage='sat-ajax.txt';</script>\n"));
    www.sendContentAndClear(data);
    data.concat(FPSTR(html_script_ajax));
    www.sendContentAndClear(data);
    snprintf_P(temp, sizeof(temp), html_script_ajax_get, "sat-ajax-get.txt");
    data.concat(temp);
    www.sendContentAndClear(data);

    satLocationTile(data);
    satPositionTile(data);
    satControlTile(data);

    data.concat(F("<br class=\"clear\" />\n"));
    data.concat(FPSTR(html_script_collapsible));
    data.concat(FPSTR(html_page_and_body_end));

    www.sendContentAndClear(data);
    www.sendContent("");
}

void satAjax()
{
    www.setContentLength(CONTENT_LENGTH_UNKNOWN);
    www.sendHeader("Cache-Control", "no-cache");
    www.send(200, "text/plain", String());
    
    String data = "";

    if (www.hasArg("tle_load")) {
        String tle1 = www.arg("tle1");
        String tle2 = www.arg("tle2");
        String name = www.arg("name");
        
        if (satTracker.loadTLE(tle1.c_str(), tle2.c_str())) {
            data = "Satellite loaded: " + name;
        } else {
            data = "Error loading TLE";
        }
        www.sendContentAndClear(data);
        www.sendContent("");
        return;
    }
    
    if (www.hasArg("add_sat")) {
        if (satTracker.hasSatellite()) {
            char tle1[128], tle2[128];
            satTracker.getCurrentTLE(tle1, tle2);
            String name = www.arg("sat_name");
            if (name.length() == 0) name = satTracker.getSatelliteName();
            if (satCatalog.addSatellite(name.c_str(), tle1, tle2)) {
                data = "Satellite saved: " + name;
            } else {
                data = " Catalog full";
            }
        } else {
            data = " No satellite loaded";
        }
        www.sendContentAndClear(data);
        www.sendContent("");
        return;
    }
    
    if (www.hasArg("load_sat")) {
        int idx = www.arg("load_sat").toInt();
        SatelliteEntry* sat = satCatalog.getSatellite(idx);
        if (sat && sat->valid) {
            if (satTracker.loadTLE(sat->line1, sat->line2)) {
                data = " Loaded: " + String(sat->name);
            } else {
                data = " Error loading TLE";
            }
        } else {
            data = " Satellite not found";
        }
        www.sendContentAndClear(data);
        www.sendContent("");
        return;
    }
    
    if (www.hasArg("delete_sat")) {
        int idx = www.arg("delete_sat").toInt();
        if (satCatalog.removeSatellite(idx)) {
            data = " Satellite deleted";
        } else {
            data = " Error deleting";
        }
        www.sendContentAndClear(data);
        www.sendContent("");
        return;
    }
    
    if (www.hasArg("clear_catalog")) {
        satCatalog.clear();
        data = " Catalog cleared";
        www.sendContentAndClear(data);
        www.sendContent("");
        return;
    }
    
    if (www.hasArg("cmd")) {
        String cmd = www.arg("cmd");
        
        if (cmd == "goto") {
            double lat = state.latitude;
            double lon = getRealLongitude();
            unsigned long now = getUnixTimestampFromState();
    
            if (now == 0) {
               data = "ERROR: Cannot get current time";
               www.sendContentAndClear(data);
               www.sendContent("");
               return;
            }
    
        SatPosition pos = satTracker.computePosition(lat, lon, 0, now);
    
        if (pos.valid) {
           if (pos.elevation <= 0) {
               data = "WARNING: Satellite below horizon (EL=" + String(pos.elevation, 1) + "°). Cannot point.";
               www.sendContentAndClear(data);
               www.sendContent("");
               return;
            }
        
        // Formato RA: HH:MM:SS
            int raH = (int)pos.ra;
            int raM = (int)((pos.ra - raH) * 60);
            int raS = (int)(((pos.ra - raH) * 60 - raM) * 60);
        
        // Formato DEC: sDD*MM:SS (nota: usa * no :)
            double decAbs = fabs(pos.dec);
            int decD = (int)decAbs;
            int decM = (int)((decAbs - decD) * 60);
            int decS = (int)(((decAbs - decD) * 60 - decM) * 60);
            char decSign = (pos.dec >= 0) ? '+' : '-';
        
        // Comandos en formato OnStepX
            char cmdRA[30], cmdDec[30];
            sprintf(cmdRA, ":Sr %02d:%02d:%02d#", raH, raM, raS);
            sprintf(cmdDec, ":Sd %c%02d*%02d:%02d#", decSign, decD, decM, decS);
        
        char response[80];
        
        // Enviar RA
        onStep.command(cmdRA, response);
        delay(100);
        
        // Enviar DEC
        onStep.command(cmdDec, response);
        delay(100);
        
        // Iniciar GOTO
        onStep.command(":MS#", response);
        
        data = "GOTO: RA=" + String(raH) + "h" + String(raM) + "m" + String(raS) + "s, DEC=" + String(decSign) + String(decD) + "°" + String(decM) + "'" + String(decS) + "\", EL=" + String(pos.elevation, 1) + "°";
    } else {
        data = "ERROR: Cannot compute satellite position";
    }
    www.sendContentAndClear(data);
    www.sendContent("");
    return;
}
        
        if (cmd == "track_start") {
            trackingActive = true;
            data = "Tracking activated";
            www.sendContentAndClear(data);
            www.sendContent("");
            return;
        }
        
        if (cmd == "track_stop") {
            trackingActive = false;
            data = "Tracking stopped";
            www.sendContentAndClear(data);
            www.sendContent("");
            return;
        }
    }
    
    if (status.onStepFound)
    {
        satLocationTileAjax(data);
        satPositionTileAjax(data);
        satControlTileAjax(data);
    }
    
    www.sendContentAndClear(data);
    www.sendContent("");
    
    state.lastMountPageLoadTime = millis();
}

void satAjaxGet()
{
    www.setContentLength(CONTENT_LENGTH_UNKNOWN);
    www.sendHeader("Cache-Control", "no-cache");
    www.send(200, "text/plain", String());
    
    processSatGet();
    
    www.sendContent("");
}

void processSatGet()
{
    String cmd = www.arg("cmd");
    
    if (cmd == "load_tle") {
        String tle1 = www.arg("tle1");
        String tle2 = www.arg("tle2");
        satTracker.loadTLE(tle1.c_str(), tle2.c_str());
    }
}

void satLoop()
{
    satControlTileLoop();
}