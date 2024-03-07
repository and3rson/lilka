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

void AppManager::setPanel(App *app) {
    xSemaphoreTake(mutex, portMAX_DELAY);
    panel = app;
    panel->start();
    xSemaphoreGive(mutex);
}

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
        vTaskSuspend(topApp->taskHandle);
    }
}

void AppManager::loop() {
    xSemaphoreTake(mutex, portMAX_DELAY);
    if (panel->needsRedraw()) {
        Serial.println("Redrawing panel");
        panel->acquireCanvas();
        lilka::display.draw16bitRGBBitmap(
            panel->canvas->x(), panel->canvas->y(), panel->canvas->getFramebuffer(), panel->canvas->width(),
            panel->canvas->height()
        );
        panel->releaseCanvas();
        panel->markClean();
    }
    App *topApp = apps.back();
    eTaskState state = eTaskGetState(topApp->taskHandle);
    if (state == eDeleted) {
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
        vTaskResume(topApp->taskHandle);
    }
    if (topApp->needsRedraw()) {
        Serial.println("Redrawing " + String(topApp->getName()));
        topApp->acquireCanvas();
        lilka::display.draw16bitRGBBitmap(
            topApp->canvas->x(), topApp->canvas->y(), topApp->canvas->getFramebuffer(), topApp->canvas->width(),
            topApp->canvas->height()
        );
        topApp->releaseCanvas();
        topApp->markClean();
    }
    xSemaphoreGive(mutex);
    // for (auto app : apps) {
    //     eTaskState state = eTaskGetState(app->taskHandle);
    //     if (state == eDeleted) {
    //         // Remove app from list
    //         apps.pop_back();
    //         delete app;
    //         // Resume top app
    //         if (apps.size() == 0) {
    //             // No apps left - panic!
    //             lilka::serial_err("No apps left! Panic!");
    //             while (1) {
    //             }
    //         }
    //         App *topApp = apps.back();
    //         vTaskResume(topApp->taskHandle);
    //     } else {
    //         if (app->needsRedraw()) {
    //             app->acquireCanvas();
    //             lilka::display.draw16bitRGBBitmap(
    //                 app->canvas->x(), app->canvas->y(), app->canvas->getFramebuffer(), app->canvas->width(),
    //                 app->canvas->height()
    //             );
    //             app->releaseCanvas();
    //             app->markClean();
    //         }
    //     }
    // }
}
