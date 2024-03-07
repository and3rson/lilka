// Default setup & loop functions (weak symbols)

#include <Arduino.h>
#include "lilka.h"

void __attribute__((weak)) setup() {
    lilka::begin();
}

void __attribute__((weak)) loop() {
    // TODO: FreeRTOS experiment
    lilka::Canvas canvas;
    canvas.begin();
    lilka::ui_alert(&canvas, "Лілка", "Ця програма не має жодного коду.");
    lilka::display.renderCanvas(canvas);
}
