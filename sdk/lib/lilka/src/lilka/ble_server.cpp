#include "ble_server.h"
#include "serial.h"

namespace lilka {

// Constructor
BLE_Server::BLE_Server() : server(nullptr) {
}

// Start the BLE server
void BLE_Server::begin() {
    if (!serverCallbacks) {
        serverCallbacks = new ServerCallbacks();
    }

    NimBLEDevice::init("Lilka");

    NimBLEAddress mac = NimBLEDevice::getAddress();

    serial_log("MAC Address: ");
    serial_log(mac.toString().c_str());

    this->server = NimBLEDevice::createServer();
    this->server->setCallbacks(serverCallbacks);

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->start();
}

// Stop the BLE server
void BLE_Server::stop() {
    NimBLEDevice::stopAdvertising();
    NimBLEDevice::deinit();
    if (serverCallbacks) {
        delete serverCallbacks;
        serverCallbacks = nullptr; // Avoid dangling pointer
    }
}

// Create a new service dynamically
BLE_Server_status BLE_Server::create_service(const std::string& uuid) {
    if (services.find(uuid) != services.end()) {
        return BLE_Server_status::SERVICE_EXISTS;
    }

    NimBLEService* service = server->createService(uuid);
    services[uuid] = service;
    service->start();

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->addServiceUUID(uuid.c_str());
    advertising->start();
    return BLE_Server_status::OK;
}

BLE_Server_status BLE_Server::delete_service(const std::string& uuid) {
    auto it = services.find(uuid);
    if (it != services.end()) {
        server->removeService(it->second);
        services.erase(it);
        NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
        advertising->start();

        return BLE_Server_status::OK;
    } else {
        return BLE_Server_status::SERVICE_NOT_FOUND;
    }
}

// Create a new characteristic dynamically
BLE_Server_status BLE_Server::create_new_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, uint32_t properties
) {
    auto service_it = services.find(service_uuid);
    if (service_it == services.end()) {
        return BLE_Server_status::SERVICE_NOT_FOUND;
    }

    NimBLEService* service = service_it->second;
    const NimBLECharacteristic* characteristic = service->createCharacteristic(char_uuid, properties);

    if (!characteristic) {
        return BLE_Server_status::CHARACTERISTIC_FAILED_TO_CREATE;
    }

    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    advertising->start();

    return BLE_Server_status::OK;
}

// Delete a characteristic dynamically
BLE_Server_status BLE_Server::delete_characteristics(const std::string& service_uuid, const std::string& char_uuid) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        // Remove the characteristic
        service->removeCharacteristic(service->getCharacteristic(char_uuid));

        // Restart advertising to reflect the removal of the characteristic
        NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
        advertising->start(); // Restart advertising

        return BLE_Server_status::OK;
    } else {
        return BLE_Server_status::SERVICE_NOT_FOUND;
    }
}

// Write to a characteristic dynamically
BLE_Server_status BLE_Server::write_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, const std::string& value
) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        NimBLECharacteristic* characteristic = service->getCharacteristic(char_uuid);
        if (characteristic) {
            characteristic->setValue(value);
            return BLE_Server_status::OK;
        } else {
            return BLE_Server_status::CHARACTERISTIC_NOT_FOUND;
        }
    } else {
        return BLE_Server_status::SERVICE_NOT_FOUND;
    }
}

// Read from a characteristic dynamically
std::string BLE_Server::read_characteristics(const std::string& service_uuid, const std::string& char_uuid) {
    // Find the service
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;

        // Find the characteristic
        NimBLECharacteristic* characteristic = service->getCharacteristic(char_uuid);
        if (characteristic) {
            // Read the characteristic value
            std::string value = characteristic->getValue();
            characteristic->setValue(NULL);
            return value;
        } else {
            return "ERR:" + std::to_string(static_cast<int>(BLE_Server_status::CHARACTERISTIC_NOT_FOUND));
        }
    } else {
        return "ERR:" + std::to_string(static_cast<int>(BLE_Server_status::SERVICE_NOT_FOUND));
    }
}

// Notify a characteristic dynamically
BLE_Server_status BLE_Server::notify_characteristics(
    const std::string& service_uuid, const std::string& char_uuid, const std::string& value
) {
    auto service_it = services.find(service_uuid);
    if (service_it != services.end()) {
        NimBLEService* service = service_it->second;
        NimBLECharacteristic* characteristic = service->getCharacteristic(char_uuid);
        if (characteristic) {
            characteristic->setValue(value);
            characteristic->notify();
            return BLE_Server_status::OK;
        } else {
            return BLE_Server_status::CHARACTERISTIC_NOT_FOUND;
        }
    } else {
        return BLE_Server_status::SERVICE_NOT_FOUND;
    }
}

bool BLE_Server::getConnectionStatus() {
    if (serverCallbacks) {
        return serverCallbacks->getConnectionStatus();
    }

    serial_err("serverCallbacks is null");
    return false;
}

ServerCallbacks* serverCallbacks;
BLE_Server BLE_server;

} // namespace lilka
