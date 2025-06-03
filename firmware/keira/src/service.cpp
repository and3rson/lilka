#include "service.h"

Service::Service(const char* name) : name(name), taskHandle(NULL), stackSize(8192) {
}

Service::~Service() {
}

void Service::start() {
    lilka::serial.log(K_S_SERVICE_STARTUP, name);
    xTaskCreate(_run, name, stackSize, this, 1, &taskHandle);
}

void Service::_run(void* arg) {
    Service* service = static_cast<Service*>(arg);
    service->run();
    lilka::serial.err(K_S_SERVICE_DIE, service->name);
}

void Service::setStackSize(uint32_t stackSize) {
    this->stackSize = stackSize;
}
