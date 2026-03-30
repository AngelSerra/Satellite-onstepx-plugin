// pages/sat/Sat.h
#pragma once

#include "../htmlHeaders.h"
#include "../htmlMessages.h"
#include "../htmlScripts.h"

#include "SatLocationTile.h"
#include "SatPositionTile.h"
#include "SatCatalogTile.h"
#include "SatControlTile.h"

void handleSat();
void satAjaxGet();
void satAjax();
void processSatGet();
void satLoop();