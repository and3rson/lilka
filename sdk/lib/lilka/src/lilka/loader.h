#ifndef LILKA_LOADER_H
#define LILKA_LOADER_H

#include <esp_ota_ops.h>
#include <Arduino.h>

#include "sdcard.h"

namespace lilka {

/// Контекст завантажувача.
///
/// Використовується для завантаження прошивки з microSD-картки в OTA-розділ.
///
/// Приклад використання:
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka::begin();
///
///     Serial.println("Завантаження прошивки 'firmware.bin' з microSD-картки...");
///
///     lilka::LoaderHandle *handle = lilka::loader.createHandle("/sd/firmware.bin"); // Створити новий завантажувач файлу "/sd/firmware.bin"
///     handle->start(); // Почати завантаження
///     while (handle->process() != 0) { // Обробити завантаження
///         Serial.printf("Завантажено %d/%d байтів\n", handle->getBytesWritten(), handle->getBytesTotal());
///     }
///     handle->finishAndReboot(); // Завершити завантаження та перезавантажити пристрій
/// }
/// @endcode
class LoaderHandle {
public:
    LoaderHandle(String path);

    /// Почати завантаження.
    /// \return 0, якщо завантаження почалося успішно, <0 - у разі помилки.
    int start();
    /// Обробити завантаження.
    /// Цей метод повинен викликатися в циклі, поки не поверне 0.
    /// Щоразу він опрацьовує частину файлу та записує її в OTA-розділ.
    /// \return 0, якщо завантаження завершилося успішно, <0 - у разі помилки, >0 - означає кількість байтів, які було оброблено.
    int process();
    /// Отримати кількість байтів, які було записано в OTA-розділ.
    /// \return Кількість байтів
    int getBytesWritten();
    /// Отримати загальну кількість байтів, які потрібно записати в OTA-розділ.
    /// \return Кількість байтів
    int getBytesTotal();
    /// Завершити завантаження та перезавантажити пристрій.
    /// \return <0 - у разі помилки. В разі успіху цей метод не повертається, оскільки пристрій перезавантажується.
    int finishAndReboot();

private:
    String path;
    FILE *file;
    esp_ota_handle_t ota_handle;
    const esp_partition_t *ota_partition;
    int bytesWritten;
    int bytesTotal;
};

/// Завантажувач прошивок з microSD-картки.
///
/// Дозволяє прочитати файл прошивки з microSD-картки в OTA-розділ та запустити його один зар,
/// не замінюючи основну прошивку.
///
/// Нова прошивка зберігається в OTA-розділі та запускається при перезавантаженні,
/// але активною залишається основна прошивка (app rollback).
///
/// Це дозволяє компілювати за записувати різні прошивки на microSD-карту і запускати їх через браузер SD-карти без необхідності перепрошивки.
///
/// Детальніше про auto-rollback: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/ota.html#rollback-process
class Loader {
public:
    /// Почати роботу з завантажувачем.
    /// \warning Цей метод викликається автоматично при виклику `lilka::begin()`.
    void begin();

    /// Створити новий завантажувач.
    /// \return Вказівник на новий завантажувач `LoaderHandle`.
    LoaderHandle *createHandle(String path);
};

/// Екземпляр класу `Loader`, який можна використовувати для роботи з завантажувачем.
/// Вам не потрібно інстанціювати `Loader` вручну.
extern Loader loader;

} // namespace lilka

#endif // LILKA_LOADER_H
