#ifndef AUTOMOUNT_SERVICE_H
#define AUTOMOUNT_SERVICE_H
#include <SPIFFS.h>
#include <SD.h>
#include "service.h"
//#include "serial.h"

// This service should auto mount any avaliable media

#define LILKA_SD_ROOT     "/sd"
#define LILKA_SPIFFS_ROOT "/fs"

// TODO: USB-OTG implementation ?

class AutoMountService : public Service {
private:
    sdcard_type_t sdType;
    bool spiffsAvaliable;
    bool sdAvaliable;

    bool SDInit();
    bool SPIFFSInit();
    void run() override;

public:
    AutoMountService();
};

#endif
