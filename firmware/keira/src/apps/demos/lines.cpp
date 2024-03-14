#include <lilka.h>

#include "lines.h"

DemoLines::DemoLines() : App("Лінії") {
}

void DemoLines::run() {
    // Створюємо додатковий буфер для малювання ліній.
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.fillScreen(0);

    while (1) {
        // Малюємо 64 випадкові лінії на буфері.
        for (int i = 0; i < 64; i++) {
            int x1 = random(0, canvas->width());
            int y1 = random(0, canvas->height());
            int x2 = random(0, canvas->width());
            int y2 = random(0, canvas->height());
            uint16_t color = random(0, 0xFFFF);
            buffer.drawLine(x1, y1, x2, y2, color);
        }
        // Малюємо наш буфер в передній буфер.
        canvas->drawCanvas(&buffer);
        queueDraw();

        // Перевіряємо, чи натиснута кнопка "A".
        if (lilka::controller.getState().a.justPressed) {
            return;
        }

        // Чекаємо 30 мс.
        vTaskDelay(30 / portTICK_PERIOD_MS);
    }
}
