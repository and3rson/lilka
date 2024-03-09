#include <lilka.h>

#include "lines.h"

DemoLines::DemoLines() : App("Лінії") {}

void DemoLines::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);
    canvas->drawCanvas(&buffer);
    queueDraw();

    while (1) {
        for (int i = 0; i < 64; i++) {
            int x1 = random(0, canvas->width());
            int y1 = random(0, canvas->height());
            int x2 = random(0, canvas->width());
            int y2 = random(0, canvas->height());
            uint16_t color = random(0, 0xFFFF);
            buffer.drawLine(x1, y1, x2, y2, color);
        }
        canvas->drawCanvas(&buffer);
        queueDraw();
        if (lilka::controller.getState().a.justPressed) {
            stop();
            return;
        }
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}
