#include <lilka.h>
#include <WiFi.h>

void demo_scan_wifi(lilka::Canvas *canvas) {
    canvas->fillScreen(canvas->color565(0, 0, 0));
    canvas->setCursor(4, 150);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    canvas->println("Скануємо мережі WiFi...");

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
    lilka::ui_menu(canvas, "Мережі", networks, count, 0);

    // while (!lilka::controller.getState().a.justPressed) {
    //     delay(10);
    // }
}
