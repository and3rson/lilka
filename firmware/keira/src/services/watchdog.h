#pragma once

#include <WiFi.h>
#include "service.h"

class WatchdogService : public Service {
public:
    WatchdogService();

private:
    void run() override;
};
