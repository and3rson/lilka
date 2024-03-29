#pragma once

#include <SPIFFS.h>
#include <SD.h>
#include "config.h"
#define LILKA_SDROOT          "/sd"
#define LILKA_SPIFFS_ROOT     "/fs"
#define LILKA_SDROOT_LEN      3
#define LILKA_SPIFFS_ROOT_LEN 3

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

class FileSystem {
public:
    FileSystem();
    // This function tries to init both SD and SPIFFS
    // Filesystems.
    void init();
    // Tries to init SD if it's not initialized yet
    void initSD();
    // Tries to init SPIFFS if it's not initialized yet
    void initSPIFFS();
    uint32_t getEntryCount(const String& path);
    String stripPath(const String& path);
    size_t listDir(const String& path, Entry entries[]);
    bool sdAvailable();

    // Standart for FS class functions mapping
    // automatically converts full path in relative paths
    // for each filesystem
    File open(const char* path, const char* mode = FILE_READ, const bool create = false);
    File open(const String& path, const char* mode = FILE_READ, const bool create = false);

    bool exists(const char* path);
    bool exists(const String& path);

    bool remove(const char* path);
    bool remove(const String& path);

    bool rename(const char* pathFrom, const char* pathTo);
    bool rename(const String& pathFrom, const String& pathTo);

    bool mkdir(const char* path);
    bool mkdir(const String& path);

    bool rmdir(const char* path);
    bool rmdir(const String& path);

private:
    FS* getFSClassByPath(const String& path);
    String getRelativePath(const String& path);
    FS* spiffs;
    SDFS* sdfs;
};

extern FileSystem filesystem;

} // namespace lilka
