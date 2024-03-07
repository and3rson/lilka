#include <lilka.h>

#include "lines.h"

DemoLines::DemoLines() : App("Лінії") {}

void DemoLines::run() {
    acquireCanvas();
    canvas->fillScreen(0);
    releaseCanvas();
    while (1) {
        acquireCanvas();
        int x1 = random(0, canvas->width());
        int y1 = random(0, canvas->height());
        int x2 = random(0, canvas->width());
        int y2 = random(0, canvas->height());
        uint16_t color = random(0, 0xFFFF);
        canvas->drawLine(x1, y1, x2, y2, color);
        releaseCanvas();
        if (lilka::controller.getState().a.justPressed) {
            vTaskDelete(NULL);
            return;
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}
