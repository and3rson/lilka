#include "app.h"

App::App(const char *name) : App(name, 0, 24, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT - 24) {}

App::App(const char *name, uint16_t x, uint16_t y, uint16_t w, uint16_t h) : name(name), x(x), y(y), w(w), h(h) {
    canvas = new lilka::Canvas(x, y, w, h);
    canvas->begin();
    backCanvas = new lilka::Canvas(x, y, w, h);
    backCanvas->begin();
    backCanvasMutex = xSemaphoreCreateMutex();
    dirty = false;
}

void App::start() {
    xTaskCreate(
        [](void *app) {
            App *a = (App *)app;
            a->run();
        },
        name, 4096, this, 1, NULL
    );
}

void App::suspend() {
    // TODO: Check if the task is already suspended
    vTaskSuspend(taskHandle);
}

void App::resume() {
    // TODO: Check if the task is already running
    vTaskResume(taskHandle);
}

void App::stop() {
    vTaskDelete(taskHandle);
}

App::~App() {
    vSemaphoreDelete(backCanvasMutex);
    delete canvas;
    delete backCanvas;
}
void App::queueDraw() {
    // Swap the front and back canvases
    xSemaphoreTake(backCanvasMutex, portMAX_DELAY);
    lilka::Canvas *temp = canvas;
    canvas = backCanvas;
    backCanvas = temp;
    xSemaphoreGive(backCanvasMutex);
    dirty = true;
    taskYIELD();
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

void App::acquireBackCanvas() {
    xSemaphoreTake(backCanvasMutex, portMAX_DELAY);
}

void App::releaseBackCanvas() {
    xSemaphoreGive(backCanvasMutex);
}

eTaskState App::getState() {
    return eTaskGetState(taskHandle);
}
