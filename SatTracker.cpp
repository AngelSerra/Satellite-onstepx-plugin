#include "SatTracker.h"
#include "SatLocationTile.h"
#include "SatTimeUtils.h"
#include "../website/pages/Pages.common.h"
#include <math.h>
#include <string.h>

SatTracker::SatTracker() {
    _sat = NULL;
    _valid = false;
    memset(_name, 0, sizeof(_name));
    memset(_line1, 0, sizeof(_line1));
    memset(_line2, 0, sizeof(_line2));
    _loadedAt = 0;
}

double SatTracker::_getJulianDate(unsigned long timestamp) {
    return 2440587.5 + (timestamp / 86400.0);
}

double SatTracker::_getGMST(double jd) {
    double T = (jd - 2451545.0) / 36525.0;
    double gmst = 280.46061837 + 360.98564736629 * (jd - 2451545.0) + 
                  0.000387933 * T * T - T * T * T / 38710000.0;
    gmst = fmod(gmst, 360.0);
    if (gmst < 0) gmst += 360.0;
    return gmst * DEG_TO_RAD;
}

double SatTracker::_getLocalSiderealTime(double lon, unsigned long timestamp) {
    double jd = _getJulianDate(timestamp);
    double gmst = _getGMST(jd);
    double lst = gmst + lon * DEG_TO_RAD;
    lst = fmod(lst, 2.0 * PI);
    if (lst < 0) lst += 2.0 * PI;
    return lst * RAD_TO_DEG / 15.0;
}

bool SatTracker::loadTLE(const char* line1, const char* line2) {
    
    if (strlen(line1) < 69 || strlen(line2) < 69) {
        return false;
    }
    
    strncpy(_line1, line1, sizeof(_line1) - 1);
    strncpy(_line2, line2, sizeof(_line2) - 1);
    _line1[sizeof(_line1) - 1] = '\0';
    _line2[sizeof(_line2) - 1] = '\0';
    
    char noradId[7];
    strncpy(noradId, line1 + 2, 5);
    noradId[5] = '\0';
    
    if (strcmp(noradId, "25544") == 0) {
        strcpy(_name, "ISS (ZARYA)");
    } else if (strcmp(noradId, "25338") == 0) {
        strcpy(_name, "NOAA 15");
    } else {
        sprintf(_name, "SAT-%s", noradId);
    }
    
    if (_sat != NULL) {
        delete _sat;
        _sat = NULL;
    }
    
    _sat = new Sgp4();
    if (_sat == NULL) {
        return false;
    }
    
    bool initOk = _sat->init(_name, (char*)line1, (char*)line2);
    
    if (initOk) {

        double lat = state.latitude;
        double lon = getRealLongitude();
        _sat->site(lat, lon, 0);

    }
    
    _valid = initOk;
    _loadedAt = millis();
    
    return initOk;
}

SatPosition SatTracker::computePosition(double observerLat, double observerLon, double observerAlt,
                                         unsigned long timestamp) {
    SatPosition pos;
    pos.valid = false;
    
    if (!_valid || _sat == NULL) {
        return pos;
    }
	unsigned long now = timestamp;
    if (now == 0) {
        now = getUnixTimestampFromState();
    }
    

    _sat->site(observerLat, observerLon, observerAlt * 1000);
    _sat->findsat(timestamp);
    
    pos.lat = _sat->satLat;
    pos.lon = _sat->satLon;
    pos.altitude = _sat->satAlt;
    pos.azimuth = _sat->satAz;
    pos.elevation = _sat->satEl;
    pos.range = _sat->satDist;
    pos.period = 1440.0 / _sat->revpday;
    

    double jd = _getJulianDate(timestamp); 
    double gmst = _getGMST(jd);
    double lst = gmst + observerLon * DEG_TO_RAD;
    

    double satLonRad = pos.lon * DEG_TO_RAD;
    
    double ha = lst - satLonRad;
    
    pos.dec = pos.lat;
    
    double raRad = lst - ha;
    pos.ra = raRad * RAD_TO_DEG / 15.0;
    if (pos.ra < 0) pos.ra += 24;
    if (pos.ra >= 24) pos.ra -= 24;
    
    double mu = 398600.4418;
    double r = 6378.137 + pos.altitude;
    pos.velocity = sqrt(mu / r);
    
    pos.lst = _getLocalSiderealTime(observerLon, timestamp);
    pos.valid = true;
      
    return pos;
}

const char* SatTracker::getSatelliteName() {
    return _name;
}

bool SatTracker::hasSatellite() {
    return _valid && (_sat != NULL);
}

void SatTracker::getCurrentTLE(char* line1, char* line2) {
    if (line1) strcpy(line1, _line1);
    if (line2) strcpy(line2, _line2);
}

SatTracker satTracker;