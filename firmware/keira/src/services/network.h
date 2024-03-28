#pragma once

#include <WiFi.h>
#include "service.h"
// Macro magic used to convert decimal constant to char[] constant
#define STRX(x)               #x
#define STR(x)                STRX(x)
#define LILKA_HOSTNAME_PREFIX "LilkaV"
enum NetworkState {
    NETWORK_STATE_OFFLINE,
    NETWORK_STATE_CONNECTING,
    NETWORK_STATE_ONLINE,
};

class NetworkService : public Service {
public:
    NetworkService();
    // void configure(String ssid, String password);
    NetworkState getNetworkState();
    int getSignalStrength();
    bool connect(String ssid);
    void connect(String ssid, String password);

    String getPassword(String ssid);
    String getIpAddr();

private:
    void run() override;
    String hash(String input);
    void setNetworkState(NetworkState state);

    SemaphoreHandle_t mutex;
    NetworkState state;
    int reason;
    int8_t signalStrength; // Value in range [0,3]
    String lastPassword;
    String ipAddr;
};
