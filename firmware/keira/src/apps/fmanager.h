#pragma once
#include "app.h"
#include <dirent.h>
#include <vector>

typedef enum { FT_NES_ROM, FT_BIN, FT_LUA_SCRIPT, FT_JS_SCRIPT, FT_MOD, FT_LT, FT_OTHER } FileType;

// We already 've similar structure in lilka/sdk/fileutils,
// though cause we use here a bit different way to achieve same results
// we could add some additional data provided by stat()
typedef struct {
    FileType type;
    const menu_icon_t* icon;
    uint16_t color;
    //long int size;  >>>> stat.st_size
    String name;
    struct stat stat;
} FMEntry;

class FileManagerApp : public App {
public:
    FileManagerApp(FS* fSysDriver, const String& path);

private:
    FileType detectFileType(const String& filename);
    void readDir(const String& path);
    void loadRom(const String& path);
    const menu_icon_t* getFileIcon(const String& filename);
    const uint16_t getFileColor(const String& filename);
    void openFile(const String& path);
    void showFileInfo(const String& path);
    // TODO: void openFileWith(String &path);
    String getFileMD5(const String& file_path);
    String currentPath;
    String menuPrefix;
    void run() override;
    void alert(const String& title, const String& message);
};
