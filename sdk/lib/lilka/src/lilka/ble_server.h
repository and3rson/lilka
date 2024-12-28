#pragma once

#include <stdint.h>
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <map>
#include <string>

namespace lilka {

class BLE_Server {
private:
    NimBLEServer* server;
    std::map<std::string, NimBLEService*> services; // UUID to NimBLEService map
    std::map<std::string, NimBLECharacteristic*> characteristics;

public:
    BLE_Server();
    void begin();
    void stop();

    // Dynamic service and characteristic management
    NimBLEService* create_service(const std::string& uuid);
    void delete_service(const std::string& uuid);
    NimBLECharacteristic* create_new_characteristics(
        const std::string& service_uuid, const std::string& characteristics_uuid, uint32_t properties
    );
    void delete_characteristics(const std::string& service_uuid, const std::string& characteristics_uuid);
    void write_characteristics(
        const std::string& service_uuid, const std::string& characteristics_uuid, const std::string& value
    );
    void read_characteristics(const std::string& service_uuid, const std::string& characteristics_uuid);
    void notify_characteristics(
        const std::string& service_uuid, const std::string& characteristics_uuid, const std::string& value
    );
};

extern BLE_Server BLE_server;

} // namespace lilka
