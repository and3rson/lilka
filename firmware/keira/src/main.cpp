#include <lilka.h>

#include "keira_splash.h"

#include "servicemanager.h"
#include "appmanager.h"

#include "services/clock.h"
#include "services/network.h"
#include "services/screenshot.h"
#include "apps/statusbar.h"
#include "apps/launcher.h"

AppManager* appManager = AppManager::getInstance();
ServiceManager* serviceManager = ServiceManager::getInstance();

void setup() {
    lilka::display.setSplash(keira_splash, keira_splash_length);
    lilka::begin();
    serviceManager->addService(new NetworkService());
    serviceManager->addService(new ClockService());
    serviceManager->addService(new ScreenshotService());
    appManager->setPanel(new StatusBarApp());
    appManager->runApp(new LauncherApp());
}

void loop() {
    appManager->loop();
}
