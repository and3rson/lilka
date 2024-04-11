#pragma once

#include <FreeRTOS.h>
#include <semphr.h>

class Acquire {
public:
    explicit Acquire(SemaphoreHandle_t semaphore, bool recursive = false);
    ~Acquire();

private:
    SemaphoreHandle_t _semaphore;
    bool recursive;
};
