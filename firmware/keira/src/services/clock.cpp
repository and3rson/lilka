#include "clock.h"
#include "network.h"
#include "servicemanager.h"

ClockService::ClockService() : Service("clock") {
}

void ClockService::run() {
    while (1) {
        NetworkService* network = ServiceManager::getInstance()->getService<NetworkService>("network");
        if (network == NULL) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        if (network->getNetworkState() == NetworkState::NETWORK_STATE_ONLINE) {
            lilka::serial.log("ClockService: Setting time from NTP server");
            configTzTime(MYTZ, "ua.pool.ntp.org", "pool.ntp.org");
            // Delay for 12 hours
            vTaskDelay(1000 * 60 * 60 * 12 / portTICK_PERIOD_MS);
        } else {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

struct tm ClockService::getTime() {
    struct timeval tv;
    struct tm timeinfo;
    gettimeofday(&tv, NULL);
    time_t now = tv.tv_sec;
    localtime_r(&now, &timeinfo);
    return timeinfo;
}
