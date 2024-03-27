#ifndef MAIN_APPMANAGER_H
#define MAIN_APPMANAGER_H

#include <Arduino.h>
#include <lilka.h>

#include "app.h"

class AppManager {
public:
    ~AppManager();
    void setPanel(App* app);
    void runApp(App* app);
    void loop();
    void renderToCanvas(lilka::Canvas* canvas);
    void startToast(String message, uint64_t duration = 2000);

    static AppManager* getInstance();

private:
    AppManager();
    App* removeTopApp();

    App* panel;
    std::vector<App*> apps;
    static AppManager* instance;
    SemaphoreHandle_t mutex;

    String toastMessage;
    uint64_t toastEndTime;
};

#endif // MAIN_APPMANAGER_H
