// pages/sat/SatLocationTile.h
#pragma once

#include <Arduino.h>

void satLocationTile(String &data);
void satLocationTileAjax(String &data);
double getRealLongitude();  // ← Asegurar que está declarada