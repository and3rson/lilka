#include "app.h"

App::App(const char *name) : App(name, 0, 24, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT - 24) {}

App::App(const char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h) : name(name), x(x), y(y), w(w), h(h) {
    canvas = new lilka::Canvas(x, y, w, h);
    canvas->begin();
    mutex = xSemaphoreCreateMutex();
    dirty = false;
}
void App::start() {
    xTaskCreate([](void *app) { ((App *)app)->run(); }, name, 32768, this, 1, &taskHandle);
}
App::~App() {
    vSemaphoreDelete(mutex);
    delete canvas;
}
void App::acquireCanvas() {
    xSemaphoreTake(mutex, portMAX_DELAY);
}
void App::releaseCanvas() {
    dirty = true;
    xSemaphoreGive(mutex);
}
bool App::needsRedraw() {
    return dirty;
}
void App::markClean() {
    dirty = false;
}

const char *App::getName() {
    return name;
}
