#include <lilka.h>

void demo_lines(lilka::Canvas *canvas) {
    while (1) {
        canvas->acquireMutex();
        int x1 = random(0, canvas->width());
        int y1 = random(0, canvas->height());
        int x2 = random(0, canvas->width());
        int y2 = random(0, canvas->height());
        uint16_t color = random(0, 0xFFFF);
        canvas->drawLine(x1, y1, x2, y2, color);
        canvas->releaseMutex();
        if (lilka::controller.getState().a.justPressed) {
            return;
        }
        vTaskDelay(0);
    }
}
