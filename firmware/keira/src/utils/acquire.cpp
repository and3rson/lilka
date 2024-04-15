#include "acquire.h"

Acquire::Acquire(SemaphoreHandle_t semaphore, bool recursive) : _semaphore(semaphore), recursive(recursive) {
    if (recursive) {
        xSemaphoreTakeRecursive(_semaphore, portMAX_DELAY);
        return;
    }
    xSemaphoreTake(_semaphore, portMAX_DELAY);
}

Acquire::~Acquire() {
    if (recursive) {
        xSemaphoreGiveRecursive(_semaphore);
        return;
    }
    xSemaphoreGive(_semaphore);
}
