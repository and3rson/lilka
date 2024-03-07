#include <lilka.h>

void demo_disc(lilka::Canvas *canvas) {
    float x = random(16, canvas->width() - 16);
    float y = random(16, canvas->height() - 16);
    float xDir = 3;
    float yDir = 3;
    int16_t radius = 16;
    uint64_t prevRenderTime = millis();
    while (1) {
        canvas->acquireMutex();
        canvas->fillScreen(canvas->color565(0, 0, 0));
        x += xDir;
        y += yDir;
        bool hit = false;
        if (x < radius || x > canvas->width() - radius) {
            xDir *= -1;
        }
        if (y < radius || y > canvas->height() - radius) {
            yDir *= -1;
        }
        if (hit) {
            // Rotate vector a little bit randomly
            float angle = ((float)random(-30, 30)) / 180 * PI;
            float xDirNew = xDir * cos(angle) - yDir * sin(angle);
            float yDirNew = xDir * sin(angle) + yDir * cos(angle);
            xDir = xDirNew;
            yDir = yDirNew;
        }
        canvas->drawCircle(x, y, radius, 0xFFFF);
        canvas->releaseMutex();
        if (lilka::controller.getState().a.justPressed) {
            return;
        }

        // Calculate FPS
        canvas->acquireMutex();
        canvas->setCursor(16, 32);
        canvas->println("FPS: " + String(1000 / (millis() - prevRenderTime)));
        prevRenderTime = millis();
        canvas->releaseMutex();
        vTaskDelay(0);
    }
}
