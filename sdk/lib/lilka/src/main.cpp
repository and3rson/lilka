// Default setup & loop functions (weak symbols)

#include <Arduino.h>
#include "lilka.h"

void __attribute__((weak)) setup() {
    lilka::begin();
}

void __attribute__((weak)) loop() {
    // TODO: FreeRTOS experiment
    lilka::Alert alert("Лілка", "Ця програма не має жодного коду.");
    while (!alert.isDone()) {
        alert.update();
        alert.draw(&lilka::display);
        delay(100);
    }
}
