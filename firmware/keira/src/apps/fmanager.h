#pragma once
#include "app.h"
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#define MD5_CHUNK_SIZE 1024

//////////////////////////////////////////////////////////////
// File manager for Keira OS header file
//////////////////////////////////////////////////////////////
// Used Buttons:
// A -> Okay/Enter
// B -> Back/Exit
// C -> Info
// D -> Options
// Start -> Force Okay (Delete)
// Start -> Paste (Move/Copy) // To implement
//////////////////////////////////////////////////////////////

typedef enum { FT_NONE, FT_NES_ROM, FT_BIN, FT_LUA_SCRIPT, FT_JS_SCRIPT, FT_MOD, FT_LT, FT_DIR, FT_OTHER } FileType;
typedef enum { FM_MODE_VIEW, FM_MODE_SELECT } FmMode;

// COLORS: ///////////////////////////////////////////////////
#define FT_NONE_COLOR       lilka::colors::Red
#define FT_NES_ROM_COLOR    lilka::colors::Candy_pink
#define FT_BIN_COLOR        lilka::colors::Mint_green
#define FT_LUA_SCRIPT_COLOR lilka::colors::Maya_blue
#define FT_JS_SCRIPT_COLOR  lilka::colors::Butterscotch
#define FT_MOD_COLOR        lilka::colors::Plum_web
#define FT_LT_COLOR         lilka::colors::Pink_lace
#define FT_DIR_COLOR        lilka::colors::Arylide_yellow
#define FT_OTHER_COLOR      lilka::colors::Light_gray
//////////////////////////////////////////////////////////////

// TODO : Add separate icons to new file types
// ICONS: ////////////////////////////////////////////////////
#define FT_NONE_ICON       &normalfile_img
#define FT_NES_ICON        &nes_img
#define FT_BIN_ICON        &bin_img
#define FT_LUA_SCRIPT_ICON &lua_img
#define FT_JS_SCRIPT_ICON  &js_img
#define FT_MOD_ICON        &music_img
#define FT_LT_ICON         &music_img
#define FT_DIR_ICON        &folder_img
#define FT_OTHER_ICON      &normalfile_img
//////////////////////////////////////////////////////////////

// We already 've similar structure in lilka/sdk/fileutils,
// though cause we use here a bit different way to achieve same results
// we could add some additional data provided by stat()
typedef struct {
    FileType type;
    const menu_icon_t* icon;
    uint16_t color;
    //long int size;  >>>> stat.st_size
    String name;
    String path; // dir
    struct stat stat;
} FMEntry; // Maybe switch to class?

class FileManagerApp : public App {
public:
    FileManagerApp(FS* fSysDriver, const String& path);

private:
    FMEntry pathToEntry(const String& path);
    void selectPath(const String& filename);
    void deselectPath(const String& filename);
    bool isSelectedPath(const String& filename);

    // UI:
    void showEntryOptions(const FMEntry& entry);
    void readDir(const String& path);
    void loadRom(const String& path);
    String getFileMD5(const String& file_path);
    void showEntryInfo(const FMEntry& entry);
    void openEntryWith(const FMEntry& entry);
    void openEntry(const FMEntry& entry);
    void alert(const String& title, const String& message);

    void renameEntry(const FMEntry& entry);
    void deleteEntry(const FMEntry& entry, bool force = false);
    void copyEntry(const FMEntry& entry, const String& path);
    void moveEntry(const FMEntry& entry, const String& path);
    void makeDir(const String& path); // path where to create dir, not a path to dir
    void alertNotImplemented();

    // Main loop:
    void run() override;

    // Storage:
    String currentPath;
    String menuPrefix;
    std::vector<String> selectedPaths;
    //  State:
    bool exitChildDialogs = false;
    FmMode mode = FM_MODE_VIEW;
};
