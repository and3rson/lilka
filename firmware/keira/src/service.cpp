#include "service.h"

Service::Service(const char* name) : name(name), taskHandle(NULL) {
}

Service::~Service() {
}

void Service::start() {
    Serial.println("Starting service " + String(name));
    xTaskCreate(_run, name, 16384, this, 1, &taskHandle);
}

void Service::_run(void* arg) {
    Service* service = (Service*)arg;
    service->run();
    Serial.println("Service " + String(service->name) + " died");
}
