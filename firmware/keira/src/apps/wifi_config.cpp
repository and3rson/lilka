#include <WiFi.h>

#include "wifi_config.h"
#include "servicemanager.h"
#include "services/network.h"

#include "icons/wifi_0.h"
#include "icons/wifi_1.h"
#include "icons/wifi_2.h"
#include "icons/wifi_3.h"

WiFiConfigApp::WiFiConfigApp() : App("WiFi") {
}
String WiFiConfigApp::getEncryptionTypeStr(uint8_t encryptionType) {
    switch (encryptionType) {
        case WIFI_AUTH_OPEN:
            return "Open";
        case WIFI_AUTH_WEP:
            return "WEP";
        case WIFI_AUTH_WPA_PSK:
            return "WPA PSK";
        case WIFI_AUTH_WPA2_PSK:
            return "WPA2 PSK";
        case WIFI_AUTH_WPA_WPA2_PSK:
            return "WPA WPA2 PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE:
            return "ENTERPRISE";
        case WIFI_AUTH_WPA3_PSK:
            return "WPA3 PSK";
        case WIFI_AUTH_WPA2_WPA3_PSK:
            return "WPA2 WPA3 PSK";
        case WIFI_AUTH_WAPI_PSK:
            return "WAPI PSK";
        default:
            return "Unknown";
    }
}

void WiFiConfigApp::run() {
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);

    NetworkService* networkService =
        static_cast<NetworkService*>(ServiceManager::getInstance()->getService<NetworkService>("network"));
    // TODO: use dynamic_cast and assert networkService != nullptr

    buffer.fillScreen(lilka::colors::Black);
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
    menu.addActivationButton(lilka::Button::B); // Back
    for (int16_t i = 0; i < count; i++) {
        const int8_t rssi = WiFi.RSSI(i);
        uint8_t signalStrength;
        if (rssi == 0) {
            signalStrength = 0;
        } else {
            const int8_t excellent = -50;
            const int8_t good = -70;
            const int8_t fair = -80;

            if (rssi >= excellent) {
                signalStrength = 3;
            } else if (rssi >= good) {
                signalStrength = 2;
            } else if (rssi >= fair) {
                signalStrength = 1;
            } else {
                signalStrength = 0;
            }
        }
        menu_icon_t* icons[] = {&wifi_0_img, &wifi_1_img, &wifi_2_img, &wifi_3_img};
        menu.addItem(
            networks[i],
            icons[signalStrength],
            networkService->getPassword(networks[i]).length() ? lilka::colors::Green : lilka::colors::White
        );
    }
    menu.addItem("<< Назад");
    menu.addActivationButton(lilka::Button::C);
    count++;
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int cursor = menu.getCursor();
        if (cursor == count - 1 || menu.getButton() == lilka::Button::B) {
            return;
        }
        if (menu.getButton() == lilka::Button::C) {
            int16_t index = menu.getCursor();
            String networkInfo = "Канал: " + String(WiFi.channel(index)) + "\n";
            networkInfo += "Сила сигналу: " + String(WiFi.RSSI(index)) + "db\n";
            networkInfo += "MAC: " + WiFi.BSSIDstr(index) + "\n";
            networkInfo += "Захист: " + getEncryptionTypeStr(WiFi.encryptionType(index)) + "\n";
            lilka::Alert info(networks[menu.getCursor()], networkInfo);
            while (!info.isFinished()) {
                info.update();
                info.draw(canvas);
                queueDraw();
            }
            continue;
        }
        String ssid = networks[cursor];

        String password = "";
        // Check if WiFi network is insecure
        if (WiFi.encryptionType(cursor) == WIFI_AUTH_OPEN) {
            lilka::Alert alert(
                "Увага",
                "Ви під'єднуєтеся до незахищеної мережі " + ssid +
                    "\n\n"
                    "A - продовжити\n"
                    "B - обрати іншу мережу"
            );
            alert.addActivationButton(lilka::Button::B);
            alert.draw(canvas);
            queueDraw();
            while (!alert.isFinished()) {
                alert.update();
            }
            if (alert.getButton() == lilka::Button::B) {
                continue;
            }
        } else {
            lilka::InputDialog passwordDialog("Введіть пароль:");
            passwordDialog.setMasked(true);
            passwordDialog.setValue(networkService->getPassword(ssid));
            while (!passwordDialog.isFinished()) {
                passwordDialog.update();
                passwordDialog.draw(canvas);
                queueDraw();
            }
            password = passwordDialog.getValue();
        }
        networkService->connect(ssid, password);

        buffer.fillScreen(lilka::colors::Black);
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
