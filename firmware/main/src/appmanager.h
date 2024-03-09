#ifndef MAIN_APPMANAGER_H
#define MAIN_APPMANAGER_H

#include <Arduino.h>
#include <lilka.h>

#include "app.h"

class AppManager {
public:
    ~AppManager();
    void setPanel(App *app);
    void runApp(App *app);
    void loop();

    static AppManager *getInstance();

private:
    AppManager();
    App *removeTopApp();

    App *panel;
    std::vector<App *> apps;
    static AppManager *instance;
    SemaphoreHandle_t mutex;
};

#endif // MAIN_APPMANAGER_H
