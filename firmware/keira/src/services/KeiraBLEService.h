#pragma once

#include "service.h"

// BLE, BLEService, BLEServer reserved by another files!

class KeiraBLEService : public Service {
public:
    KeiraBLEService();

private:
    void run() override;
};
