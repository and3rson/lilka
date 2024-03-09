#pragma once

#include <WiFi.h>
#include "service.h"

enum NetworkState {
    NETWORK_STATE_OFFLINE,
    NETWORK_STATE_CONNECTING,
    NETWORK_STATE_ONLINE,
};

class NetworkService : public Service {
public:
    NetworkService();
    void configure(String ssid, String password);
    NetworkState getNetworkState();
    int getSignalStrength();

private:
    void run() override;
    void connect();
    NetworkState state;

    String ssid;
    String password;
    int8_t _signalStrength; // Value in range [0,3]
};
