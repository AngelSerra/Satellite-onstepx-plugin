// pages/sat/SatelliteCatalog.cpp
#include "SatelliteCatalog.h"
#include "../../../../lib/nv/Nv.h"
#include <string.h>
#include <Arduino.h>

SatelliteCatalog::SatelliteCatalog() {
    _count = 0;
    for (int i = 0; i < MAX_SATELLITES; i++) {
        memset(&_satellites[i], 0, sizeof(SatelliteEntry));
        _satellites[i].valid = false;
    }
    loadFromNV();
}

void SatelliteCatalog::loadFromNV() {
    // Intentar cargar los datos - si no hay datos, simplemente se queda en 0
    nv().kv().get("SAT_CATALOG", *this);
    
    // Validar datos cargados
    if (_count > MAX_SATELLITES) _count = MAX_SATELLITES;
    for (int i = 0; i < _count; i++) {
        // Asegurar que los strings están terminados
        _satellites[i].name[31] = '\0';
        _satellites[i].line1[MAX_TLE_LINE - 1] = '\0';
        _satellites[i].line2[MAX_TLE_LINE - 1] = '\0';
        
        // Validar que el TLE tenga formato correcto
        if (_satellites[i].valid) {
            if (strlen(_satellites[i].line1) < 60 || _satellites[i].line1[0] != '1') {
                _satellites[i].valid = false;
            }
            if (strlen(_satellites[i].line2) < 60 || _satellites[i].line2[0] != '2') {
                _satellites[i].valid = false;
            }
        }
    }
}

void SatelliteCatalog::saveToNV() {
    // Limpiar datos antes de guardar
    for (int i = 0; i < _count; i++) {
        // Asegurar que los strings están correctamente terminados
        _satellites[i].name[31] = '\0';
        _satellites[i].line1[MAX_TLE_LINE - 1] = '\0';
        _satellites[i].line2[MAX_TLE_LINE - 1] = '\0';
        
        // Verificar integridad del TLE antes de guardar
        if (_satellites[i].valid) {
            if (strlen(_satellites[i].line1) < 69 || _satellites[i].line1[0] != '1') {
                _satellites[i].valid = false;
            }
            if (strlen(_satellites[i].line2) < 69 || _satellites[i].line2[0] != '2') {
                _satellites[i].valid = false;
            }
        }
    }
    nv().kv().put("SAT_CATALOG", *this);
}

bool SatelliteCatalog::addSatellite(const char* name, const char* line1, const char* line2) {
    if (_count >= MAX_SATELLITES) return false;
    
    // Validar TLE antes de añadir
    if (strlen(line1) < 69 || line1[0] != '1') return false;
    if (strlen(line2) < 69 || line2[0] != '2') return false;
    
    // Limpiar la entrada
    memset(&_satellites[_count], 0, sizeof(SatelliteEntry));
    
    // Copiar nombre
    strncpy(_satellites[_count].name, name, 31);
    _satellites[_count].name[31] = '\0';
    
    // Copiar TLE línea 1 - preservar exactamente los espacios
    strncpy(_satellites[_count].line1, line1, MAX_TLE_LINE - 1);
    _satellites[_count].line1[MAX_TLE_LINE - 1] = '\0';
    
    // Copiar TLE línea 2 - preservar exactamente los espacios
    strncpy(_satellites[_count].line2, line2, MAX_TLE_LINE - 1);
    _satellites[_count].line2[MAX_TLE_LINE - 1] = '\0';
    
    _satellites[_count].valid = true;
    
    _count++;
    saveToNV();
    return true;
}

bool SatelliteCatalog::removeSatellite(int index) {
    if (index < 0 || index >= _count) return false;
    
    for (int i = index; i < _count - 1; i++) {
        _satellites[i] = _satellites[i + 1];
    }
    _count--;
    memset(&_satellites[_count], 0, sizeof(SatelliteEntry));
    _satellites[_count].valid = false;
    saveToNV();
    return true;
}

SatelliteEntry* SatelliteCatalog::getSatellite(int index) {
    if (index < 0 || index >= _count) return NULL;
    if (!_satellites[index].valid) return NULL;
    return &_satellites[index];
}

int SatelliteCatalog::getCount() {
    return _count;
}

void SatelliteCatalog::clear() {
    _count = 0;
    for (int i = 0; i < MAX_SATELLITES; i++) {
        memset(&_satellites[i], 0, sizeof(SatelliteEntry));
        _satellites[i].valid = false;
    }
    saveToNV();
}

SatelliteCatalog satCatalog;