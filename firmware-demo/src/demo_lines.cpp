#include <lilka.h>

void demo_lines() {
    while (1) {
        int x1 = random(0, lilka::display.width());
        int y1 = random(0, lilka::display.height());
        int x2 = random(0, lilka::display.width());
        int y2 = random(0, lilka::display.height());
        uint16_t color = random(0, 0xFFFF);
        lilka::display.drawLine(x1, y1, x2, y2, color);
        if (lilka::controller.getState().start.justPressed) {
            return;
        }
    }
}
