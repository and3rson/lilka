#include "statusbar.h"
#include "icons/battery.h"
#include "icons/battery_danger.h"
#include "icons/battery_absent.h"
#include "icons/wifi_offline.h"
#include "icons/wifi_connecting.h"
#include "icons/wifi_0.h"
#include "icons/wifi_1.h"
#include "icons/wifi_2.h"
#include "icons/wifi_3.h"
#include "servicemanager.h"
#include "services/network.h"

StatusBarApp::StatusBarApp() : App("StatusBar", 0, 0, lilka::display.width(), 24) {
}

const uint16_t* icons[] = {wifi_offline, wifi_0, wifi_1, wifi_2, wifi_3};

void StatusBarApp::run() {
    int counter = 0;
    NetworkService* networkService = ServiceManager::getInstance()->getService<NetworkService>();
    while (1) {
        canvas->fillScreen(lilka::display.color565(0, 0, 0));

        int16_t xOffset = 144;

        // Print counter for debug purpose
        // TODO: Replace with actual time from NTP
        canvas->setTextColor(lilka::display.color565(255, 255, 255), lilka::display.color565(0, 0, 0));
        canvas->setFont(FONT_9x15);
        canvas->setCursor(32, 17);
        canvas->print("Час: " + String(counter++));

        // Draw WiFi signal strength
        if (networkService != NULL) {
            if (networkService->getNetworkState() == NETWORK_STATE_OFFLINE) {
                canvas->draw16bitRGBBitmapWithTranColor(144, 0, wifi_offline, 0, 24, 24);
            } else if (networkService->getNetworkState() == NETWORK_STATE_CONNECTING) {
                canvas->draw16bitRGBBitmapWithTranColor(144, 0, wifi_connecting, 0, 24, 24);
            } else {
                canvas->draw16bitRGBBitmapWithTranColor(144, 0, icons[networkService->getSignalStrength()], 0, 24, 24);
            }
        }
        xOffset += 8 + 24;

        // Draw battery
        int level = lilka::battery.readLevel();
        if (level == -1) {
            canvas->draw16bitRGBBitmapWithTranColor(
                xOffset, 0, battery_absent, lilka::display.color565(255, 0, 255), 32, 24
            );
        } else {
            int16_t x1 = 6, y1 = 8;
            int16_t fullWidth = 22, h = 8;
            int filledWidth = fullWidth * level / 100;
            if (filledWidth < 1) filledWidth = 1;
            int16_t color =
                level > 50 ? lilka::display.color565(0, 255, 0)
                           : (level > 20 ? lilka::display.color565(255, 255, 0) : lilka::display.color565(255, 0, 0));
            canvas->draw16bitRGBBitmapWithTranColor(
                xOffset, 0, level > 10 ? battery : battery_danger, lilka::display.color565(255, 0, 255), 32, 24
            );
            canvas->fillRect(xOffset + x1 + (fullWidth - filledWidth), y1, filledWidth, h, color);
        }
        xOffset += 8 + 32;
        canvas->setCursor(xOffset, 17);
        canvas->print(String(level) + "%");

        // Draw everything
        queueDraw();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
