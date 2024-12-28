#ifndef LILKA_BLE_SERVER_H
#define LILKA_BLE_SERVER_H

#include <stdint.h>
#include <string>
#include <map>
#include <NimBLEDevice.h>

namespace lilka {

/// Статуси, що описують результат операцій з BLE сервером.
enum class BLE_Server_status {
    OK = 0,
    SERVICE_NOT_FOUND, ///< Сервіс не знайдений.
    SERVICE_EXISTS, ///< Сервіс вже існує.
    SERVICE_FAILED_TO_CREATE, ///< Не вдалося створити сервіс.
    CHARACTERISTIC_NOT_FOUND, ///< Характеристика не знайдена.
    CHARACTERISTIC_EXISTS, ///< Характеристика вже існує.
    CHARACTERISTIC_FAILED_TO_CREATE, ///< Не вдалося створити характеристику.
    WRITE_ERROR, ///< Помилка запису.
    READ_ERROR, ///< Помилка читання.
    NOTIFY_ERROR ///< Помилка при сповіщенні.
};

/// Клас зворотних викликів для управління підключенням клієнта до BLE сервера.
class ServerCallbacks : public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* pServer) override {
        Serial.println("Client connected");
        isConnected = true;
    }

    void onDisconnect(NimBLEServer* pServer) override {
        Serial.println("Client disconnected");
        isConnected = false;
    }

    bool getConnectionStatus() const {
        return isConnected;
    }

private:
    bool isConnected = false;
};

/// Клас для управління BLE сервером.
class BLE_Server {
private:
    NimBLEServer* server; ///< Вказівник на сервер BLE.
    std::map<std::string, NimBLEService*> services; ///< Карта сервісів за їх UUID.
    std::map<std::string, NimBLECharacteristic*> characteristics; ///< Карта характеристик за їх UUID.

protected:
    void set_server(NimBLEServer* srv) {
        server = srv;
    }

public:
    BLE_Server();

    /// Почати роботу з BLE сервером.
    void begin();

    /// Зупинити BLE сервер.
    void stop();

    /// Створити новий сервіс за заданим UUID.
    /// @param uuid UUID сервісу.
    /// @return Статус операції.
    BLE_Server_status create_service(const std::string& uuid);

    /// Видалити сервіс за заданим UUID.
    /// @param uuid UUID сервісу.
    /// @return Статус операції.
    BLE_Server_status delete_service(const std::string& uuid);

    /// Створити нову характеристику для сервісу.
    /// @param service_uuid UUID сервісу.
    /// @param characteristics_uuid UUID характеристики.
    /// @param properties Властивості характеристики.
    /// @return Статус операції.
    BLE_Server_status create_new_characteristics(
        const std::string& service_uuid, const std::string& characteristics_uuid, uint32_t properties
    );

    /// Видалити характеристику для сервісу.
    /// @param service_uuid UUID сервісу.
    /// @param characteristics_uuid UUID характеристики.
    /// @return Статус операції.
    BLE_Server_status delete_characteristics(const std::string& service_uuid, const std::string& characteristics_uuid);

    /// Записати значення в характеристику.
    /// @param service_uuid UUID сервісу.
    /// @param characteristics_uuid UUID характеристики.
    /// @param value Значення для запису.
    /// @return Статус операції.
    BLE_Server_status write_characteristics(
        const std::string& service_uuid, const std::string& characteristics_uuid, const std::string& value
    );

    /// Прочитати значення з характеристики.
    /// @param service_uuid UUID сервісу.
    /// @param char_uuid UUID характеристики.
    /// @return Значення характеристики.
    std::string read_characteristics(const std::string& service_uuid, const std::string& char_uuid);

    /// Сповістити про зміни в характеристиці.
    /// @param service_uuid UUID сервісу.
    /// @param characteristics_uuid UUID характеристики.
    /// @param value Значення для сповіщення.
    /// @return Статус операції.
    BLE_Server_status notify_characteristics(
        const std::string& service_uuid, const std::string& characteristics_uuid, const std::string& value
    );

    /// Отримати статус підключення до сервера.
    /// @return Статус підключення.
    bool getConnectionStatus();
};

/// Екземпляр класу `BLE_Server`, який можна використовувати для управління BLE сервером.
extern ServerCallbacks* serverCallbacks;
extern BLE_Server BLE_server;

} // namespace lilka

#endif
