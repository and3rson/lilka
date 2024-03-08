#include "statusbar.h"
#include "icons/battery.h"
#include "icons/wifi.h"

StatusBarApp::StatusBarApp() : App("StatusBar", 0, 0, LILKA_DISPLAY_WIDTH, 24) {}

void StatusBarApp::run() {
    int counter = 0;
    while (1) {
        canvas->fillScreen(lilka::display.color565(0, 0, 0));
        canvas->setTextColor(lilka::display.color565(255, 255, 255), lilka::display.color565(0, 0, 0));
        canvas->setFont(FONT_10x20);
        canvas->setCursor(32, 20);
        canvas->print("Time: " + String(counter++));
        canvas->draw16bitRGBBitmapWithTranColor(136, 0, wifi, 0, 24, 24);
        canvas->draw16bitRGBBitmapWithTranColor(136 + 24, 0, battery, 0, 48, 24);
        // canvas->draw16bitRGBBitmapWithTranColor(160, 0, wifi, 24, 24, 0);
        // canvas->draw16bitRGBBitmapWithTranColor(160 + 24, 0, battery, 24, 24, 0);
        queueDraw();
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
