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
#include "icons/ram.h"
#include "servicemanager.h"
#include "services/clock.h"
#include "services/network.h"

StatusBarApp::StatusBarApp() : App("StatusBar", 0, 0, lilka::display.width(), 24) {
}

const uint16_t* icons[] = {wifi_0, wifi_1, wifi_2, wifi_3};

void StatusBarApp::run() {
    lilka::Canvas iconCanvas(240, 24);
    while (1) {
        canvas->fillScreen(lilka::colors::Black);

        ClockService* clockService = ServiceManager::getInstance()->getService<ClockService>("clock");
        canvas->setTextColor(lilka::colors::White, lilka::colors::Black);
        canvas->setFont(FONT_9x15);
        canvas->setCursor(24, 17);
        struct tm timeinfo = clockService->getTime();
        char strftime_buf[16];
        strftime(strftime_buf, sizeof(strftime_buf), "%H:%M:%S", &timeinfo);
        canvas->print(strftime_buf);

        // Draw icons
        int16_t xOffset = drawIcons(&iconCanvas);
        canvas->draw16bitRGBBitmap(canvas->width() - xOffset - 16, 0, iconCanvas.getFramebuffer(), 240, 24);

        // Draw everything
        queueDraw();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

int16_t StatusBarApp::drawIcons(lilka::Canvas* iconCanvas) {
    NetworkService* networkService = ServiceManager::getInstance()->getService<NetworkService>("network");

    int16_t xOffset = 0;

    iconCanvas->fillScreen(lilka::colors::Black);
    iconCanvas->setFont(FONT_9x15);

    // Draw RAM usage
    uint32_t freeRAM = ESP.getFreeHeap();
    uint32_t totalRAM = ESP.getHeapSize();
    int16_t padding = 2;
    int16_t barWidth = 24 - padding * 2;
    int16_t barHeight = 10;
    int16_t barWidthUsed = barWidth * (totalRAM - freeRAM) / totalRAM;
    iconCanvas->fillRect(xOffset + padding, padding, barWidthUsed, barHeight, lilka::colors::Red);
    iconCanvas->draw16bitRGBBitmapWithTranColor(xOffset, 0, ram, lilka::colors::Black, 24, 24);
    xOffset += 4 + 24;

    // Draw WiFi signal strength
    if (networkService != NULL) {
        if (networkService->getNetworkState() == NETWORK_STATE_OFFLINE) {
            iconCanvas->draw16bitRGBBitmapWithTranColor(xOffset, 0, wifi_offline, 0, 24, 24);
        } else if (networkService->getNetworkState() == NETWORK_STATE_CONNECTING) {
            iconCanvas->draw16bitRGBBitmapWithTranColor(xOffset, 0, wifi_connecting, 0, 24, 24);
        } else if (networkService->getNetworkState() == NETWORK_STATE_ONLINE) {
            iconCanvas->draw16bitRGBBitmapWithTranColor(
                xOffset, 0, icons[networkService->getSignalStrength()], 0, 24, 24
            );
        }
        xOffset += 4 + 24;
    }

    // Draw battery
    int level = lilka::battery.readLevel();
    if (level == -1) {
        iconCanvas->draw16bitRGBBitmapWithTranColor(xOffset, 0, battery_absent, lilka::colors::Fuchsia, 16, 24);
        xOffset += 4 + 16;
    } else {
        int16_t x1 = 4, y1 = 6;
        int16_t width = 8, fullHeight = 14;
        int filledHeight = fullHeight * level / 100;
        if (filledHeight < 1) filledHeight = 1;
        int emptyHeight = fullHeight - filledHeight;
        int16_t color = level > 50 ? lilka::colors::Green : (level > 20 ? lilka::colors::Yellow : lilka::colors::Red);
        iconCanvas->draw16bitRGBBitmapWithTranColor(
            xOffset, 0, level > 10 ? battery : battery_danger, lilka::colors::Fuchsia, 16, 24
        );
        iconCanvas->fillRect(xOffset + x1, y1 + emptyHeight, width, filledHeight, color);
        xOffset += 4 + 16;
        iconCanvas->setCursor(xOffset, 17);
        iconCanvas->print(String(level) + "%");
        xOffset += 36;
    }

    return xOffset;
}
