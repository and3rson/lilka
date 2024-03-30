#include "fileutils.h"
#include "serial.h"
#include "spi.h"
namespace lilka {
FileUtils::FileUtils() {
    sdfs = &SD;
    spiffs = &SPIFFS;
}

void FileUtils::init() {
    initSD();
    initSPIFFS();
}

void FileUtils::initSPIFFS() {
    File spiffsRoot = spiffs->open("/");
    if (spiffsRoot) {
        // Allready initialized
        spiffsRoot.close();
        return;
    }
    serial_log("initializing SPIFFS");
    if (!SPIFFS.begin(false, LILKA_SPIFFS_ROOT)) {
        serial_err("failed to initialize SPIFFS");
    } else {
        serial_log("SPIFFS ok");
        spiffsAvailable = true;
    }
}

bool FileUtils::initSD() {
    if (sdMountLocked) return false;
    // check if LILKA_SDROOT pathable, if not perform init
    File sdRoot = sdfs->open("/");
    if (sdRoot) {
        // Allready initialized
        sdRoot.close();
        return false;
    }
    serial_log("initializing SD card");

#if LILKA_SDCARD_CS < 0
    serial_err("SD init failed: no CS pin");
#else
    sdfs->begin(LILKA_SDCARD_CS, SPI1, 20000000, LILKA_SDROOT); // TODO: is 20 MHz OK for all cards?
    sdcard_type_t cardType = sdfs->cardType();

    if (cardType == CARD_NONE) {
        serial_err("no SD card found");
        return false;
    }

    if (cardType == CARD_SD || cardType == CARD_SDHC) {
        serial_log(
            "card type: %s, card size: %ld MB", cardType == CARD_SD ? "SD" : "SDHC", sdfs->totalBytes() / (1024 * 1024)
        );
        return true;
    } else {
        serial_err("unknown SD card type: %d", cardType);
    }
#endif
    return false;
}

uint32_t FileUtils::getEntryCount(FS* fSysDriver, const String& path) {
    FS* fs = fSysDriver;

    if (fs == NULL) {
        serial_err("Path (%s) reffers to unknown filesystem type", path.c_str());
        return 0;
    }

    File root = fs->open(stripPath(path));
    // Below we assume if folder can't be open then it has zero files
    // Btw we will show this error using serial
    if (!root) {
        serial_err("getEntryCount: failed to open directory: %s", path.c_str());
        return 0;
    }
    if (!root.isDirectory()) {
        serial_err("getEntryCount: not a directory: %s", path.c_str());
        return 0;
    }

    size_t countFiles = 0;
    File file = root.openNextFile();

    while (file) {
        file = root.openNextFile();
        countFiles++;
    }

    root.close();

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

size_t FileUtils::listDir(FS* fSysDriver, const String& path, Entry entries[]) {
    FS* fs = fSysDriver;

    if (fs == NULL) {
        serial_err("Path (%s) reffers to unknown filesystem type", path.c_str());
        return 0;
    }

    File root = fs->open(stripPath(path));
    if (!root) {
        serial_err("listDir: failed to open directory: %s", path.c_str());
        return -1;
    }
    if (!root.isDirectory()) {
        serial_err("listDir: not a directory: %s", path.c_str());
        return -1;
    }

    int i = 0;
    File file = root.openNextFile();
    while (file) {
        entries[i].name = file.name();
        if (file.isDirectory()) {
            entries[i].type = ENT_DIRECTORY;
        } else {
            entries[i].type = ENT_FILE;
        }
        entries[i].size = file.size();
        file.close();
        file = root.openNextFile();
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

FS* FileUtils::getFSysDriverByFullPath(const String& path) {
    FS* fs = NULL;
    if (path.startsWith(LILKA_SDROOT LILKA_SLASH)) {
        fs = static_cast<FS*>(sdfs);
    } else if (path.startsWith(LILKA_SPIFFS_ROOT LILKA_SLASH)) {
        fs = static_cast<FS*>(spiffs);
    }
    return fs;
}
const String FileUtils::getFullPath(const FS* fSysDriver, const String& path) {
    // Allready okay
    if (path.startsWith(LILKA_SDROOT LILKA_SLASH) || path.startsWith(LILKA_SPIFFS_ROOT LILKA_SLASH)) return path;

    if (fSysDriver == sdfs) {
        return String(LILKA_SDROOT) + path;
    } else if (fSysDriver == spiffs) {
        return String(LILKA_SPIFFS_ROOT) + path;
    }
    serial_err("Unknown fSysDriver provided");
    return path;
}

const String FileUtils::getRelativePath(const String& path) {
    String relativePath;
    if (path.startsWith(LILKA_SDROOT LILKA_SLASH)) {
        relativePath = path.substring(LILKA_SDROOT_LEN);
    } else if (path.startsWith(LILKA_SPIFFS_ROOT LILKA_SLASH)) {
        relativePath = path.substring(LILKA_SPIFFS_ROOT_LEN);
    } else
        // Maybe path is allready relative?
        relativePath = path;
    return relativePath;
}

bool FileUtils::isSDAvailable() {
    return !(sdfs->cardType() == CARD_NONE || sdfs->cardType() == CARD_UNKNOWN);
}

bool FileUtils::isSPIFFSAvailable() {
    return spiffsAvailable;
}

const String FileUtils::getSDRoot() {
    return LILKA_SDROOT;
}

const String FileUtils::getSPIFFSRoot() {
    return LILKA_SPIFFS_ROOT;
}
const String FileUtils::getHumanFriendlySize(const size_t size) {
    // Max length of file size

    const char* suffixes[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};
    const int numSuffixes = sizeof(suffixes) / sizeof(suffixes[0]);
    if (size == 0) return "0B";

    int exp = 0;
    double dsize = (double)(size);

    while (dsize >= 1024 && exp < numSuffixes - 1) {
        dsize /= 1024;
        exp++;
    }

    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.0f%s", dsize, suffixes[exp]);
    String hFileSize(buffer);
    while (hFileSize.length() != H_FILE_SIZE) {
        hFileSize = hFileSize + " ";
    }

    return hFileSize;
}

bool FileUtils::createSDPartTable() {
    sdMountLocked = true;
    // Unmount sd
    // We could have some io problems here, but user actually want to destroy all io
    // results, seems to be okay :)
    SD.end();
    constexpr uint32_t workbufSize = FF_MAX_SS * 4;
    uint8_t* workbuf = new uint8_t[workbufSize];
    std::unique_ptr<uint8_t[]> workbufPtr(workbuf);

    // mount without init
    SPI1.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    uint8_t pdrv = sdcard_init(LILKA_SDCARD_CS, &SPI1, 2000000);
    SPI1.endTransaction();
    if (pdrv == 0xFF) {
        sdMountLocked = false;
        return false;
    }

    // SD card uninitializer (RAII)
    std::unique_ptr<void, void (*)(void*)> sdcardUninit(nullptr, [](void* pdrv) {
        // C++ is beautiful and ugly at the same time
        SPI1.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
        sdcard_uninit(*static_cast<uint8_t*>(pdrv));
        SPI1.endTransaction();
    });

    // Create partition table

    DWORD plist[] = {100, 0, 0, 0};
    FRESULT res = f_fdisk(pdrv, plist, workbuf);
    if (res != FR_OK) {
        sdMountLocked = false;
        return false;
    }
    sdMountLocked = false;

    return true;
}

bool FileUtils::formatSD() {
    sdMountLocked = true;
    SD.end();

    constexpr uint32_t workbufSize = FF_MAX_SS * 4;
    uint8_t* workbuf = new uint8_t[workbufSize];

    std::unique_ptr<uint8_t[]> workbufPtr(workbuf);

    FRESULT res = f_mkfs(LILKA_SDROOT, FM_ANY, 0, workbuf, workbufSize);
    if (res != FR_OK) {
        sdMountLocked = false;
        return false;
    }
    sdMountLocked = false;

    return true;
}

FileUtils fileutils;
} // namespace lilka