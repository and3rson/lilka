#include "servicemanager.h"

ServiceManager* ServiceManager::instance = NULL;

ServiceManager::ServiceManager() : xMutex(xSemaphoreCreateMutex()) {
    xSemaphoreGive(xMutex);
}

ServiceManager::~ServiceManager() {
    // TODO: Should never be destroyed
}

ServiceManager* ServiceManager::getInstance() {
    // TODO: Not thread-safe, but is first called in static context before any tasks are created
    if (instance == NULL) {
        instance = new ServiceManager();
    }
    return instance;
}

void ServiceManager::addService(Service* service) {
    xSemaphoreTake(xMutex, portMAX_DELAY);
    services.push_back(service);
    service->start();
    xSemaphoreGive(xMutex);
}
