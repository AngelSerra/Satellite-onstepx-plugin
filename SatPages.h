#pragma once

#include "Sat.h"

inline void registerSatPages() {
    www.on("/sat.htm", handleSat);
    www.on("/sat-ajax-get.txt", satAjaxGet);
    www.on("/sat-ajax.txt", satAjax);
}