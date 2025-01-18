#pragma once
#include <ff.h>
#include <SPIFFS.h>
#include <SD.h>
#include <dirent.h>
#include <sd_diskio.h>

#define LILKA_SD_ROOT      "/sd"
#define LILKA_SPIFFS_ROOT  "/spiffs"
#define LILKA_SLASH        "/"
#define LILKA_SD_FREQUENCY 20000000

namespace lilka {

/// Інформація про шлях
typedef struct {
    /// Вказівник на драйвер файлової системи
    FS* driver;
    /// Локальний шлях у цій файловій системі
    String path;
} PathInfo;

/// Тип елементу в директорії
typedef enum {
    /// Файл
    ENT_FILE,
    /// Директорія
    ENT_DIRECTORY,
} EntryType;

/// Інформація про елемент в директорії
typedef struct {
    /// Ім'я елементу
    String name;
    /// Тип елементу
    EntryType type;
} Entry;

/// Клас, що містить допоміжні функції для роботи з файловими системами.
///
/// Цей клас НЕ містить методів типу `open`, `read`, `write` і т.д., оскільки ці методи вже є в класах `SD` та `SPIFFS`
/// і для роботи з файлами вам слід використовуйте саме їх, оскільки вони добре документовані та легкі в використанні: https://www.arduino.cc/reference/en/libraries/sd/
///
/// Тому у цьому класі ви знайдете лише допоміжні методи, такі як ініціалізація носіїв даних, перетворення шляхів, читання директорії, перевірка доступності файлових систем тощо.
///
/// Крім цього, деякий сторонній код (наприклад, емулятор NES чи рушій Doom) працюють з файлами через стандартні функції Сі, тому цей клас містить методи для перетворення канонічних шляхів в локальні та навпаки.
///
/// * Локальний шлях - це такий, який визначається відносно кореня носія даних (наприклад, `/test.txt`). В більшості випадків ви будуте працювати лише з локальними шляхами. Наприклад, `File test = SD.open("/test.txt")` відкриє файл `test.txt` з SD-карти.
/// * Канонічний шлях - це такий, який визначається відносно кореня віртуальної файлової системи ESP32 (наприклад, `/sd/test.txt`). Ви можете використовувати канонічні шляхи для взаємодії з файлами через стандартні функції Сі, наприклад `FILE* test = fopen("/sd/test.txt", "r")`.
///
/// Детальніше про віртуальну файлову систему ESP32 можна прочитати тут: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/storage/vfs.html
class FileUtils {
public:
    FileUtils();
    /// Ініціалізувати SPIFFS та SD-карту
    /// @warning Цей метод викликається автоматично при виклику `lilka::begin()`.
    void begin(bool beginSD = true, bool beginSPIFFS = true);
    /// Ініціалізувати SD-карту
    bool initSD();
    /// Ініціалізує SPIFFS
    void initSPIFFS();
    /// Кількість елементів у директорії
    /// @param driver драйвер файлової системи
    /// @param localPath шлях до директорії
    /// @return кількість елементів
    uint32_t getEntryCount(FS* driver, const String& localPath);
    /// Канонічний шлях до SD
    /// @return канонічний шлях (наприклад, `/sd`)
    const String getSDRoot();
    /// Канонічний шлях до SPIFFS
    /// @return канонічний шлях (наприклад, `/spiffs`)
    const String getSPIFFSRoot();
    /// Перевірити, чи доступна SD картка
    /// @return true у разі успіху
    bool isSDAvailable();
    /// Перевірити, чи доступна SPIFFS
    /// @return true у разі успіху
    bool isSPIFFSAvailable();
    /// Перетворити локальний шлях в канонічний
    /// @param driver драйвер файлової системи
    /// @param localPath локальний шлях
    /// @return канонічний шлях
    ///
    /// Приклад використання:
    /// @code
    /// String localPath = "/test.txt";
    /// String cannonicalPath = fileutils.getCannonicalPath(&SD, localPath);
    /// // cannonicalPath міститиме рядок "/sd/test.txt"
    /// FILE* test = fopen(cannonicalPath.c_str(), "r"); // Відкрити файл "/sd/test.txt" з SD-карти
    /// @endcode
    const String getCannonicalPath(const FS* driver, const String& localPath);
    /// Отримати інформацію про канонічний шлях
    ///
    /// Використовується для перетворення канонічного шляху в локальний
    /// або для визначення драйвера файлової системи.
    /// @param cannonicalPath локальний шлях
    /// @return структуру PathInfo
    ///
    /// Приклад використання:
    /// @code
    /// String cannonicalPath = "/sd/test.txt";
    /// PathInfo pathInfo = fileutils.getLocalPathInfo(cannonicalPath);
    /// // pathInfo.driver міститиме вказівник на драйвер файлової системи SD
    /// // pathInfo.path міститиме рядок "/test.txt"
    /// File test = pathInfo.driver->open(pathInfo.path); // Відкрити файл "/test.txt" з SD-карти
    /// @endcode
    const PathInfo getLocalPathInfo(const String& cannonicalPath);
    /// Отримати елементи в директорії за відносним шляхом
    /// @param driver драйвер файлової системи (наприклад, `&SD`)
    /// @param localPath локальний шлях
    /// @param entries вказівник, за яким буде записано елементи
    /// @return кількість записаних елементів
    size_t listDir(FS* driver, const String& localPath, Entry entries[]);
    /// Об'єднує шляхи
    /// @param lPath ліва половина шляху
    /// @param rPath права половина шляху
    /// @return повний шлях
    const String joinPath(const String& lPath, const String& rPath);
    /// Отримати шлях до директорії в якій лежить файл
    /// @param path Шлях до файлу
    /// @return шлях
    // TODO: задокументувати
    const String getParentDirectory(const String& path);
    /// Створити нову таблицю розділів на SD картці
    /// @warning Після виклику цієї функції необхідно перезавантажити систему
    /// @return true у разі успіху
    bool createSDPartTable();
    /// Форматувати SD картку
    /// @warning Після виклику цієї функції необхідно перезавантажити систему
    /// @return true у разі успіху
    bool formatSD();
    /// Обрізати слеші в кінці шляху.
    ///
    /// @param path Шлях до файлу
    /// @return Шлях без кінцевих слешів
    const String stripPath(const String& path);
    /// Повернути розмір в читабельному форматі (наприклад, 101 MB).
    ///
    /// @param size Розмір (в байтах)
    /// @param compact Чи виводити розмір компактно (наприклад, 1.23MB замість 1.23 MB)
    /// @return Рядок, що містить читабельний розмір з суфіксами одиниць виміру
    ///
    /// @code
    /// fileutils.getHumanFriendlySize(1234567); // Поверне "1.23 MB"
    /// @endcode
    const String getHumanFriendlySize(const uint64_t size, bool compact = false);

private:
    SemaphoreHandle_t sdMutex = NULL;
    SPIFFSFS* spiffs;
    SDFS* sdfs;
};

/// Екземпляр класу `FileUtils`, який можна використовувати для роботи з дисплеєм.
/// Вам не потрібно інстанціювати `FileUtils` вручну.
extern FileUtils fileutils;

} // namespace lilka
