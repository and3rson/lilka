#pragma once

#include <stdint.h>
#include <stddef.h>

class Sink {
public:
    virtual void start() = 0;
    virtual size_t write(const int16_t* data, size_t size) = 0;
    virtual void stop() = 0;
    virtual ~Sink() {
    }
};
