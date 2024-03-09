#include <WiFi.h>

#include "wifi_scan.h"

WifiScanApp::WifiScanApp() : App("WiFi Scanner") {}

void WifiScanApp::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);

    buffer.fillScreen(buffer.color565(0, 0, 0));
    buffer.setCursor(4, 150);
    canvas->drawCanvas(&buffer);
    queueDraw();

    if (WiFi.getMode() != WIFI_STA) {
        WiFi.mode(WIFI_STA);
    }
    // WiFi.disconnect();

    buffer.println("Скануємо мережі WiFi...");
    canvas->drawCanvas(&buffer);
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
