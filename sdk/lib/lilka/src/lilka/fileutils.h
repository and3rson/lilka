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
    // This function tries to init both SD and SPIFFS
    // Filesystems.
    void init();
    // Tries to init SD if it's not initialized yet
    bool initSD();
    // Tries to init SPIFFS if it's not initialized yet
    void initSPIFFS();
    uint32_t getEntryCount(FS* fSysDriver, const String& relPath);
    // Get stdlib compatible paths
    const String getSDRoot();
    const String getSPIFFSRoot();
    // Check Filesystems avalibility
    bool isSDAvailable();
    bool isSPIFFSAvailable();
    // Helper functions for path
    const String getFullPath(const FS* fSysDriver, const String& relPath);
    const PathInfo getRelativePathInfo(const String& absPath);
    const String stripPath(const String& path);
    // Get lilka::Entry[] for dir
    size_t listDir(FS* fSysDriver, const String& relPath, Entry entries[]);
    // SD Utils:
    // SD Card format
    // Note: system should be rebooted anyway, after call
    // to any of these methods
    bool createSDPartTable();
    bool formatSD();
    // Misc:
    const String getHumanFriendlySize(const size_t size);

private:
    bool sdMountLocked = false;
    bool spiffsAvailable = false;
    FS* spiffs;
    SDFS* sdfs;
};

extern FileUtils fileutils;

} // namespace lilka
