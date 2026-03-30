#ifndef SAT_TIME_UTILS_H
#define SAT_TIME_UTILS_H

#include <Arduino.h>

unsigned long getUnixTimestampFromState();
void getUTCFromUnix(unsigned long unixTime, int &year, int &month, int &day,
                    int &hour, int &minute, int &second);
void getLocalTimeFromUnix(unsigned long unixTime, int &year, int &month, int &day,
                          int &hour, int &minute, int &second);
double getJulianDate(unsigned long unixTime);
double getLocalSiderealTime(double longitude, unsigned long unixTime);

#endif