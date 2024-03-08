// Default setup & loop functions (weak symbols)

#include <Arduino.h>
#include "lilka.h"

void __attribute__((weak)) setup() {
    lilka::begin();
}

void __attribute__((weak)) loop() {
    // TODO: FreeRTOS experiment
    lilka::Alert alert("Лілка", "Ця прошивка не має\nжодного коду.");
    alert.draw(&lilka::display);
    while (!alert.isDone()) {
        alert.update();
        delay(100);
    }
}
