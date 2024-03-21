#pragma once

#include "service.h"

// TODO: Hardcoded timezone
#define MYTZ PSTR("EET-2EEST,M3.5.0/3,M10.5.0/4") // Europe/Kyiv

class ClockService : public Service {
public:
    ClockService();

    struct tm getTime();

private:
    void run() override;
};
