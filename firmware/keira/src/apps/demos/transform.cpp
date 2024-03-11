#include "transform.h"

TransformApp::TransformApp() : App("Transform") {
}

void TransformApp::run() {
    lilka::Image* face = lilka::resources.loadImage("/sd/face.bmp");

    int x = canvas->width() / 2 - face->width / 2;
    int y = canvas->height() / 2 - face->height / 2;

    int angle = 0;

    while (1) {
        canvas->fillScreen(canvas->color565(0, 0, 0));
        lilka::Transform transform = lilka::Transform().rotate(angle);
        canvas->drawImageTransformed(face, x, y, transform);
        queueDraw();
        angle++;

        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            return;
        }
    }
}
