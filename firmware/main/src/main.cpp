// #include <esp_wifi.h>
//
// #include "nes/hw_config.h"
//
// extern "C" {
// #include <nofrendo.h>
// }
//
//
// // #include "demos/demos.h"
// #include "demos/lines.h"

#include <lilka.h>

#include "appmanager.h"
#include "app.h"

#include "apps/statusbar.h"
#include "apps/launcher.h"

AppManager *appManager = AppManager::getInstance();

void setup() {
    lilka::begin();
    appManager->setPanel(new StatusBarApp());
    appManager->addApp(new LauncherApp());
}

void loop() {
    appManager->loop();
    taskYIELD();
}
