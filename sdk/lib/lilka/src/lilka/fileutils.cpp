#include "fileutils.h"
#include "serial.h"
#include "spi.h"
#include "config.h"

namespace lilka {
FileUtils::FileUtils() : sdMutex(xSemaphoreCreateMutex()) {
    sdfs = &SD;
    spiffs = &SPIFFS;
}

void FileUtils::begin(bool beginSD, bool beginSPIFFS) {
    if (beginSD) {
        initSD();
    }
    if (beginSPIFFS) {
        initSPIFFS();
    }
}

void FileUtils::initSPIFFS() {
    serial_log("initializing SPIFFS");
    if (!SPIFFS.begin(false, LILKA_SPIFFS_ROOT)) {
        serial_err("failed to initialize SPIFFS");
    } else {
        serial_log("SPIFFS ok");
    }
}

bool FileUtils::initSD() {
    // Most likely we're trying to format card,
    // please standby
    xSemaphoreTake(sdMutex, portMAX_DELAY);

    // check if LILKA_SDROOT pathable, if not perform init
    serial_log("initializing SD card");

#if LILKA_SDCARD_CS < 0
    serial_err("SD init failed: no CS pin");
#else
    // clang-format off
#ifdef USE_EXT_SPI_FOR_SD
bool init_result = sdfs->begin(
        SPI2_DEV1_CS, SPI2, LILKA_SD_FREQUENCY, LILKA_SD_ROOT
    ); // TODO: is 20 MHz OK for all cards? // TODO: is 20 MHz OK for all cards?
#else
    bool init_result = sdfs->begin(
        LILKA_SDCARD_CS, SPI1, LILKA_SD_FREQUENCY, LILKA_SD_ROOT
    ); // TODO: is 20 MHz OK for all cards? // TODO: is 20 MHz OK for all cards?
#endif
    // clang-format on
    sdcard_type_t cardType = sdfs->cardType();
    if (!init_result) {
        // Can't init -> should end or we'll mess
        // with internal implemenation of sdfs
        sdfs->end();
        xSemaphoreGive(sdMutex);
        return false;
    }
    if (cardType == CARD_SD || cardType == CARD_SDHC) {
        serial_log(
            "card type: %s, card size: %s",
            cardType == CARD_SD ? "SD" : "SDHC",
            getHumanFriendlySize(sdfs->totalBytes()).c_str()
        );
        // ALL OKAY!
        xSemaphoreGive(sdMutex);
        return true;
    } else {
        serial_err("unknown SD card type: %d", cardType);
    }
#endif
    xSemaphoreGive(sdMutex);
    return false;
}

uint32_t FileUtils::getEntryCount(FS* driver, const String& localPath) {
    if (driver == NULL) {
        serial_err("Path (%s) reffers to unknown filesystem type", localPath.c_str());
        return 0;
    }

    File root = driver->open(stripPath(localPath));
    // Below we assume if folder can't be open then it has zero files
    // Btw we will show this error using serial
    if (!root) {
        serial_err("getEntryCount: failed to open directory: %s", localPath.c_str());
        return 0;
    }
    if (!root.isDirectory()) {
        serial_err("getEntryCount: not a directory: %s", localPath.c_str());
        return 0;
    }

    size_t countFiles = 0;

    while (root.getNextFileName() != "") {
        countFiles++;
    }

    return countFiles;
}

const String FileUtils::stripPath(const String& path) {
    String striped_path = path;
    while (!striped_path.equals("/") && striped_path.endsWith("/")) {
        // Strip trailing slashes, unless it's the root directory
        striped_path.remove(striped_path.length() - 1);
    }
    return striped_path;
}

size_t FileUtils::listDir(FS* driver, const String& localPath, Entry entries[]) {
    if (driver == NULL) {
        serial_err("Path (%s) reffers to unknown filesystem type", localPath.c_str());
        return 0;
    }

    File root = driver->open(stripPath(localPath));
    if (!root) {
        serial_err("listDir: failed to open directory: %s", localPath.c_str());
        return -1;
    }
    if (!root.isDirectory()) {
        serial_err("listDir: not a directory: %s", localPath.c_str());
        return -1;
    }

    int i = 0;
    bool isDir;
    String name = "";
    while ((name = root.getNextFileName(&isDir)) != "") {
        entries[i].name = basename(name.c_str());
        //lilka::serial_log("New name = %s", name.c_str());

        if (isDir) entries[i].type = ENT_DIRECTORY;
        else entries[i].type = ENT_FILE;
        i++;
    }
    root.close();
    // Sort filenames, but keep directories first
    qsort(entries, i, sizeof(Entry), [](const void* a, const void* b) -> int {
        const Entry* ea = static_cast<const Entry*>(a);
        const Entry* eb = static_cast<const Entry*>(b);
        if (ea->type == ENT_DIRECTORY && eb->type != ENT_DIRECTORY) {
            return -1;
        } else if (ea->type != ENT_DIRECTORY && eb->type == ENT_DIRECTORY) {
            return 1;
        }
        return ea->name.compareTo(eb->name);
    });
    return i;
}

const String FileUtils::getCannonicalPath(const FS* driver, const String& localPath) {
    // Allready okay
    if (localPath.startsWith(LILKA_SD_ROOT LILKA_SLASH) || localPath.startsWith(LILKA_SPIFFS_ROOT LILKA_SLASH))
        return localPath;

    if (driver == sdfs) {
        return String(LILKA_SD_ROOT) + localPath;
    } else if (driver == spiffs) {
        return String(LILKA_SPIFFS_ROOT) + localPath;
    }
    serial_err("Unknown driver provided");
    return localPath;
}

const PathInfo FileUtils::getLocalPathInfo(const String& cannonicalPath) {
    PathInfo pathInfo;
    if (cannonicalPath.startsWith(LILKA_SD_ROOT LILKA_SLASH)) {
        pathInfo.path = cannonicalPath.substring(sizeof(LILKA_SD_ROOT) - 1);
        pathInfo.driver = sdfs;
    } else if (cannonicalPath.startsWith(LILKA_SPIFFS_ROOT LILKA_SLASH)) {
        pathInfo.path = cannonicalPath.substring(sizeof(LILKA_SPIFFS_ROOT) - 1);
        pathInfo.driver = spiffs;
    } else
    // Maybe path is allready local?
    {
        pathInfo.path = cannonicalPath;
        // Can't determine driver from
        // given path
        pathInfo.driver = NULL;
    }
    return pathInfo;
}

bool FileUtils::isSDAvailable() {
    // init could be run any count of times
    // cuz SD.begin() anyway checks if
    // sd allready mounted, btw it will not answer on a
    // card presence question
    // for this purpose we need to digitalRead CardDetect pin which we
    // do not have.
    // File Open\Close from sd card will try to use spi
    // which is really bad for display, so this is it
    sdcard_type_t sdType = sdfs->cardType();
    return (sdType == CARD_SD || sdType == CARD_SDHC);
}

bool FileUtils::isSPIFFSAvailable() {
    return SPIFFS.begin(false, LILKA_SPIFFS_ROOT);
}

const String FileUtils::getSDRoot() {
    return LILKA_SD_ROOT;
}

const String FileUtils::getSPIFFSRoot() {
    return LILKA_SPIFFS_ROOT;
}
const String FileUtils::getHumanFriendlySize(const uint64_t size, bool compact) {
    // Max length of file size

    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    const int numSuffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    if (size == 0) return String("0") + (compact ? "" : "  ") + suffixes[0];

    int exp = 0;

    long double dsize = static_cast<long double>(size);

    while (dsize >= 1024 && exp < numSuffixes - 1) {
        dsize /= 1024;
        exp++;
    }

    char buffer[50];
    if (compact) {
        snprintf(buffer, sizeof(buffer), "%.0Lf%s", dsize, suffixes[exp]);
    } else {
        snprintf(buffer, sizeof(buffer), "%.0Lf %2s", dsize, suffixes[exp]);
    }

    return String(buffer);
}

bool FileUtils::createSDPartTable() {
    xSemaphoreTake(sdMutex, portMAX_DELAY);
    // Unmount sd
    // We could have some io problems here, but user actually want to destroy all io
    // results, seems to be okay :)
    SD.end();
    constexpr uint32_t workbufSize = FF_MAX_SS * 4;
    uint8_t* workbuf = new uint8_t[workbufSize];
    std::unique_ptr<uint8_t[]> workbufPtr(workbuf);

    // init without mount
    // clang-format off
#ifdef USE_EXT_SPI_FOR_SD
    uint8_t pdrv = sdcard_init(SPI2_DEV1_CS, &SPI2, LILKA_SD_FREQUENCY);
#else
    uint8_t pdrv = sdcard_init(LILKA_SDCARD_CS, &SPI1, LILKA_SD_FREQUENCY);
#endif
    if (pdrv == 0xFF) {
        xSemaphoreGive(sdMutex);
        return false;
    }

    // SD card uninitializer (RAII)
    std::unique_ptr<void, void (*)(void*)> sdcardUninit(nullptr, [](void* pdrv) {
        // C++ is beautiful and ugly at the same time
        sdcard_uninit(*static_cast<uint8_t*>(pdrv));
    });

    // Create partition table

    DWORD plist[] = {100, 0, 0, 0};
    FRESULT res = f_fdisk(pdrv, plist, workbuf);
    if (res != FR_OK) {
        xSemaphoreGive(sdMutex);
        return false;
    }
    xSemaphoreGive(sdMutex);

    return true;
}

bool FileUtils::formatSD() {
    xSemaphoreTake(sdMutex, portMAX_DELAY);
    SD.end();

    constexpr uint32_t workbufSize = FF_MAX_SS * 4;
    uint8_t* workbuf = new uint8_t[workbufSize];

    std::unique_ptr<uint8_t[]> workbufPtr(workbuf);

    FRESULT res = f_mkfs(LILKA_SD_ROOT, FM_ANY, 0, workbuf, workbufSize);
    if (res != FR_OK) {
        xSemaphoreGive(sdMutex);
        return false;
    }
    xSemaphoreGive(sdMutex);
    return true;
}
const String FileUtils::joinPath(const String& lPath, const String& rPath) {
    String path = lPath;
    // Ensure lPath ends with "/" if not empty
    if (path != "" && path.c_str()[path.length() - 1] != '/') path += '/';

    // Remove any leading "/" from rPath
    size_t pos = 0;
    while (rPath.c_str()[pos] == '/' && pos < rPath.length())
        pos++;
    path += rPath.substring(pos);

    return path;
}

const String FileUtils::getParentDirectory(const String& path) {
    String cleanPath = path;
    // Remove trailing '/' if it exists
    if (cleanPath.endsWith("/")) {
        cleanPath.remove(cleanPath.length() - 1);
    }
    // Find the last '/'
    int lastSlash = cleanPath.lastIndexOf('/');
    if (lastSlash == -1) {
        // No '/' found, assume it's at the top level
        return "/";
    }
    // Return the substring up to the last '/'
    return cleanPath.substring(0, lastSlash);
}

FileUtils fileutils;
} // namespace lilka
