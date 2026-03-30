// pages/sat/SatControlTile.h
#pragma once

#include <Arduino.h>

void satControlTile(String &data);
void satControlTileAjax(String &data);
void satControlTileLoop();

extern bool trackingActive;

// ==================== AJUSTES DE PRECISIÓN ====================
// Modifica estos valores según necesites:

// Elevación mínima para considerar el satélite visible (grados)
// 10° es seguro, 5° es más agresivo pero puede tener obstáculos
const double MIN_ELEVATION = 5.0;

// Intervalo de actualización del tracking (milisegundos)
// 1000 = 1 segundo (más preciso pero más carga)
// 2000 = 2 segundos (menos preciso pero más suave)
const double TRACKING_INTERVAL = 1000;

// Tiempo de espera entre comandos GOTO (milisegundos)
// 500 = medio segundo
const double SLEW_COOLDOWN = 500;

// Tolerancia para RA (horas)
// 0.02 horas = 1.2 minutos de error
// 0.01 horas = 36 segundos de error (más preciso)
// 0.005 horas = 18 segundos (muy preciso)
const double RA_TOLERANCE = 0.005;

// Tolerancia para DEC (grados)
// 0.2 grados = 12 minutos de error
// 0.1 grados = 6 minutos de error (más preciso)
// 0.05 grados = 3 minutos (muy preciso)
const double DEC_TOLERANCE = 0.05;

// ==================== FIN AJUSTES ====================