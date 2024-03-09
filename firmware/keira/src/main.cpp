#include <lilka.h>

#include "servicemanager.h"
#include "appmanager.h"

#include "services/network.h"

#include "apps/statusbar.h"
#include "apps/launcher.h"

AppManager* appManager = AppManager::getInstance();
ServiceManager* serviceManager = ServiceManager::getInstance();

void setup() {
    lilka::begin();
    serviceManager->addService(new NetworkService());
    appManager->setPanel(new StatusBarApp());
    appManager->runApp(new LauncherApp());
}

void loop() {
    appManager->loop();
}
