#pragma once

#include "service.h"

class ServiceManager {
public:
    ~ServiceManager();
    void addService(Service* service);

    template <typename T>
    T* getService(const char* name) {
        xSemaphoreTake(xMutex, portMAX_DELAY);
        std::vector<Service*>::iterator it =
            std::find_if(services.begin(), services.end(), [name](const Service* service) {
                return strcmp(service->name, name) == 0;
            });
        if (it != services.end()) {
            xSemaphoreGive(xMutex);
            return static_cast<T*>(*it);
        }
        xSemaphoreGive(xMutex);
        lilka::serial_err("getService(): service %s not found", name);
        return nullptr;
    }

    static ServiceManager* getInstance();

private:
    ServiceManager();
    static ServiceManager* instance;
    std::vector<Service*> services;
    SemaphoreHandle_t xMutex;
};
