# Satellite Tracker Plugin for OnStepX

Plugin de seguimiento de satélites para OnStepX que permite cargar TLEs, calcular posiciones en tiempo real y realizar seguimiento automático de satélites.

## Características

- **Carga de satélites** mediante TLE (Two-Line Element) desde archivo o directamente desde Celestrak por NORAD ID
- **Cálculo preciso de posición** en tiempo real (coincide con N2YO.com)
- **Visualización completa** de datos: Latitud, Longitud, Altitud, Velocidad, Azimut, Elevación, RA/DEC
- **Predicción de próximos pases** con cálculo de máxima elevación
- **Seguimiento automático** (Tracking) mientras el satélite está visible
- **GOTO** para apuntar el telescopio al satélite
- **Catálogo de satélites** con almacenamiento en NV (hasta 20 satélites)
- **Soporte multi-idioma** (Español, Inglés, Alemán, Chino)
- **Compensación horaria automática** para zona horaria española (UTC+1)

## Requisitos

- OnStepX (versión 10.x o superior)
- ESP32 (WROOM 32 recomendado)
- Librería Sgp4 incluida (Hopperpop/Sgp4-Library)

## Instalación

1. Copia la carpeta `sat` completa en `plugins/website/pages/`
2. Asegúrate de que la estructura sea:

plugins/satellite/
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

3. Recompila el firmware de OnStepX

## Uso

### Cargar un satélite

1. Ve a la pestaña **"Satélites"**
2. En la sección de posición, haz clic en **"Ajustes..."**
3. Puedes:
- Introducir un **NORAD ID** (ej: 25544 para la ISS) y presionar **"Cargar"**
- Seleccionar un **archivo TLE** (.txt) y presionar **"Cargar Archivo"**
- Usar los botones predefinidos (ISS, NOAA 15, NOAA 18, NOAA 19)

### Apuntar al satélite

- Presiona el botón **"APUNTAR"** cuando el satélite esté visible (elevación > 0°)
- El telescopio se moverá a las coordenadas RA/DEC del satélite

### Seguimiento automático

- Presiona **"SEGUIR"** para activar el seguimiento continuo
- El telescopio se actualizará cada 1-2 segundos mientras el satélite esté visible
- El seguimiento se detiene automáticamente cuando el satélite baja de 10° de elevación

### Guardar en catálogo

- Una vez cargado un satélite, introduce un nombre y presiona **"Guardar"**
- Los satélites guardados aparecerán en el selector de la sección de ajustes

## Parámetros ajustables

En `SatControlTile.h` puedes modificar:

```cpp
const double MIN_ELEVATION = 5.0;     // Elevación mínima para tracking (grados)
const double TRACKING_INTERVAL = 1000; // Intervalo de actualización (ms)
const double RA_TOLERANCE = 0.01;      // Tolerancia RA (horas)
const double DEC_TOLERANCE = 0.1;      // Tolerancia DEC (grados)

Compensación horaria
El plugin asume que el mount tiene configurada la hora LOCAL como si fuera UTC (caso común en España). Si tu mount está en UTC real, modifica en SatTimeUtils.cpp:
return mountTimestamp - 3600;  // Cambia a -0 si el mount ya está en UTC
Estructura de archivos
Archivo	Descripción
Sat.cpp	Página principal del plugin y manejador AJAX
Sat.h	Cabecera principal
SatControlTile.cpp	Tile de control (botones, tracking, estado)
SatLocationTile.cpp	Tile de ubicación del observador
SatPositionTile.cpp	Tile de posición del satélite
SatTimeUtils.cpp	Utilidades de tiempo y zona horaria
SatTracker.cpp	Interfaz con librería Sgp4
SatelliteCatalog.cpp	Almacenamiento NV de satélites
Comandos OnStepX utilizados
Comando	Función
:GC#	Obtener fecha UTC
:GL#	Obtener hora UTC
:GR#	Obtener RA actual
:GD#	Obtener DEC actual
:Sr HH:MM:SS#	Establecer RA destino
:Sd sDD*MM:SS#	Establecer DEC destino
:MS#	Iniciar GOTO
Solución de problemas
El telescopio no se mueve al presionar "APUNTAR"
Verifica que el mount no esté estacionado (Un-Park en página principal)

Comprueba que la fecha/hora del mount sea correcta

Prueba el comando :GU# desde la consola del navegador

Los datos no coinciden con N2YO
Verifica la compensación horaria en SatTimeUtils.cpp

Asegúrate de usar TLEs actualizados (descargar de Celestrak)

El seguimiento se detiene antes de tiempo
Ajusta MIN_ELEVATION a un valor más bajo (ej: 3°)

Aumenta TRACKING_INTERVAL para menos carga en el ESP32

Error de compilación: getRealLongitude no declarada
Asegúrate de que SatLocationTile.h incluye la declaración de double getRealLongitude();

Dependencias externas
Sgp4-Library (Hopperpop): https://github.com/Hopperpop/Sgp4-Library.git

Créditos
Basado en la librería Sgp4 de David Vallado (2007)

Portado a C++ por Grady Hillhouse y Hopperpop

Plugin desarrollado para OnStepX

Licencia
Este plugin se distribuye bajo la misma licencia que OnStepX.
Notas de versión
v1.0
Primera versión funcional

Cálculo de posición preciso (coincide con N2YO)

Tracking automático y GOTO

Catálogo de satélites

Soporte multi-idioma