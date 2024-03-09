#include <Preferences.h>

#include "network.h"

NetworkService::NetworkService() : Service("network") {
    state = NETWORK_STATE_OFFLINE;
    _signalStrength = 0;
}

void NetworkService::run() {
    ssid = "";
    password = "";

    Preferences prefs;
    prefs.begin("network", true);
    if (!prefs.isKey("ssid")) {
        Serial.println("network worker: no SSID found in preferences");
        prefs.end();
        return;
    }
    ssid = prefs.getString("ssid");
    password = prefs.getString("password");
    prefs.end();

    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_START:
                Serial.println("network worker: connecting to WiFi");
                state = NETWORK_STATE_CONNECTING;
                break;
            case ARDUINO_EVENT_WIFI_STA_CONNECTED:
                Serial.println("network worker: connected to WiFi");
                state = NETWORK_STATE_ONLINE;
                // Get SSID from info
                // char ssid[33];
                // memcpy(ssid, info.wifi_sta_connected.ssid, info.wifi_sta_connected.ssid_len);
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                Serial.println("network worker: disconnected from WiFi");
                state = NETWORK_STATE_OFFLINE;
                break;
            default:
                break;
        }
    });

    connect();

    while (1) {
        const int8_t rssi = WiFi.RSSI();
        const int8_t excellent = -50;
        const int8_t good = -70;
        const int8_t fair = -80;

        if (rssi >= excellent) {
            _signalStrength = 3;
        } else if (rssi >= good) {
            _signalStrength = 2;
        } else if (rssi >= fair) {
            _signalStrength = 1;
        } else {
            _signalStrength = 0;
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void NetworkService::configure(String ssid, String password) {
    ssid = ssid;
    password = password;
    Preferences prefs;
    prefs.begin("network", false);
    prefs.putString("ssid", ssid);
    prefs.putString("password", password);
    prefs.end();
    connect();
}

NetworkState NetworkService::getNetworkState() {
    return state;
}

int NetworkService::getSignalStrength() {
    return _signalStrength;
}

void NetworkService::connect() {
    state = NETWORK_STATE_CONNECTING;
    WiFi.begin(ssid.c_str(), password.c_str());
}
