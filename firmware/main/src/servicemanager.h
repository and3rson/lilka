#pragma once

#include "service.h"

class ServiceManager {
public:
    ~ServiceManager();
    void addService(Service *service);

    template <typename T>
    T *getService();

    static ServiceManager *getInstance();

private:
    ServiceManager();
    static ServiceManager *instance;
    std::vector<Service *> services;
};
