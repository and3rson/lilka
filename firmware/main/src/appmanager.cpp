#include "appmanager.h"

AppManager *AppManager::instance = NULL;

AppManager::AppManager() {
    if (instance != NULL) {
        lilka::serial_err("AppManager instance already exists! Bad things will happen!");
        return;
    }
    instance = this;
    panel = NULL;
    mutex = xSemaphoreCreateMutex();
}

AppManager::~AppManager() {
    // TODO: Should never be destroyed
}

/// Set the panel app.
/// Panel app is drawn separately from the other apps on the top of the screen.
void AppManager::setPanel(App *app) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    panel = app;
    panel->start();
    xSemaphoreGive(mutex);
}

/// Spawn a new app and pause the current one.
void AppManager::addApp(App *app) {
    // If there's an app already running, pause it
    xSemaphoreTake(mutex, portMAX_DELAY);
    App *topApp = NULL;
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

/// Perform one iteration of the main loop.
/// Redraws the panel and the top app if necessary.
/// If the top app has finished, it is removed from the list and the next app is resumed.
void AppManager::loop() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Draw panel
    panel->acquireBackCanvas();
    if (panel->needsRedraw()) {
        lilka::display.draw16bitRGBBitmap(
            panel->backCanvas->x(), panel->backCanvas->y(), panel->backCanvas->getFramebuffer(),
            panel->backCanvas->width(), panel->backCanvas->height()
        );
        panel->markClean();
    }
    panel->releaseBackCanvas();

    // Check if top app has finished
    App *topApp = apps.back();
    if (topApp->getState() == eDeleted) {
        // Remove app from list
        apps.pop_back();
        delete topApp;
        // Resume top app
        if (apps.size() == 0) {
            // No apps left - panic!
            lilka::serial_err("No apps left! Panic!");
            while (1) {
            }
        }
        topApp = apps.back();
        topApp->resume();
    }

    // Draw top app
    topApp->acquireBackCanvas();
    if (topApp->needsRedraw()) {
        lilka::display.draw16bitRGBBitmap(
            topApp->backCanvas->x(), topApp->backCanvas->y(), topApp->backCanvas->getFramebuffer(),
            topApp->backCanvas->width(), topApp->backCanvas->height()
        );
        topApp->markClean();
    }
    topApp->releaseBackCanvas();

    xSemaphoreGive(mutex);
}
