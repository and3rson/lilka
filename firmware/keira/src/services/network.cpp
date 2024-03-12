#include <Preferences.h>

#include "network.h"

// EEPROM preferences used:
// - network.last_ssid - last connected SSID
// - network.[SSID]_password - password of known network with a given SSID

NetworkService::NetworkService() : Service("network") {
    state = NETWORK_STATE_OFFLINE;
    signalStrength = 0;
    mutex = xSemaphoreCreateMutex();
}

void NetworkService::run() {
    Preferences prefs;

    // Check if there is a known network to connect to
    prefs.begin("network", true);
    if (!prefs.isKey("last_ssid")) {
        Serial.println("network worker: no last SSID found, skipping startup connection");
    } else {
        String currentSSID = prefs.getString("last_ssid");
        String currentPassword = prefs.getString(String(currentSSID + "_password").c_str());
        connect(currentSSID, currentPassword);
    }
    prefs.end();

    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_START: {
                Serial.println("network worker: connecting to WiFi");
                state = NETWORK_STATE_CONNECTING;
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_CONNECTED: {
                Serial.println("network worker: connected to WiFi");
                state = NETWORK_STATE_ONLINE;
                Preferences prefs;
                String connectedSSID = String(info.wifi_sta_connected.ssid, info.wifi_sta_connected.ssid_len);
                prefs.begin("network", true);
                if (!String(prefs.getString("last_ssids")).equals(connectedSSID)) {
                    // Set current SSID as last connected
                    prefs.putString("last_ssid", String(connectedSSID));
                    Serial.println("network worker: last SSID set to " + connectedSSID);
                }
                if (!prefs.isKey(String(connectedSSID + "_password").c_str())) {
                    // Save password for the connected network
                    prefs.putString(String(connectedSSID + "_password").c_str(), connectedSSID);
                    Serial.println("network worker: password for " + connectedSSID + " saved");
                }
                prefs.end();
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
                Serial.println("network worker: disconnected from WiFi");
                state = NETWORK_STATE_OFFLINE;
                break;
            }
            default:
                break;
        }
    });

    while (1) {
        const int8_t rssi = WiFi.RSSI();
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

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// bool NetworkService::connect(String ssid, String password) {
//     ssid = ssid;
//     password = password;
//     Preferences prefs;
//     prefs.begin("network", false);
//     prefs.putString("ssid", ssid);
//     prefs.putString("password", password);
//     prefs.end();
//     connect();
// }

NetworkState NetworkService::getNetworkState() {
    return state;
}

int NetworkService::getSignalStrength() {
    return signalStrength;
}

// Attempt to connect to a given network.
// If the network is not known (password is required), return false.
bool NetworkService::connect(String ssid) {
    Preferences prefs;
    prefs.begin("network", false);
    if (!prefs.isKey(String(ssid + "_password").c_str())) {
        Serial.println("network worker: no password found for SSID " + ssid);
        prefs.end();
        return false;
    }
    prefs.end();
    // connect(ssid, prefs.getString(String(ssid + "_password").c_str()));
    Serial.println("network worker: found password for SSID " + ssid);
    connect(ssid, prefs.getString(String(ssid + "_password").c_str()));
    return true;
}

// Attempt to connect to a given network with a given password.
void NetworkService::connect(String ssid, String password) {
    Serial.println("network worker: connecting to " + ssid);
    state = NETWORK_STATE_CONNECTING;
    WiFi.begin(ssid.c_str(), password.c_str());
}
