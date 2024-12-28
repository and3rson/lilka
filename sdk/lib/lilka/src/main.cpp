// Default setup & loop functions (weak symbols)

#include <Arduino.h>
#include "lilka.h"

void __attribute__((weak)) setup() {
    lilka::begin();
}

void __attribute__((weak)) loop() {
    lilka::Alert alert("Лілка", "Ця прошивка не має\nжодного коду.");
    alert.draw(&lilka::display);
    while (!alert.isFinished()) {
        alert.update();
    }
}
