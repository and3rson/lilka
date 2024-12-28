#include "ble_server.h"

namespace lilka {

// Constructor
BLE_Server::BLE_Server() {
    NimBLEDevice::init("Lilka_BLE_Server");
    server = NimBLEDevice::createServer();
}

static void start_advertising(void* pvParameters) {
    // This function will run in the FreeRTOS task
    Serial.println("Advertising started");
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->start();
    Serial.println("Advertising finished");
    vTaskDelete(NULL); // Delete the task after advertising starts
}

// Start the BLE server
void BLE_Server::begin() {
    for (const auto& servicePair : services) {
        const auto& key = servicePair.first;
        const auto& service = servicePair.second;
    }

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    xTaskCreate(start_advertising, "StartAdvertising", 2048, NULL, 1, NULL);

    Serial.println("BLE Server started");
}

// Stop the BLE server
void BLE_Server::stop() {
    NimBLEDevice::stopAdvertising();
    NimBLEDevice::deinit();
    Serial.println("BLE Server stopped");
}

// Create a new service dynamically
NimBLEService* BLE_Server::create_service(const std::string& uuid) {
    if (services.find(uuid) != services.end()) {
        Serial.printf("Service with UUID %s already exists\n", uuid.c_str());
        return nullptr;
    }

    NimBLEService* service = server->createService(uuid);
    services[uuid] = service;
    service->start();
    Serial.printf("Service %s created\n", uuid.c_str());
    return service;
}

// Delete a service dynamically
void BLE_Server::delete_service(const std::string& uuid) {
    auto it = services.find(uuid);
    if (it != services.end()) {
        server->removeService(it->second);
        services.erase(it);
        Serial.printf("Service %s deleted\n", uuid.c_str());
    } else {
        Serial.printf("Service %s not found\n", uuid.c_str());
    }
}

// Create a new characteristic dynamically
NimBLECharacteristic* BLE_Server::create_new_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, uint32_t properties
) {
    auto service_it = services.find(service_uuid);
    if (service_it == services.end()) {
        Serial.printf("Service %s not found\n", service_uuid.c_str());
        return nullptr;
    }

    NimBLEService* service = service_it->second;
    NimBLECharacteristic* characteristic = service->createCharacteristic(char_uuid, properties);

    if (!characteristic) {
        Serial.printf("Failed to create characteristic %s in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        return nullptr;
    }

    Serial.printf("Characteristic %s created in service %s\n", char_uuid.c_str(), service_uuid.c_str());
    return characteristic;
}

// Delete a characteristic dynamically
void BLE_Server::delete_characteristics(const std::string& service_uuid, const std::string& char_uuid) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        service->removeCharacteristic(service->getCharacteristic(char_uuid));
        Serial.printf("Characteristic %s deleted from service %s\n", char_uuid.c_str(), service_uuid.c_str());
    } else {
        Serial.printf("Service %s not found\n", service_uuid.c_str());
    }
}

// Write to a characteristic dynamically
void BLE_Server::write_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, const std::string& value
) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        NimBLECharacteristic* characteristic = service->getCharacteristic(char_uuid);
        if (characteristic) {
            characteristic->setValue(value);
            Serial.printf(
                "Characteristic %s in service %s written with value: %s\n",
                char_uuid.c_str(),
                service_uuid.c_str(),
                value.c_str()
            );
        } else {
            Serial.printf("Characteristic %s not found in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        }
    } else {
        Serial.printf("Service %s not found\n", service_uuid.c_str());
    }
}

// Read from a characteristic dynamically
void BLE_Server::read_characteristics(const std::string& service_uuid, const std::string& char_uuid) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        NimBLECharacteristic* characteristic = service->getCharacteristic(char_uuid);
        if (characteristic) {
            std::string value = characteristic->getValue();
            Serial.printf(
                "Characteristic %s in service %s read with value: %s\n",
                char_uuid.c_str(),
                service_uuid.c_str(),
                value.c_str()
            );
        } else {
            Serial.printf("Characteristic %s not found in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        }
    } else {
        Serial.printf("Service %s not found\n", service_uuid.c_str());
    }
}

// Notify a characteristic dynamically
void BLE_Server::notify_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, const std::string& value
) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        NimBLECharacteristic* characteristic = service->getCharacteristic(char_uuid);
        if (characteristic) {
            characteristic->setValue(value);
            characteristic->notify();
            Serial.printf(
                "Characteristic %s in service %s notified with value: %s\n",
                char_uuid.c_str(),
                service_uuid.c_str(),
                value.c_str()
            );
        } else {
            Serial.printf("Characteristic %s not found in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        }
    } else {
        Serial.printf("Service %s not found\n", service_uuid.c_str());
    }
}

BLE_Server BLE_server;

} // namespace lilka
