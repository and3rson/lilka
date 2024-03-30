#pragma once
#include "service.h"
#include "appmanager.h"
//#include "serial.h"

// This service should auto mount any avaliable media

// TODO: USB-OTG implementation ?

class AutoMountService : public Service {
private:
    void SDInit();
    void run() override;

public:
    AutoMountService();
};
