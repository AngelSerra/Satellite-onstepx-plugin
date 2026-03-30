#include "SatTimeUtils.h"
#include "Sat.h"

static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static const int daysPerMonth[2][12] = {
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static unsigned long utcToTimestamp(int year, int month, int day, int hour, int minute, int second) {
    if (year < 1970 || year > 2038) return 0;
    if (month < 1 || month > 12) return 0;
    if (day < 1) return 0;
    
    unsigned long days = 0;
    for (int y = 1970; y < year; y++) {
        days += isLeapYear(y) ? 366 : 365;
    }
    
    int leap = isLeapYear(year) ? 1 : 0;
    for (int m = 1; m < month; m++) {
        days += daysPerMonth[leap][m - 1];
    }
    
    days += (day - 1);
    
    unsigned long timestamp = days * 86400UL;
    timestamp += (hour * 3600UL);
    timestamp += (minute * 60UL);
    timestamp += second;
    
    return timestamp;
}

unsigned long getUnixTimestampFromState() {
    char reply[80] = "";
    int month, day, year;
    int hour, minute, second;
    
    if (onStep.command(":GC#", reply)) {
        if (sscanf(reply, "%d/%d/%d", &month, &day, &year) == 3) {
            year += 2000;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
    
    if (onStep.command(":GL#", reply)) {
        if (sscanf(reply, "%d:%d:%d", &hour, &minute, &second) != 3) {
            return 0;
        }
    } else {
        return 0;
    }
    
    unsigned long mountTimestamp = utcToTimestamp(year, month, day, hour, minute, second);
    return mountTimestamp - 3600;
}

void getUTCFromUnix(unsigned long unixTime, int &year, int &month, int &day,
                    int &hour, int &minute, int &second) {
    unsigned long timestamp = unixTime;
    
    unsigned long days = timestamp / 86400UL;
    unsigned long secondsInDay = timestamp % 86400UL;
    
    hour = secondsInDay / 3600;
    minute = (secondsInDay % 3600) / 60;
    second = secondsInDay % 60;
    
    year = 1970;
    while (days >= (unsigned long)(isLeapYear(year) ? 366 : 365)) {
        days -= (isLeapYear(year) ? 366 : 365);
        year++;
    }
    
    int leap = isLeapYear(year) ? 1 : 0;
    month = 1;
    while (month <= 12 && days >= (unsigned long)daysPerMonth[leap][month - 1]) {
        days -= daysPerMonth[leap][month - 1];
        month++;
    }
    
    day = days + 1;
}

void getLocalTimeFromUnix(unsigned long unixTime, int &year, int &month, int &day,
                          int &hour, int &minute, int &second) {
    unsigned long localTimestamp = unixTime + 3600;
    getUTCFromUnix(localTimestamp, year, month, day, hour, minute, second);
}

double getJulianDate(unsigned long unixTime) {
    return 2440587.5 + (double)unixTime / 86400.0;
}

double getLocalSiderealTime(double longitude, unsigned long unixTime) {
    double jd = getJulianDate(unixTime);
    double d = jd - 2451545.0;
    double gmst = 18.697374558 + 24.06570982441908 * d;
    gmst = fmod(gmst, 24.0);
    if (gmst < 0) gmst += 24.0;
    double lst = gmst + longitude / 15.0;
    lst = fmod(lst, 24.0);
    if (lst < 0) lst += 24.0;
    return lst;
}