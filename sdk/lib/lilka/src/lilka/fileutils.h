#pragma once
#include <ff.h>
#include "config.h"
#include <SPIFFS.h>
#include <SD.h>
#include "config.h"
#include <sd_diskio.h>

#define LILKA_SDROOT          "/sd"
#define LILKA_SPIFFS_ROOT     "/fs"
#define LILKA_SLASH           "/"
#define LILKA_SDROOT_LEN      3
#define LILKA_SPIFFS_ROOT_LEN 3
#define H_FILE_SIZE           6
namespace lilka {

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
    void initSD();
    // Tries to init SPIFFS if it's not initialized yet
    void initSPIFFS();
    uint32_t getEntryCount(FS* fSysDriver, const String& path);
    // Get stdlib compatible paths
    const String getSDRoot();
    const String getSPIFFSRoot();
    // Check Filesystems avalibility
    bool isSDAvailable();
    bool isSPIFFSAvailable();
    // Helper functions for path
    const String getFullPath(const FS* fSysDriver, const String& path);
    const String getRelativePath(const String& path);
    const String stripPath(const String& path);
    // Get lilka::Entry[] for dir
    size_t listDir(FS* fSysDriver, const String& path, Entry entries[]);
    // Get FS* for a full path
    FS* getFSysDriverByFullPath(const String& path);
    // SD Utils:
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
