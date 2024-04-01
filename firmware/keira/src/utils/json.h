#pragma once

#include <ArduinoJson.h>

struct SpiRamAllocator : ArduinoJson::Allocator {
    void* allocate(size_t size);
    void deallocate(void* pointer);
    void* reallocate(void* pointer, size_t size);
};

// Custom allocator for ArduinoJson to use SPI RAM.
// Usage: JsonDocument doc(&spiRamAllocator);
extern SpiRamAllocator spiRamAllocator;
