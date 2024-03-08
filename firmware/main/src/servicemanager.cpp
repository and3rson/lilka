#include "servicemanager.h"

ServiceManager *ServiceManager::instance = NULL;

ServiceManager::ServiceManager() {}

ServiceManager::~ServiceManager() {
    // TODO: Should never be destroyed
}

ServiceManager *ServiceManager::getInstance() {
    // TODO: Not thread-safe, but is first called in static context before any tasks are created
    if (instance == NULL) {
        instance = new ServiceManager();
    }
    return instance;
}

void ServiceManager::addService(Service *service) {
    services.push_back(service);
}

template <typename T>
T *ServiceManager::getService() {
    for (Service *service : services) {
        if (dynamic_cast<T *>(service) != NULL) {
            return dynamic_cast<T *>(service);
        }
    }
    return NULL;
}