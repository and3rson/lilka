#include <Preferences.h>

#include "network.h"

// EEPROM preferences used:
// - network.last_ssid - last connected SSID
// - network.[SSID_hash]_pw - password of known network with a given SSID

NetworkService::NetworkService() :
    Service("network"),
    state(NETWORK_STATE_OFFLINE),
    reason(0),
    signalStrength(0),
    mutex(xSemaphoreCreateMutex()),
    lastPassword(""),
    ipAddr("") {
    // TODO: Use the mutex, Luke!
}

void NetworkService::run() {
    Preferences prefs;
    // Setting Lilka hostname
    // Take LILKA_HOSTNAME_PREFIX as a prefix
    // and append MAC to it
    // This value should be guaranted random enough
    // to avoid potential conflicts
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char cstrMac[50];
    sprintf(cstrMac, LILKA_HOSTNAME_PREFIX STR(LILKA_VERSION) "_%06X", mac);
    WiFi.setHostname(cstrMac);
    WiFi.mode(WIFI_STA);

    // Check if there is a known network to connect to
    prefs.begin("network", true);
    if (!prefs.isKey("last_ssid")) {
        Serial.println("NetworkService: no last SSID found, skipping startup connection");
    } else {
        String currentSSID = prefs.getString("last_ssid");
        Serial.println("NetworkService: last SSID found: " + currentSSID);
        lastPassword = getPassword(currentSSID);
        if (lastPassword == "") {
            Serial.println("NetworkService: no password found for last SSID, skipping startup connection");
        } else {
            connect(currentSSID, lastPassword);
        }
    }
    prefs.end();

    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_START: {
                Serial.println("NetworkService: connecting to WiFi");
                setNetworkState(NETWORK_STATE_CONNECTING);
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_CONNECTED: {
                Serial.println("NetworkService: connected to WiFi");
                setNetworkState(NETWORK_STATE_ONLINE);
                Preferences prefs;
                String connectedSSID = String(info.wifi_sta_connected.ssid, info.wifi_sta_connected.ssid_len);
                prefs.begin("network", false);
                if (!prefs.isKey("last_ssid") || !String(prefs.getString("last_ssid")).equals(connectedSSID)) {
                    // Set current SSID as last connected
                    prefs.putString("last_ssid", String(connectedSSID));
                    Serial.println("NetworkService: last SSID set to " + connectedSSID);
                }
                prefs.end();
                String ssidHash = hash(connectedSSID);
                String savedPassword = getPassword(connectedSSID);
                prefs.begin("network", false);
                if (savedPassword != lastPassword) {
                    // Save password for the connected network
                    prefs.putString(String(ssidHash + "_pw").c_str(), lastPassword);
                    Serial.println("NetworkService: password for " + connectedSSID + " saved");
                }
                prefs.end();
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
                Serial.println(
                    "NetworkService: disconnected from WiFi, reason " + String(info.wifi_sta_disconnected.reason)
                );
                setNetworkState(NETWORK_STATE_OFFLINE);
                reason = info.wifi_sta_disconnected.reason;
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            case ARDUINO_EVENT_WIFI_STA_GOT_IP6: {
                IPAddress ip = WiFi.localIP();
                ipAddr = ip.toString();
                Serial.println("NetworkService: got IP address: " + ipAddr);
                setNetworkState(NETWORK_STATE_ONLINE);
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_LOST_IP: {
                Serial.println("NetworkService: lost IP address");
                ipAddr = "";
                setNetworkState(NETWORK_STATE_OFFLINE);
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
    String password = getPassword(ssid);
    if (password == "") {
        Serial.println("NetworkService: no password found for SSID " + ssid);
        return false;
    }
    Serial.println("NetworkService: found password for SSID " + ssid);
    connect(ssid, password);
    return true;
}

// Attempt to connect to a given network with a given password.
void NetworkService::connect(String ssid, String password) {
    Serial.println("NetworkService: connecting to " + ssid);
    lastPassword = password;
    WiFi.disconnect();
    WiFi.begin(ssid.c_str(), password.c_str());
}

String NetworkService::getPassword(String ssid) {
    Preferences prefs;
    prefs.begin("network", true);
    String ssidHash = hash(ssid);
    String result;
    if (!prefs.isKey(String(ssidHash + "_pw").c_str())) {
        result = "";
    } else {
        result = prefs.getString(String(ssidHash + "_pw").c_str());
    }
    prefs.end();
    return result;
}

String NetworkService::hash(String input) {
    // Calculate simple hash of the input and truncate it to 8 hex characters

    uint64_t hash = 0;
    for (int i = 0; i < input.length(); i++) {
        hash = (hash << 5) - hash + input[i];
    }

    char buffer[9];
    snprintf(buffer, sizeof(buffer), "%08x", (unsigned int)hash);
    return String(buffer);
}

String NetworkService::getIpAddr() {
    return ipAddr;
}

void NetworkService::setNetworkState(NetworkState state) {
    // xSemaphoreTake(mutex, portMAX_DELAY);
    if (this->state != state) {
        this->state = state;
        if (state == NETWORK_STATE_OFFLINE) {
            AppManager::getInstance()->startToast("WiFi втрачено", 2000);
        } else if (state == NETWORK_STATE_ONLINE) {
            AppManager::getInstance()->startToast("Приєднано до WiFi", 2000);
        }
    }
    // xSemaphoreGive(mutex);
}
