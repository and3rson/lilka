#include "ball.h"

BallApp::BallApp() : App("Ball") {
}

void BallApp::run() {
    float x = (float)canvas->width() / 2;
    float y = (float)canvas->height() / 4;
    float xVelo = 320, yVelo = 0;
    float gravity = 1500.0;
    int16_t radius = 16;
    uint64_t prevRenderTime = millis();
    while (1) {
        float delta = (millis() - prevRenderTime) / 1000.0;

        yVelo += gravity * delta;
        x += xVelo * delta;
        y += yVelo * delta;

        if (x < radius || x > canvas->width() - radius) {
            xVelo *= -0.9;
            x = x < radius ? radius : canvas->width() - radius;
        }
        if (y < radius || y > canvas->height() - radius) {
            yVelo *= -0.9;
            xVelo *= 0.95; // Тертя
            y = y < radius ? radius : canvas->height() - radius;
        }

        lilka::State state = lilka::controller.getState();
        if (state.a.justPressed) {
            return;
        } else if (state.up.justPressed) {
            yVelo = -500;
        } else if (state.down.justPressed) {
            yVelo = 500;
        } else if (state.left.justPressed) {
            xVelo = -500;
        } else if (state.right.justPressed) {
            xVelo = 500;
        }

        canvas->fillScreen(lilka::colors::Black);
        canvas->fillCircle(x, y, radius, lilka::colors::Chrome_yellow);
        // Calculate FPS
        canvas->setCursor(16, 32);
        canvas->println("FPS: " + String(1000 / (millis() - prevRenderTime)));

        prevRenderTime = millis();
        queueDraw();
    }
}
