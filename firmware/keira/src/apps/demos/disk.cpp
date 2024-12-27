#include "disk.h"

DiskApp::DiskApp() : App("Disk") {
}

void DiskApp::run() {
    float x = random(16, canvas->width() - 16);
    float y = random(16, canvas->height() - 16);
    float xDir = 5;
    float yDir = 5;
    int16_t radius = 16;
    uint64_t prevRenderTime = millis();
    while (1) {
        x += xDir;
        y += yDir;
        bool hit = false;
        if (x < radius || x > canvas->width() - radius) {
            xDir *= -1;
            hit = true;
        }
        if (y < radius || y > canvas->height() - radius) {
            yDir *= -1;
            hit = true;
        }
        if (hit) {
            // Rotate vector a little bit randomly
            float angle = ((float)random(-15, 15)) / 180 * PI;
            float xDirNew = xDir * cos(angle) - yDir * sin(angle);
            float yDirNew = xDir * sin(angle) + yDir * cos(angle);
            xDir = xDirNew;
            yDir = yDirNew;
        }
        if (lilka::controller.getState().a.justPressed) {
            return;
        }

        canvas->fillScreen(lilka::colors::Black);
        canvas->drawCircle(x, y, radius, 0xFFFF);
        canvas->setCursor(16, 32);
        canvas->println("FPS: " + String(1000 / (millis() - prevRenderTime)));
        prevRenderTime = millis();
        queueDraw();
    }
}
