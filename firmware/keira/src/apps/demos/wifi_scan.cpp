#include <WiFi.h>

#include "wifi_scan.h"
#include "servicemanager.h"
#include "services/network.h"

WifiScanApp::WifiScanApp() : App("WiFi Scanner") {
}

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

    lilka::Menu menu("Мережі");
    for (int16_t i = 0; i < count; i++) {
        menu.addItem(networks[i]);
    }
    menu.addItem("<< Назад");
    count++;
    while (1) {
        int index = -1;
        while (index == -1) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
            index = menu.getSelectedIndex();
        }
        if (index == count - 1) {
            return;
        }

        String ssid = networks[index];

        // Attempt to connect to the selected network
        NetworkService* networkService = (NetworkService*)ServiceManager::getInstance()->getService<NetworkService>();
        // TODO: assert networkService != nullptr
        if (!networkService->connect(ssid)) {
            // Password required
            lilka::InputDialog passwordDialog("Введіть пароль:");
            passwordDialog.setMasked(true);
            while (!passwordDialog.isDone()) {
                passwordDialog.update();
                passwordDialog.draw(canvas);
                queueDraw();
            }
            String password = passwordDialog.getValue();
            networkService->connect(ssid, password);
        }

        buffer.fillScreen(buffer.color565(0, 0, 0));
        buffer.setCursor(4, 150);
        buffer.println("Під'єднуємось...");
        canvas->drawCanvas(&buffer);
        queueDraw();

        // Wait for the network to connect or fail
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
        while (!alert.isDone()) {
            alert.update();
        }

        if (success) {
            return;
        }
    }
}
