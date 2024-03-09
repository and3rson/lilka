#include "epilepsy.h"

EpilepsyApp::EpilepsyApp() : App("Epilepsy") {
}

void EpilepsyApp::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);
    canvas->drawCanvas(&buffer);
    queueDraw();

    while (1) {
        float time = millis() / 1000.0;
        float size = sin(time * PI * 1.5) * canvas->width();
        float angle = time * 4;

        // Draw a square with a side of size `size` and a rotation of `angle`
        uint16_t color = random(0, 0xFFFF);
        double points[4][2] = {
            {(float)canvas->width() / 2 + size * cos(angle + PI / 4),
             (float)canvas->height() / 2 + size * sin(angle + PI / 4)},
            {(float)canvas->width() / 2 + size * cos(angle + 3 * PI / 4),
             (float)canvas->height() / 2 + size * sin(angle + 3 * PI / 4)},
            {(float)canvas->width() / 2 + size * cos(angle + 5 * PI / 4),
             (float)canvas->height() / 2 + size * sin(angle + 5 * PI / 4)},
            {(float)canvas->width() / 2 + size * cos(angle + 7 * PI / 4),
             (float)canvas->height() / 2 + size * sin(angle + 7 * PI / 4)},
        };
        buffer.fillTriangle(points[0][0], points[0][1], points[1][0], points[1][1], points[2][0], points[2][1], color);
        buffer.fillTriangle(points[0][0], points[0][1], points[2][0], points[2][1], points[3][0], points[3][1], color);
        canvas->drawCanvas(&buffer);
        queueDraw();

        if (lilka::controller.getState().a.justPressed) {
            return;
        }
    }
}
