// Default setup & loop functions (weak symbols)

#include <Arduino.h>
#include "lilka.h"

void __attribute__((weak)) setup() {
    lilka::begin();
}

void __attribute__((weak)) loop() {
    lilka::ui_alert("Лілка", "Ця програма не має жодного коду.");
}
