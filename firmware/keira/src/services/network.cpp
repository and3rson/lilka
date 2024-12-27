// TODO: Add enable/disable methods instead of deallocating WiFi from apps like LilTracker
// TODO: Use the mutex, Luke!

#include <Preferences.h>
#include <esp_wifi.h>

#include "network.h"

// Macro magic used to convert decimal constant to char[] constant
#define STRX(x)               #x
#define STR(x)                STRX(x)
#define LILKA_HOSTNAME_PREFIX "LilkaV"

// EEPROM preferences used:
// - keira.last_ssid - last connected SSID
// - keira.[SSID_hash]_pw - password of known network with a given SSID

NetworkService::NetworkService() :
    Service("network"),
    state(NETWORK_STATE_OFFLINE),
    reason(0),
    signalStrength(0),
    mutex(xSemaphoreCreateMutex()),
    lastPassword(""),
    ipAddr("") {
    // TODO: Use the mutex, Luke!
    xSemaphoreGive(mutex);
}

void NetworkService::run() {
    Preferences prefs;
    prefs.begin("keira", true);
    bool enabled = prefs.isKey("enabled") ? prefs.getBool("enabled") : false;
    prefs.end();

    WiFi.onEvent([this](WiFiEvent_t event, WiFiEventInfo_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_START: {
                lilka::serial_log("NetworkService: got event: connecting to WiFi");
                setNetworkState(NETWORK_STATE_CONNECTING);
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_CONNECTED: {
                lilka::serial_log("NetworkService: got event: connected to WiFi");
                setNetworkState(NETWORK_STATE_ONLINE);
                Preferences prefs;
                String connectedSSID = String(info.wifi_sta_connected.ssid, info.wifi_sta_connected.ssid_len);
                prefs.begin("keira", false);
                if (!prefs.isKey("last_ssid") || !String(prefs.getString("last_ssid")).equals(connectedSSID)) {
                    // Set current SSID as last connected
                    prefs.putString("last_ssid", String(connectedSSID));
                    lilka::serial_log("NetworkService: last SSID set to  %s", connectedSSID.c_str());
                }
                prefs.end();
                String ssidHash = hash(connectedSSID);
                String savedPassword = getPassword(connectedSSID);
                prefs.begin("keira", false);
                if (savedPassword != lastPassword) {
                    // Save password for the connected network
                    prefs.putString(String(ssidHash + "_pw").c_str(), lastPassword);
                    lilka::serial_log("NetworkService: password for %s saved", connectedSSID.c_str());
                }
                prefs.end();
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
                lilka::serial_log(
                    "NetworkService: got event: disconnected from WiFi, reason: %d", info.wifi_sta_disconnected.reason
                );
                setNetworkState(NETWORK_STATE_OFFLINE);
                reason = info.wifi_sta_disconnected.reason;
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            case ARDUINO_EVENT_WIFI_STA_GOT_IP6: {
                IPAddress ip = WiFi.localIP();
                ipAddr = ip.toString();
                lilka::serial_log("NetworkService: got event: got IP address: %s", ipAddr.c_str());
                setNetworkState(NETWORK_STATE_ONLINE);
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_LOST_IP: {
                lilka::serial_log("NetworkService: got event: lost IP address");
                ipAddr = "";
                setNetworkState(NETWORK_STATE_OFFLINE);
                break;
            }
            case ARDUINO_EVENT_WIFI_STA_STOP: {
                lilka::serial_log("NetworkService: got event: WiFi stopped");
                setNetworkState(NETWORK_STATE_DISABLED);
                break;
            }
            default:
                break;
        }
    });

    if (enabled) {
        lilka::serial_log("NetworkService: WiFi is enabled, starting auto connection");
        setNetworkState(NETWORK_STATE_OFFLINE);
        WiFi.mode(WIFI_STA);
        autoConnect();
    } else {
        lilka::serial_log("NetworkService: WiFi is disabled, not starting auto connection");
        setNetworkState(NETWORK_STATE_DISABLED);
        WiFi.disconnect(true, true);
        WiFi.mode(WIFI_OFF);
    }

    while (1) {
        // Check if WiFi is deallocated
        wifi_mode_t mode;
        if (esp_wifi_get_mode(&mode) == ESP_ERR_WIFI_NOT_INIT) {
            // WiFi was deallocated
            // TODO: This is a crutch to avoid using WiFi after deallocation by apps (e. g. LilTracker). /AD
            // lilka::serial_log("NetworkService: WiFi deallocated");
        } else if (WiFi.status() == WL_DISCONNECTED) {
            // WiFi is disconnected
            // lilka::serial_log("NetworkService: WiFi disconnected");
        } else {
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
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void NetworkService::autoConnect() {
    // Setting Lilka hostname
    // Take LILKA_HOSTNAME_PREFIX as a prefix and append MAC to it
    // This value should be random enough to avoid potential conflicts
    uint8_t mac[6];
    WiFi.macAddress(mac);
    char cstrMac[50];
    sprintf(cstrMac, LILKA_HOSTNAME_PREFIX STR(LILKA_VERSION) "_%02X%02X%02X", mac[3], mac[4], mac[5]);
    WiFi.setHostname(cstrMac);
    WiFi.mode(WIFI_STA);

    // Check if there is a known network to connect to
    Preferences prefs;
    prefs.begin("keira", true);
    if (!prefs.isKey("last_ssid")) {
        lilka::serial_log("NetworkService: no last SSID found, skipping auto connection");
    } else {
        String currentSSID = prefs.getString("last_ssid");
        lilka::serial_log("NetworkService: last SSID found: %s", currentSSID.c_str());
        lastPassword = getPassword(currentSSID);
        if (lastPassword == "") {
            lilka::serial_log("NetworkService: no password found for last SSID, skipping auto connection");
        } else {
            connect(currentSSID, lastPassword);
        }
    }
    prefs.end();
}

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
        lilka::serial_log("NetworkService: no password found for SSID %s", ssid.c_str());
        return false;
    }
    lilka::serial_log("NetworkService: found password for SSID ", ssid.c_str());
    connect(ssid, password);
    return true;
}

// Attempt to connect to a given network with a given password.
void NetworkService::connect(String ssid, String password) {
    lilka::serial_log("NetworkService: connecting to %s", ssid.c_str());
    lastPassword = password;
    WiFi.disconnect();
    WiFi.begin(ssid.c_str(), password.c_str());
}

bool NetworkService::getEnabled() {
    return state != NETWORK_STATE_DISABLED;
}

void NetworkService::setEnabled(bool enabled) {
    Preferences prefs;
    prefs.begin("keira", false);
    prefs.putBool("enabled", enabled);
    prefs.end();

    lilka::serial_log("NetworkService: WiFi set to %s", enabled ? "enabled" : "disabled");

    if (enabled) {
        setNetworkState(NETWORK_STATE_OFFLINE);
        WiFi.mode(WIFI_STA);
        autoConnect();
    } else {
        WiFi.disconnect(true, true);
        WiFi.mode(WIFI_OFF);
    }
}

String NetworkService::getPassword(String ssid) {
    Preferences prefs;
    prefs.begin("keira", true);
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
    if (this->state != state) {
        this->state = state;
        if (state == NETWORK_STATE_OFFLINE) {
            AppManager::getInstance()->startToast("WiFi втрачено", 2000);
        } else if (state == NETWORK_STATE_ONLINE) {
            AppManager::getInstance()->startToast("Приєднано до WiFi", 2000);
        }
    }
}
