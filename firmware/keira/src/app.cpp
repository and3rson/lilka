#include "app.h"

App::App(const char* name) : App(name, 0, 24, lilka::display.width(), lilka::display.height() - 24) {
}

App::App(const char* name, uint16_t x, uint16_t y, uint16_t w, uint16_t h) :
    name(name),
    flags(AppFlags::APP_FLAG_NONE),
    taskHandle(NULL),
    canvas(new lilka::Canvas(x, y, w, h)),
    backCanvas(new lilka::Canvas(x, y, w, h)),
    isDrawQueued(false),
    backCanvasMutex(xSemaphoreCreateMutex()),
    stackSize(8192),
    appCore(0),
    frame(0) {
    // Clear buffers
    canvas->fillScreen(0);
    backCanvas->fillScreen(0);
    lilka::serial.log("Created app %s at %d, %d with size %dx%d on core %d", name, x, y, w, h, appCore);
    xSemaphoreGive(backCanvasMutex);
}

void App::start() {
    if (taskHandle != NULL) {
        lilka::serial.err("App %s is already running", name);
        return;
    }
    lilka::serial.log("Starting app %s", name);
    if (xTaskCreatePinnedToCore(_run, name, stackSize, this, 1, &taskHandle, appCore) != pdPASS) {
        lilka::serial.err(
            "Failed to create task for app %s"
            " - not enough memory? Try increasing stack size with setStackSize()",
            name
        );
    }
}

void App::_run(void* data) {
    App* app = static_cast<App*>(data);
    app->run();
    if (app->getState() != eTaskState::eDeleted) {
        // App might have been stopped by itself. If not, stop it, or we'll get panic from FreeRTOS kernel.
        app->stop();
    }
}

void App::suspend() {
    if (taskHandle == NULL) {
        lilka::serial.err("App %s is not running, cannot suspend", name);
        return;
    }
    lilka::serial.log("Suspending app %s (state = %d)", name, getState());
    onSuspend();
    vTaskSuspend(taskHandle);
}

void App::resume() {
    if (taskHandle == NULL) {
        lilka::serial.err("App %s is not running, cannot resume", name);
        return;
    }
    lilka::serial.log("Resuming app %s (state = %d)", name, getState());
    onResume();
    vTaskResume(taskHandle);
}

void App::stop() {
    if (taskHandle == NULL) {
        lilka::serial.err("App %s is not running, cannot stop", name);
        return;
    }
    lilka::serial.log("Stopping app %s (state = %d)", name, getState());
    onStop();
    vTaskDelete(taskHandle);
    taskHandle = NULL;
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
    frame++;
    xSemaphoreGive(backCanvasMutex);
    taskYIELD();
}

void App::setCore(int appCore) {
    this->appCore = appCore;
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

void App::setStackSize(uint32_t stackSize) {
    this->stackSize = stackSize;
}
