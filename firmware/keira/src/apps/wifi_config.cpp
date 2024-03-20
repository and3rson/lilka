#include <WiFi.h>

#include "wifi_config.h"
#include "servicemanager.h"
#include "services/network.h"

WiFiConfigApp::WiFiConfigApp() : App("WiFi") {
}

void WiFiConfigApp::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);

    NetworkService* networkService =
        static_cast<NetworkService*>(ServiceManager::getInstance()->getService<NetworkService>());
    // TODO: use dynamic_cast and assert networkService != nullptr

    buffer.fillScreen(buffer.color565(0, 0, 0));
    buffer.setCursor(8, 24);
    canvas->drawCanvas(&buffer);
    queueDraw();

    // This has to be done before scanning since WiFi is sometimes in invalid state and scanNetworks() returns -2 (WIFI_SCAN_FAILED)
    // There's a better way to do this, but I don't have time to figure it out...
    WiFi.disconnect();
    if (WiFi.getMode() != WIFI_STA) {
        WiFi.mode(WIFI_STA);
    }
    // WiFi.disconnect();

    buffer.println("Скануємо мережі WiFi...");
    canvas->drawCanvas(&buffer);
    queueDraw();

    int16_t count = WiFi.scanNetworks(false);
    if (count < 0) {
        lilka::Alert alert("Помилка", "Не вдалося сканувати мережі, код помилки: " + String(count));
        alert.draw(canvas);
        queueDraw();
        while (!alert.isFinished()) {
            alert.update();
        }
        return;
    }
    // while (count == WIFI_SCAN_RUNNING) {
    //     delay(250);
    //     lilka::display.print(".");
    //     count = WiFi.scanComplete();
    // }
    String networks[count];
    for (int16_t i = 0; i < count; i++) {
        networks[i] = WiFi.SSID(i);
    }

    lilka::Menu menu("Мережі");
    for (int16_t i = 0; i < count; i++) {
        // TODO: Add security (or signal strength) icon
        menu.addItem(
            networks[i],
            0,
            networkService->getPassword(networks[i]).length() ? lilka::display.color565(0, 255, 0)
                                                              : lilka::display.color565(255, 255, 255)
        );
    }
    menu.addItem("<< Назад");
    count++;
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int cursor = menu.getCursor();
        if (cursor == count - 1) {
            return;
        }

        String ssid = networks[cursor];

        lilka::InputDialog passwordDialog("Введіть пароль:");
        passwordDialog.setMasked(true);
        passwordDialog.setValue(networkService->getPassword(ssid));
        while (!passwordDialog.isFinished()) {
            passwordDialog.update();
            passwordDialog.draw(canvas);
            queueDraw();
        }
        String password = passwordDialog.getValue();
        networkService->connect(ssid, password);

        buffer.fillScreen(buffer.color565(0, 0, 0));
        buffer.setCursor(8, 24);
        buffer.println("Під'єднуємось...");
        canvas->drawCanvas(&buffer);
        queueDraw();

        // // Wait for the adapter to start connecting
        // while (networkService->getNetworkState() != NETWORK_STATE_CONNECTING &&
        //        networkService->getNetworkState() != NETWORK_STATE_ONLINE) {
        //     taskYIELD();
        // }
        // // Wait for the adapter to finish connecting
        // while (networkService->getNetworkState() == NETWORK_STATE_CONNECTING) {
        //     taskYIELD();
        // }
        // Wait for the adapter to start connecting (it can briefly enter DISCONNECTED state)
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // Wait for the adapter to finish connecting
        while (networkService->getNetworkState() == NETWORK_STATE_CONNECTING) {
            taskYIELD();
        }

        lilka::Alert alert("", "");
        bool success = networkService->getNetworkState() == NETWORK_STATE_ONLINE;
        if (success) {
            alert.setTitle("Успіх");
            alert.setMessage("Під'єднано до мережі " + ssid);
        } else {
            alert.setTitle("Помилка");
            alert.setMessage("Не вдалося під'єднатись до мережі " + ssid);
        }

        alert.draw(canvas);
        queueDraw();
        while (!alert.isFinished()) {
            alert.update();
        }

        if (success) {
            return;
        }
    }
}
