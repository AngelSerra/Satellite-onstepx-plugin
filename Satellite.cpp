// plugins/satellite/Satellite.cpp
#include "Satellite.h"
#include "Sat.h"

// NO declarar extern aquí

void Satellite::init() {
    Serial.println("MSG: Satellite Tracking Plugin - Starting...");
    
    // Registrar rutas en el servidor web nativo
    www.on("/sat.htm", handleSat);
    www.on("/sat-ajax.txt", satAjax);
    www.on("/sat-ajax-get.txt", satAjaxGet);
    
    Serial.println("MSG: Satellite available at /sat.htm");
}

void Satellite::loop() {
    satLoop();
}

Satellite satellite;