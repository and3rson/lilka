#pragma once
#include <ff.h>
#include "config.h"
#include <SPIFFS.h>
#include <SD.h>
#include "config.h"
#include <dirent.h>
#include <sd_diskio.h>

#define LILKA_SD_ROOT      "/sd"
#define LILKA_SPIFFS_ROOT  "/spiffs"
#define LILKA_SLASH        "/"
#define H_FILE_SIZE        6
#define LILKA_SD_FREQUENCY 20000000

namespace lilka {
typedef struct {
    FS* fSysDriver;
    String path;
} PathInfo;
typedef enum {
    ENT_FILE,
    ENT_DIRECTORY,
} EntryType;

typedef struct {
    String name;
    EntryType type;
    size_t size;
} Entry;

class FileUtils {
public:
    FileUtils();
    /// Ініціалізує SPIFFS та SD
    void init();
    /// Ініціалізує лише SD
    bool initSD();
    /// Ініціалізує лише SPIFFS
    void initSPIFFS();
    /// Кількість елементів у директорії
    /// @param fSysDriver драйвер файлової системи
    /// @param relPath відносний шлях до директорії
    /// @returns кількість елементів
    uint32_t getEntryCount(FS* fSysDriver, const String& relPath);
    /// Канонічний шлях до SD
    /// @returns канонічний шлях
    const String getSDRoot();
    /// Канонічний шлях до SPIFFS
    /// @returns канонічний шлях
    const String getSPIFFSRoot();
    /// Перевіряє чи доступна SD картка
    /// @returns true у разі успіху
    bool isSDAvailable();
    /// Перевіряє чи доступна SPIFFS
    /// @returns true у разі успіху
    bool isSPIFFSAvailable();
    /// Конвертує відносний шлях в канонічний
    /// @param fSysDriver драйвер файлової системи
    /// @param relPath  відносний шлях
    /// @returns канонічний шлях
    const String getFullPath(const FS* fSysDriver, const String& relPath);
    /// Отримати інформацію про канонічний шлях
    /// Використовується для конвертації канонічного шляху в відносний
    /// або для визначення драйвера файлової системи
    /// @param fSysDriver драйвер файлової системи
    /// @param relPath  відносний шлях
    /// @returns структуру PathInfo
    const PathInfo getRelativePathInfo(const String& absPath);
    /// Отримати елементи в директорії за відносним шляхом
    /// @param fSysDriver драйвер файлової системи
    /// @param relPath відносний шлях
    /// @param entries вказівник за яким буде записано елементи
    /// @returns кількість записаних елементів
    size_t listDir(FS* fSysDriver, const String& relPath, Entry entries[]);
    /// Об'єднує шляхи
    /// @param lPath ліва половина шляху
    /// @param rPath права половина шляху
    /// @returns повний шлях
    const String joinPath(const String& lPath, const String& rPath);
    /// Створити нову таблицю розділів на SD картці
    /// УВАГА: після виклику цієї функції необхідно перезавантажити систему
    /// @returns true у разі успіху
    bool createSDPartTable();
    /// Форматувати SD картку
    /// УВАГА: після виклику цієї функції необхідно перезавантажити систему
    /// @returns true у разі успіху
    bool formatSD();
    /// Обрізати слеші в кінці шляху.
    ///
    /// @param path Шлях до файлу
    /// @return Шлях без кінцевих слешів
    const String stripPath(const String& path);
    /// Повернути розмір в читабельному форматі (наприклад, 101 MB).
    ///
    /// @param size Розмір (в байтах)
    /// @return Рядок, що містить читабельний розмір з суфіксами одиниць виміру
    ///
    /// @code
    /// fileutils.getHumanFriendlySize(1234567); // Поверне "1.23 MB"
    /// @endcode
    const String getHumanFriendlySize(const size_t size);

private:
    bool sdMountLocked = false;
    bool spiffsAvailable = false;
    FS* spiffs;
    SDFS* sdfs;
};

extern FileUtils fileutils;

} // namespace lilka
