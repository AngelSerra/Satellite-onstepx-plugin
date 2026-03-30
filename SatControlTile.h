// plugins/satellite/SatControlTile.h
#pragma once

#include <Arduino.h>

void satControlTile(String &data);
void satControlTileAjax(String &data);
void satControlTileLoop();

extern bool trackingActive;

// ==================== AJUSTES DE PRECISIÓN ====================
const double MIN_ELEVATION = 5.0;
const double TRACKING_INTERVAL = 1000;
const double SLEW_COOLDOWN = 500;
const double RA_TOLERANCE = 0.005;
const double DEC_TOLERANCE = 0.05;