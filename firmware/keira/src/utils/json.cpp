#include <Arduino.h>
#include "json.h"

void* SpiRamAllocator::allocate(size_t size) {
    return heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}
void SpiRamAllocator::deallocate(void* pointer) {
    heap_caps_free(pointer);
}
void* SpiRamAllocator::reallocate(void* pointer, size_t size) {
    return heap_caps_realloc(pointer, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
}

SpiRamAllocator spiRamAllocator;
