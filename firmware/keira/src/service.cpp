#include "service.h"

Service::Service(const char* name) : name(name), taskHandle(NULL), stackSize(8192) {
}

Service::~Service() {
}

void Service::start() {
    Serial.println("Starting service " + String(name));
    xTaskCreate(_run, name, stackSize, this, 1, &taskHandle);
}

void Service::_run(void* arg) {
    Service* service = static_cast<Service*>(arg);
    service->run();
    Serial.println("Service " + String(service->name) + " died");
}

void Service::setStackSize(uint32_t stackSize) {
    this->stackSize = stackSize;
}
