#pragma once

#include "service.h"

class ServiceManager {
public:
    ~ServiceManager();
    void addService(Service* service);

    template <typename T>
    T* getService() {
        for (Service* service : services) {
            T* t = static_cast<T*>(service);
            if (t != nullptr) {
                return t;
            }
        }
        return nullptr;
    }

    static ServiceManager* getInstance();

private:
    ServiceManager();
    static ServiceManager* instance;
    std::vector<Service*> services;
};
