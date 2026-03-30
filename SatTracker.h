// pages/sat/SatTracker.h
#ifndef SAT_TRACKER_H
#define SAT_TRACKER_H

#include <Arduino.h>
#include "sgp4pred.h"
#include "../Pages.common.h"

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (PI / 180.0)
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0 / PI)
#endif

struct SatPosition {
    double azimuth;
    double elevation;
    double ra;
    double dec;
    double range;
    double velocity;
    double lat, lon;
    double altitude;
    double period;
    double lst;
    bool valid;
};

class SatTracker {
public:
    SatTracker();
    
    bool loadTLE(const char* line1, const char* line2);
    SatPosition computePosition(double observerLat, double observerLon, double observerAlt, 
                                unsigned long timestamp = 0);
    const char* getSatelliteName();
    bool hasSatellite();
    void getCurrentTLE(char* line1, char* line2);
    
private:
    Sgp4* _sat;
    char _name[32];
    char _line1[128];
    char _line2[128];
    bool _valid;
    unsigned long _loadedAt;
    
    double _getJulianDate(unsigned long timestamp);
    double _getGMST(double jd);
    double _getLocalSiderealTime(double lon, unsigned long timestamp);
};

extern SatTracker satTracker;

#endif