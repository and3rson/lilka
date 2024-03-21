#pragma once

#include <lilka.h>
#include "appmanager.h"

class Service {
    friend class AppManager;
    friend class ServiceManager;

public:
    Service(const char* name);
    virtual ~Service();
    void start();

protected:
    void setStackSize(uint32_t stackSize);
    const char* name;

private:
    static void _run(void* arg);
    virtual void run() = 0;

    xTaskHandle taskHandle;
    uint32_t stackSize;
};
