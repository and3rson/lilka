#include <EscapeCodes.h>

#include "telnet.h"
#include "servicemanager.h"
#include "network.h"

#define STR_HELPER(x) #x
#define STR(x)        STR_HELPER(x)

ESPTelnet TelnetService::telnet;
EscapeCodes ansi;

TelnetService::TelnetService() : Service("telnet") {
}

TelnetService::~TelnetService() {
}

void TelnetService::run() {
    NetworkService* network = ServiceManager::getInstance()->getService<NetworkService>("network");

    telnet.onConnectionAttempt([](String ip) {
        lilka::serial_log("TelnetService: %s attempting to connect", ip.c_str());
    });
    telnet.onConnect([](String ip) {
        lilka::serial_log("TelnetService: %s connected", ip.c_str());
        telnet.print(ansi.cls());
        telnet.print(ansi.home());
        telnet.println(ansi.setBG(ANSI_BLUE) + " " + ansi.reset() + " Keira OS @ Lilka v" STR(LILKA_VERSION));
        telnet.println(ansi.setBG(ANSI_YELLOW) + " " + ansi.reset() + " Слава Україні! ");
        telnet.print("> ");
    });
    telnet.onReconnect([](String ip) { lilka::serial_log("TelnetService: %s reconnected", ip.c_str()); });
    telnet.onDisconnect([](String ip) { lilka::serial_log("TelnetService: %s disconnected", ip.c_str()); });
    telnet.onInputReceived([](String input) { lilka::serial_log("TelnetService: received text: %s", input.c_str()); });

    bool wasOnline = false;
    while (1) {
        if (!network) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        bool isOnline = network->getNetworkState() == NetworkState::NETWORK_STATE_ONLINE;
        if (!wasOnline && isOnline) {
            wasOnline = true;
            // Start telnet server
        } else if (wasOnline && !isOnline) {
            wasOnline = false;
            // Stop telnet server
        }
    }
}
