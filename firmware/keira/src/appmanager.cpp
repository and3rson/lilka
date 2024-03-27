#include "appmanager.h"
#include <lilka/default_splash.h>

AppManager* AppManager::instance = NULL;

AppManager::AppManager() : mutex(xSemaphoreCreateMutex()), panel(NULL), toastMessage(""), toastEndTime(0) {
}

AppManager::~AppManager() {
    // TODO: Should never be destroyed
}

AppManager* AppManager::getInstance() {
    // TODO: Not thread-safe, but is first called in static context before any tasks are created
    if (instance == NULL) {
        instance = new AppManager();
    }
    return instance;
}

/// Set the panel app.
/// Panel app is drawn separately from the other apps on the top of the screen.
void AppManager::setPanel(App* app) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    panel = app;
    panel->start();
    xSemaphoreGive(mutex);
}

/// Spawn a new app and pause the current one.
void AppManager::runApp(App* app) {
    // If there's an app already running, pause it
    xSemaphoreTake(mutex, portMAX_DELAY);
    App* topApp = NULL;
    if (apps.size() > 0) {
        topApp = apps.back();
    }
    apps.push_back(app);
    app->start();
    xSemaphoreGive(mutex);
    if (topApp != NULL) {
        // This method may be called from an app itself, that's why we postpone the suspension till the end of this method
        topApp->suspend();
    }
}

/// Remove the top app and resume the previous one.
/// Returns new top app.
App* AppManager::removeTopApp() {
    App* topApp = apps.back();
    apps.pop_back();
    delete topApp;
    if (apps.size() == 0) {
        // Panic! No apps left
        lilka::serial_err("appmanager: no apps left! Panic!");
        while (1) {
        }
    } else {
        topApp = apps.back();
        topApp->resume();
        topApp->forceRedraw();
        panel->forceRedraw();
        return topApp;
    }
}

/// Perform one iteration of the main loop.
/// Redraws the panel and the top app if necessary.
/// If the top app has finished, it is removed from the list and the next app is resumed.
void AppManager::loop() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Check if top app has finished
    App* topApp = apps.back();
    if (topApp->getState() == eDeleted) {
        // Switch to the next app in stack
        topApp = removeTopApp();
    }

    // Draw panel and top app
    for (App* app : {panel, topApp}) {
        if (app == panel) {
            // Check if topApp is fullscreen. If it is, don't draw the panel
            if (topApp->getFlags() & AppFlags::APP_FLAG_FULLSCREEN) {
                continue;
            }
        }
        app->acquireBackCanvas();
        if (app->needsRedraw()) {
            lilka::display.draw16bitRGBBitmap(
                app->backCanvas->x(),
                app->backCanvas->y(),
                app->backCanvas->getFramebuffer(),
                app->backCanvas->width(),
                app->backCanvas->height()
            );
            app->markClean();
        }
        app->releaseBackCanvas();
    }

    // Draw toast message
    if (millis() < toastEndTime) {
        int16_t x, y;
        uint16_t w, h;
        lilka::display.setFont(FONT_8x13);
        lilka::display.getTextBounds(toastMessage.c_str(), 0, 0, &x, &y, &w, &h);
        int16_t cx = lilka::display.width() / 2;
        int16_t cy = lilka::display.height() / 7 * 6;
        lilka::display.fillRect(cx - w / 2 - 5, cy - h - 5, w + 10, h + 10, lilka::display.color565(0, 0, 0));
        lilka::display.setTextColor(lilka::display.color565(255, 255, 255));
        lilka::display.setCursor(cx - w / 2, cy + h / 2);
        lilka::display.print(toastMessage.c_str());
    }

    xSemaphoreGive(mutex);

    taskYIELD();
}

/// Render panel and top app to the given canvas.
/// Useful for taking screenshots.
void AppManager::renderToCanvas(lilka::Canvas* canvas) {
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Draw panel and top app
    for (App* app : {panel, apps.back()}) {
        app->acquireBackCanvas();
        canvas->draw16bitRGBBitmap(
            app->backCanvas->x(),
            app->backCanvas->y(),
            app->backCanvas->getFramebuffer(),
            app->backCanvas->width(),
            app->backCanvas->height()
        );
        app->releaseBackCanvas();
    }

    xSemaphoreGive(mutex);
}

/// Display a toast message.
void AppManager::startToast(String message, uint64_t duration) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    toastMessage = message;
    toastEndTime = millis() + duration;
    xSemaphoreGive(mutex);
}
