#include <WiFi.h>

#include "wifi_scan.h"

WifiScanApp::WifiScanApp() : App("WiFi Scanner") {}

void WifiScanApp::run() {
    canvas->fillScreen(canvas->color565(0, 0, 0));
    canvas->setCursor(4, 150);
    queueDraw();

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    canvas->println("Скануємо мережі WiFi...");
    queueDraw();

    int16_t count = WiFi.scanNetworks(false);
    // while (count == WIFI_SCAN_RUNNING) {
    //     delay(250);
    //     lilka::display.print(".");
    //     count = WiFi.scanComplete();
    // }
    String networks[count];
    for (int16_t i = 0; i < count; i++) {
        networks[i] = WiFi.SSID(i);
    }

    // TODO: FreeRTOS experiment
    // lilka::ui_menu(canvas, "Мережі", networks, count, 0);
    lilka::Menu menu("Мережі");
    for (int16_t i = 0; i < count; i++) {
        menu.addItem(networks[i]);
    }
    menu.draw(canvas);
    queueDraw();
    while (1) {
        menu.update();
        menu.draw(canvas);
        queueDraw();
        if (menu.getSelectedIndex() != -1) {
            break;
        }
    }

    // while (!lilka::controller.getState().a.justPressed) {
    //     delay(10);
    // }
}
