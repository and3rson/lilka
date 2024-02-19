#include <lilka.h>
#include <WiFi.h>

void demo_scan_wifi() {
    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
    lilka::display.setCursor(4, 150);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    lilka::display.println("Скануємо мережі WiFi...");

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

    lilka::ui_menu("Мережі", networks, count, 0);

    // while (!lilka::controller.getState().a.justPressed) {
    //     delay(10);
    // }
}
