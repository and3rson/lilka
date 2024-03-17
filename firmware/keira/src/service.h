#pragma once

#include <lilka.h>
#include "appmanager.h"

class Service {
    friend class AppManager;

public:
    Service(const char* name);
    virtual ~Service();
    void start();

protected:
    void setStackSize(uint32_t stackSize);

private:
    static void _run(void* arg);
    virtual void run() = 0;

    const char* name;
    xTaskHandle taskHandle;
    uint32_t stackSize;
};
