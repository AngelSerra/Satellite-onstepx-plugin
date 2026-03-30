// plugins/satellite/Sgp4Wrapper.h
#pragma once

// Incluir SGP4
#include "Sgp4/src/sgp4pred.h"

// Namespace para aislar SGP4 y evitar conflictos
namespace Sgp4Lib {
    // Re-exportar las funciones necesarias
    using ::Sgp4;
}

// Evitar que el conflicto de site() se propague
// No usamos la función site() de SGP4 directamente