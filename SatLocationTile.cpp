// plugins/satellite/SatLocationTile.cpp
#include "SatLocationTile.h"
#include "Sat.h"
#include "SatTimeUtils.h"

static double cachedLongitude = NAN;
static unsigned long lastLongitudeUpdate = 0;

double getRealLongitude() {
    if (millis() - lastLongitudeUpdate < 60000 && !isnan(cachedLongitude)) {
        return cachedLongitude;
    }
    
    char reply[80] = "";
    double lon = 0;
    
    if (status.getVersionMajor() > 3) {
        if (onStep.command(":GgH#", reply)) {
            int sign = 1;
            int idx = 0;
            if (reply[0] == '-') { sign = -1; idx = 1; }
            else if (reply[0] == '+') { idx = 1; }
            
            int degrees = 0, minutes = 0, seconds = 0;
            while (reply[idx] >= '0' && reply[idx] <= '9') {
                degrees = degrees * 10 + (reply[idx] - '0');
                idx++;
            }
            if (reply[idx] == '*') idx++;
            while (reply[idx] >= '0' && reply[idx] <= '9') {
                minutes = minutes * 10 + (reply[idx] - '0');
                idx++;
            }
            if (reply[idx] == ':') idx++;
            while (reply[idx] >= '0' && reply[idx] <= '9') {
                seconds = seconds * 10 + (reply[idx] - '0');
                idx++;
            }
            lon = sign * (degrees + minutes / 60.0 + seconds / 3600.0);
            cachedLongitude = lon;
            lastLongitudeUpdate = millis();
        }
    }
    return cachedLongitude;
}

void satLocationTile(String &data)
{
    char temp[240];

    snprintf_P(temp, sizeof(temp), html_tile_text_beg, "22em", "11em", L_OBSERVER_LOCATION);
    data.concat(temp);
    data.concat(F("<br /><hr>"));

    data.concat(F(
        "<div style='text-align: left;'>"
        "<span style='display: inline-block; width: 80px;'>" L_LATITUDE ":</span>"
        "<span id='obs_lat' class='c'>---</span><span> &deg;</span><br />"
        "<span style='display: inline-block; width: 80px;'>" L_LONGITUDE ":</span>"
        "<span id='obs_lon' class='c'>---</span><span> &deg;</span><br />"
        "<span style='display: inline-block; width: 80px;'>UTC:</span>"
        "<span id='obs_utc' class='c'>---</span><br />"
        "<span style='display: inline-block; width: 80px;'>LOCAL:</span>"
        "<span id='obs_local' class='c'>---</span><br />"
        "</div>"
    ));

    data.concat(FPSTR(html_tile_end));
}

void satLocationTileAjax(String &data)
{
    double lat = state.latitude;
    double lon = getRealLongitude();
    unsigned long utcNow = getUnixTimestampFromState();
    
    data.concat("obs_lat|" + String(lat, 6) + "\n");
    data.concat("obs_lon|" + String(lon, 6) + "\n");
    
    if (utcNow > 0) {
        int y,m,d,h,min,s;
        
        getUTCFromUnix(utcNow, y,m,d,h,min,s);
        char utcBuf[32];
        snprintf(utcBuf, sizeof(utcBuf), "%04d/%02d/%02d %02d:%02d:%02d", y,m,d,h,min,s);
        
        getLocalTimeFromUnix(utcNow, y,m,d,h,min,s);
        char localBuf[32];
        snprintf(localBuf, sizeof(localBuf), "%04d/%02d/%02d %02d:%02d:%02d", y,m,d,h,min,s);
        
        data.concat("obs_utc|" + String(utcBuf) + "\n");
        data.concat("obs_local|" + String(localBuf) + "\n");
    } else {
        data.concat("obs_utc|---\n");
        data.concat("obs_local|---\n");
    }
}