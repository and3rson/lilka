#include "statusbar.h"
#include "icons/battery.h"
#include "icons/wifi_offline.h"
#include "icons/wifi_connecting.h"
#include "icons/wifi_0.h"
#include "icons/wifi_1.h"
#include "icons/wifi_2.h"
#include "icons/wifi_3.h"
#include "servicemanager.h"
#include "services/network.h"

StatusBarApp::StatusBarApp() : App("StatusBar", 0, 0, lilka::display.width(), 24) {}

const uint16_t *icons[] = {wifi_offline, wifi_0, wifi_1, wifi_2, wifi_3};

void StatusBarApp::run() {
    int counter = 0;
    NetworkService *networkService = ServiceManager::getInstance()->getService<NetworkService>();
    while (1) {
        canvas->fillScreen(lilka::display.color565(0, 0, 0));
        canvas->setTextColor(lilka::display.color565(255, 255, 255), lilka::display.color565(0, 0, 0));
        canvas->setFont(FONT_9x15);
        canvas->setCursor(32, 18);
        canvas->print("Time: " + String(counter++));
        if (networkService != NULL) {
            if (networkService->getNetworkState() == NETWORK_STATE_OFFLINE) {
                canvas->draw16bitRGBBitmapWithTranColor(144, 0, wifi_offline, 0, 24, 24);
            } else if (networkService->getNetworkState() == NETWORK_STATE_CONNECTING) {
                canvas->draw16bitRGBBitmapWithTranColor(144, 0, wifi_connecting, 0, 24, 24);
            } else {
                canvas->draw16bitRGBBitmapWithTranColor(144, 0, icons[networkService->getSignalStrength()], 0, 24, 24);
            }
        }
        canvas->draw16bitRGBBitmapWithTranColor(144 + 8 + 24, 0, battery, 0, 32, 24);
        // canvas->draw16bitRGBBitmapWithTranColor(160, 0, wifi, 24, 24, 0);
        // canvas->draw16bitRGBBitmapWithTranColor(160 + 24, 0, battery, 24, 24, 0);
        queueDraw();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
