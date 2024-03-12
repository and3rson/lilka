#include "transform.h"

#include <math.h>

TransformApp::TransformApp() : App("Transform") {
}

void TransformApp::run() {
    lilka::Image* face = lilka::resources.loadImage("/sd/face.bmp", canvas->color565(0, 0, 0), 32, 32);

    if (!face) {
        lilka::Alert alert("Помилка", "Не вдалось завантажити face.bmp з SD-карти.");
        alert.draw(canvas);
        queueDraw();
        while (!alert.isDone()) {
            alert.update();
        }
        return;
    }

    int x = canvas->width() / 2;
    int y = canvas->height() / 2;

    Serial.println("Drawing face at " + String(x) + ", " + String(y));

    int angle = 0;

    while (1) {
        canvas->fillScreen(canvas->color565(0, 64, 0));
        // canvas->drawImage(face, x, y);
        lilka::Transform transform = lilka::Transform().rotate(angle).scale(sin(angle / 24.0), cos(angle / 50.0));
        // lilka::Transform transform = lilka::Transform().rotate(30).scale(1.5, 1);
        // uint64_t start = micros();
        canvas->drawImageTransformed(face, x, y, transform);
        // uint64_t end = micros();
        // Serial.println("Drawing took " + String(end - start) + " us");
        queueDraw();
        angle += 8;

        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            return;
        }
    }
}
