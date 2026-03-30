# Satellite Tracker Plugin for OnStepX

A satellite tracking plugin for OnStepX that allows loading TLEs, real-time position calculation, and automatic satellite tracking.

## Features

- **Load satellites** via TLE (Two-Line Element) from file or directly from Celestrak by NORAD ID
- **Precise real-time position calculation** (matches N2YO.com)
- **Complete data display**: Latitude, Longitude, Altitude, Speed, Azimuth, Elevation, RA/DEC
- **Next pass prediction** with maximum elevation calculation
- **Automatic tracking** while satellite is visible
- **GOTO** to point the telescope at the satellite
- **Satellite catalog** with NV storage (up to 20 satellites)
- **Multi-language support** (Spanish, English, German, Chinese)
- **Automatic timezone compensation** for Spanish timezone (UTC+1)

## Requirements

- OnStepX (version 10.x or higher)
- ESP32 (WROOM 32 recommended)
- Sgp4 Library included (Hopperpop/Sgp4-Library)

## Installation

1. Copy the `sat` folder to `plugins/website/pages/`
2. Ensure the structure is:
plugins/satelite/
├── Sat.cpp
├── Sat.h
├── SatControlTile.cpp
├── SatControlTile.h
├── SatLocationTile.cpp
├── SatLocationTile.h
├── SatPositionTile.cpp
├── SatPositionTile.h
├── SatTimeUtils.cpp
├── SatTimeUtils.h
├── SatTracker.cpp
├── SatTracker.h
├── SatelliteCatalog.cpp
└── SatelliteCatalog.h



3. Recompile the OnStepX firmware

## Usage

### Load a satellite

1. Go to the **"Satellites"** tab
2. In the position section, click **"Settings..."**
3. You can:
- Enter a **NORAD ID** (e.g., 25544 for ISS) and press **"Load"**
- Select a **TLE file** (.txt) and press **"Load File"**
- Use the predefined buttons (ISS, NOAA 15, NOAA 18, NOAA 19)

### Point to the satellite

- Press the **"POINT"** button when the satellite is visible (elevation > 0°)
- The telescope will move to the satellite's RA/DEC coordinates

### Automatic tracking

- Press **"TRACK"** to activate continuous tracking
- The telescope will update every 1-2 seconds while the satellite is visible
- Tracking stops automatically when the satellite drops below 10° elevation

### Save to catalog

- Once a satellite is loaded, enter a name and press **"Save"**
- Saved satellites will appear in the selector in the settings section

## Adjustable parameters

In `SatControlTile.h` you can modify:

```cpp
const double MIN_ELEVATION = 5.0;     // Minimum elevation for tracking (degrees)
const double TRACKING_INTERVAL = 1000; // Update interval (ms)
const double RA_TOLERANCE = 0.01;      // RA tolerance (hours)
const double DEC_TOLERANCE = 0.1;      // DEC tolerance (degrees)

Timezone compensation
The plugin assumes the mount has LOCAL time configured as if it were UTC (common in Spain). If your mount is already in UTC, modify in SatTimeUtils.cpp:

return mountTimestamp - 3600;  // Change to -0 if mount is already in UTC

ile structure
File	Description
Sat.cpp	Main page and AJAX handler
Sat.h	Main header
SatControlTile.cpp	Control tile (buttons, tracking, status)
SatLocationTile.cpp	Observer location tile
SatPositionTile.cpp	Satellite position tile
SatTimeUtils.cpp	Time utilities and timezone handling
SatTracker.cpp	Interface with Sgp4 library
SatelliteCatalog.cpp	NV storage for satellites
OnStepX commands used
Command	Function
:GC#	Get UTC date
:GL#	Get UTC time
:GR#	Get current RA
:GD#	Get current DEC
:Sr HH:MM:SS#	Set target RA
:Sd sDD*MM:SS#	Set target DEC
:MS#	Start GOTO
Troubleshooting
Telescope does not move when pressing "POINT"
Check that the mount is not parked (Un-Park on main page)

Verify the mount's date/time is correct

Test the :GU# command from the browser console

Data does not match N2YO
Verify the timezone compensation in SatTimeUtils.cpp

Make sure you are using up-to-date TLEs (download from Celestrak)

Tracking stops too early
Adjust MIN_ELEVATION to a lower value (e.g., 3°)

Increase TRACKING_INTERVAL for less ESP32 load

Compilation error: getRealLongitude not declared
Ensure SatLocationTile.h includes the declaration of double getRealLongitude();

External dependencies
Sgp4-Library (Hopperpop): https://github.com/Hopperpop/Sgp4-Library.git

Credits
Based on the Sgp4 library by David Vallado (2007)

Ported to C++ by Grady Hillhouse and Hopperpop

Plugin developed for OnStepX

License
This plugin is distributed under the same license as OnStepX.

Version notes
v1.0
First functional version

Precise position calculation (matches N2YO)

Automatic tracking and GOTO

Satellite catalog

Multi-language support