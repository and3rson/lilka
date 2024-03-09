#include "appmanager.h"
#include <lilka/default_splash.h>

AppManager *AppManager::instance = NULL;

AppManager::AppManager() {
    panel = NULL;
    mutex = xSemaphoreCreateMutex();
}

AppManager::~AppManager() {
    // TODO: Should never be destroyed
}

AppManager *AppManager::getInstance() {
    // TODO: Not thread-safe, but is first called in static context before any tasks are created
    if (instance == NULL) {
        instance = new AppManager();
    }
    return instance;
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
void AppManager::runApp(App *app) {
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

/// Remove the top app and resume the previous one.
/// Returns new top app.
App *AppManager::removeTopApp() {
    App *topApp = apps.back();
    apps.pop_back();
    delete topApp;
    if (apps.size() > 0) {
        topApp = apps.back();
        topApp->resume();
    }
    topApp->forceRedraw();
    panel->forceRedraw();
    return topApp;
}

/// Perform one iteration of the main loop.
/// Redraws the panel and the top app if necessary.
/// If the top app has finished, it is removed from the list and the next app is resumed.
void AppManager::loop() {
    xSemaphoreTake(mutex, portMAX_DELAY);

    // Check if top app has finished
    App *topApp = apps.back();
    if (topApp->getState() == eDeleted) {
        // Switch to the next app in stack
        topApp = removeTopApp();
    }

    // Draw panel and top app
    for (App *app : {panel, topApp}) {
        if (app == panel) {
            // Check if topApp is fullscreen. If it is, don't draw the panel
            if (topApp->getFlags() & AppFlags::APP_FLAG_FULLSCREEN) {
                continue;
            }
        }
        app->acquireBackCanvas();
        if (app->needsRedraw()) {
            lilka::display.draw16bitRGBBitmap(
                app->backCanvas->x(), app->backCanvas->y(), app->backCanvas->getFramebuffer(), app->backCanvas->width(),
                app->backCanvas->height()
            );
            app->markClean();
        }
        app->releaseBackCanvas();
    }

    xSemaphoreGive(mutex);

    taskYIELD();
}
