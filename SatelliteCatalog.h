#pragma once

#include <Arduino.h>

#define MAX_SATELLITES 20
#define MAX_TLE_LINE 128

struct SatelliteEntry {
    char name[32];
    char line1[MAX_TLE_LINE];
    char line2[MAX_TLE_LINE];
    bool valid;
};

class SatelliteCatalog {
public:
    SatelliteCatalog();
    void loadFromNV();
    void saveToNV();
    bool addSatellite(const char* name, const char* line1, const char* line2);
    bool removeSatellite(int index);
    SatelliteEntry* getSatellite(int index);
    int getCount();
    void clear();
    
private:
    SatelliteEntry _satellites[MAX_SATELLITES];
    int _count;
};

extern SatelliteCatalog satCatalog;