#include "ble_server.h"

namespace lilka {

// Constructor
BLE_Server::BLE_Server() {
}

// Start the BLE server
void BLE_Server::begin() {
    NimBLEDevice::init("Lilka");

    NimBLEAddress mac = NimBLEDevice::getAddress();

    serial_log("MAC Address: ");
    serial_log(mac.toString().c_str());

    this->server = NimBLEDevice::createServer();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->start();
    serial_log("BLE Server started");
}

// Stop the BLE server
void BLE_Server::stop() {
    NimBLEDevice::stopAdvertising();
    NimBLEDevice::deinit();
    serial_log("BLE Server stopped");
}

// Create a new service dynamically
NimBLEService* BLE_Server::create_service(const std::string& uuid) {
    if (services.find(uuid) != services.end()) {
        serial_log("Service with UUID %s already exists\n", uuid.c_str());
        return nullptr;
    }

    NimBLEService* service = server->createService(uuid);
    services[uuid] = service;
    service->start();

    // Restart advertising with the new service UUID
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(uuid.c_str());
    advertising->start();

    serial_log("Service %s created and advertising restarted\n", uuid.c_str());
    return service;
}

// Delete a service dynamically
void BLE_Server::delete_service(const std::string& uuid) {
    auto it = services.find(uuid);
    if (it != services.end()) {
        // Remove the service
        server->removeService(it->second);
        services.erase(it);
        serial_log("Service %s deleted\n", uuid.c_str());

        // Restart advertising to reflect the removal of the service
        NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
        advertising->start(); // Restart advertising

        serial_log("Advertising restarted after removing service %s\n", uuid.c_str());
    } else {
        serial_log("Service %s not found\n", uuid.c_str());
    }
}

// Create a new characteristic dynamically
NimBLECharacteristic* BLE_Server::create_new_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, uint32_t properties
) {
    auto service_it = services.find(service_uuid);
    if (service_it == services.end()) {
        serial_log("Service %s not found\n", service_uuid.c_str());
        return nullptr;
    }

    NimBLEService* service = service_it->second;
    NimBLECharacteristic* characteristic = service->createCharacteristic(char_uuid, properties);

    if (!characteristic) {
        serial_log("Failed to create characteristic %s in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        return nullptr;
    }

    serial_log("Characteristic %s created in service %s\n", char_uuid.c_str(), service_uuid.c_str());

    // Restart advertising with the new characteristic UUID
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->start();

    serial_log(
        "Advertising restarted to include new characteristic %s in service %s\n",
        char_uuid.c_str(),
        service_uuid.c_str()
    );
    return characteristic;
}

// Delete a characteristic dynamically
void BLE_Server::delete_characteristics(const std::string& service_uuid, const std::string& char_uuid) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        // Remove the characteristic
        service->removeCharacteristic(service->getCharacteristic(char_uuid));
        serial_log("Characteristic %s deleted from service %s\n", char_uuid.c_str(), service_uuid.c_str());

        // Restart advertising to reflect the removal of the characteristic
        NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
        advertising->start(); // Restart advertising

        serial_log(
            "Advertising restarted after removing characteristic %s from service %s\n",
            char_uuid.c_str(),
            service_uuid.c_str()
        );
    } else {
        serial_log("Service %s not found\n", service_uuid.c_str());
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
            serial_log(
                "Characteristic %s in service %s written with value: %s\n",
                char_uuid.c_str(),
                service_uuid.c_str(),
                value.c_str()
            );
        } else {
            serial_log("Characteristic %s not found in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        }
    } else {
        // serial_log("Service %s not found\n", service_uuid.c_str());
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
            serial_log(
                "Characteristic %s in service %s read with value: %s\n",
                char_uuid.c_str(),
                service_uuid.c_str(),
                value.c_str()
            );
        } else {
            serial_log("Characteristic %s not found in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        }
    } else {
        serial_log("Service %s not found\n", service_uuid.c_str());
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
            // serial_log(
            //     "Characteristic %s in service %s notified with value: %s\n",
            //     char_uuid.c_str(),
            //     service_uuid.c_str(),
            //     value.c_str()
            // );
        } else {
            // serial_log("Characteristic %s not found in service %s\n", char_uuid.c_str(), service_uuid.c_str());
        }
    } else {
        // serial_log("Service %s not found\n", service_uuid.c_str());
    }
}

BLE_Server BLE_server;

} // namespace lilka
