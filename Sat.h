// plugins/satellite/Sat.h
#pragma once

#include <Arduino.h>
#include "SatDefs.h"

// Incluir headers del núcleo de OnStepX (NO declarar extern)
#include "../../../src/lib/wifi/webServer/WebServer.h"
#include "../../../src/telescope/mount/Mount.h"
#include "../../../src/telescope/mount/status/Status.h"
#include "../../../src/telescope/mount/status/State.h"

// Las variables globales ya están declaradas en los headers anteriores
// No necesitas extern WebServer& www, etc.

// Tus funciones
void handleSat();
void satAjax();
void satAjaxGet();
void processSatGet();
void satLoop();

// Tiles
void satLocationTile(String &data);
void satLocationTileAjax(String &data);
void satPositionTile(String &data);
void satPositionTileAjax(String &data);
void satControlTile(String &data);
void satControlTileAjax(String &data);

// Variables externas
extern bool trackingActive;