#include "app.h"

App::App(const char* name) : App(name, 0, 24, lilka::display.width(), lilka::display.height() - 24) {
}

App::App(const char* name, uint16_t x, uint16_t y, uint16_t w, uint16_t h) : name(name), x(x), y(y), w(w), h(h) {
    canvas = new lilka::Canvas(x, y, w, h);
    canvas->begin();
    canvas->fillScreen(0);
    backCanvas = new lilka::Canvas(x, y, w, h);
    backCanvas->begin();
    backCanvas->fillScreen(0);
    backCanvasMutex = xSemaphoreCreateMutex();
    isDrawQueued = false;
    flags = AppFlags::APP_FLAG_NONE;
    Serial.println(
        "Created app " + String(name) + " at " + String(x) + ", " + String(y) + " with size " + String(w) + "x" +
        String(h)
    );
}

void App::start() {
    Serial.println("Starting app " + String(name));
    xTaskCreate(_run, name, 32768, this, 1, &taskHandle);
}

void App::_run(void* data) {
    App* app = (App*)data;
    app->run();
    if (app->getState() != eTaskState::eDeleted) {
        // App might have been stopped by itself. If not, stop it, or we'll get panic from FreeRTOS kernel.
        app->stop();
    }
}

void App::suspend() {
    // TODO: Check if the task is already suspended
    Serial.println("Suspending app " + String(name) + " (state = " + String(getState()) + ")");
    vTaskSuspend(taskHandle);
}

void App::resume() {
    // TODO: Check if the task is already running
    Serial.println("Resuming app " + String(name) + " (state = " + String(getState()) + ")");
    vTaskResume(taskHandle);
}

void App::stop() {
    Serial.println("Stopping app " + String(name) + " (state = " + String(getState()) + ")");
    vTaskDelete(taskHandle);
}

App::~App() {
    vSemaphoreDelete(backCanvasMutex);
    delete canvas;
    delete backCanvas;
}
void App::queueDraw() {
    // Swap the front and back canvases
    // Serial.println("Queuing draw for " + String(name) + ", canvas address = " + String((uint32_t)canvas));
    xSemaphoreTake(backCanvasMutex, portMAX_DELAY);
    lilka::Canvas* temp = canvas;
    canvas = backCanvas;
    backCanvas = temp;
    isDrawQueued = true;
    xSemaphoreGive(backCanvasMutex);
    taskYIELD();
}

void App::setFlags(AppFlags flags) {
    this->flags = flags;
}

AppFlags App::getFlags() {
    return flags;
}

bool App::needsRedraw() {
    return isDrawQueued;
}

void App::markClean() {
    isDrawQueued = false;
}

void App::forceRedraw() {
    isDrawQueued = true;
}

const char* App::getName() {
    return name;
}

void App::acquireBackCanvas() {
    xSemaphoreTake(backCanvasMutex, portMAX_DELAY);
}

void App::releaseBackCanvas() {
    xSemaphoreGive(backCanvasMutex);
}

eTaskState App::getState() {
    if (taskHandle == NULL) {
        return eTaskState::eInvalid;
    }
    return eTaskGetState(taskHandle);
}
