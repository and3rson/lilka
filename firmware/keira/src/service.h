#pragma once

#include <lilka.h>

class Service {
public:
    Service(const char* name);
    virtual ~Service();
    void start();
    static void _run(void* arg);

private:
    virtual void run() = 0;

    const char* name;
    xTaskHandle taskHandle;
};
