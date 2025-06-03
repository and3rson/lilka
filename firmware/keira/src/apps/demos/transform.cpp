#include "transform.h"
#include "keira.h"
#include <math.h>

TransformApp::TransformApp() : App("Transform") {
}

void TransformApp::run() {
    lilka::Image* face = lilka::resources.loadImage("/sd/face.bmp", lilka::colors::Black, 32, 32);

    if (!face) {
        lilka::Alert alert(K_S_ERROR, K_S_TRANSFORM_CANT_LOAD_FACE);
        alert.draw(canvas);
        queueDraw();
        while (!alert.isFinished()) {
            alert.update();
        }
        return;
    }

    int x = canvas->width() / 2;
    int y = canvas->height() / 2;

    Serial.println(K_S_DRAWING_FACE_AT_PREFIX + String(x) + ", " + String(y));

    int angle = 0;

    while (1) {
        canvas->fillScreen(lilka::colors::Myrtle_green);
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
