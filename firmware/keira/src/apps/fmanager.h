#pragma once
//////////////////////////////////////////////////////////////
// File manager for Keira OS header file
//////////////////////////////////////////////////////////////
// Used Buttons:
// A -> Okay/Enter
// B -> Back/Exit
// C -> Info
// D -> Options
// Start -> Force Okay (Delete)
// Start -> Paste (Move/Copy)
//////////////////////////////////////////////////////////////
#include "app.h"
#include <dirent.h>
#include <vector>
#include <sys/stat.h>

#define MAKE_SANDWICH(X) AppManager::getInstance()->startToast(X)

#ifdef FMANAGER_DEBUG
#    define FM_DBG if (1)
#else
#    define FM_DBG if (0)
#endif

// There's a big chance, that task won't be suspended immediately, which could cause a bug
// If ui hangs up after trying to open file, increase this value
#define SUSPEND_AWAIT_TIME  100

#define FM_CHUNK_SIZE       256
#define FM_MKDIR_MODE       0777
#define STACK_MIN_FREE_SIZE 100

typedef enum { FT_NONE, FT_NES_ROM, FT_BIN, FT_LUA_SCRIPT, FT_JS_SCRIPT, FT_MOD, FT_LT, FT_DIR, FT_OTHER } FileType;
typedef enum { FM_MODE_VIEW, FM_MODE_SELECT, FM_MODE_COPY_SINGLE, FM_MODE_MOVE_SINGLE } FmMode;

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
#define FM_SELECTED_ICON   &music_img // yeah yeah I know
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////
// TODO FManager list:
//////////////////////////////////////////////////////////////
// 1. Get stats about filesystem
// statvfs() should work for any path, so
// I consider is a good choice here
// this part seems to be a good alternative for an
// info button, which works on a file now, so instead of
// info about file we could show info about filesystem
// where file is located. Also there's a possibility to move
// it in an Options menu
//////////////////////////////////////////////////////////////
// 2. Different sortings "by name, by size, by type"
// implement ascending/descending order
//////////////////////////////////////////////////////////////
// 3. Select/Select All features
//////////////////////////////////////////////////////////////
// 4. Flaten dir feature
// [recursive open all directories and make a single file list]
// should be expensive so probably better not to try. See 6
//////////////////////////////////////////////////////////////
// 5. Check file type by mime-type instead of extension
//////////////////////////////////////////////////////////////
// 6. Avoid recursive calls. Our stack is finite
//  Build File List[Remember dirs] -> Sort dirs by count of / in
//  their paths -> make dirs -> copy other files
//////////////////////////////////////////////////////////////
// 7. Check name in mkdirInputShow and renameInputShow
//  on wrong characters
//////////////////////////////////////////////////////////////

typedef struct {
    FileType type;
    const menu_icon_t* icon;
    uint16_t color;
    String name;
    String path; // dir
    struct stat stat;
} FMEntry; // Maybe switch to class?

class FileManagerApp : public App {
public:
    explicit FileManagerApp(const String& path);

private:
    FMEntry pathToEntry(const String& path);
    bool changeMode(FmMode newMode);

    void fileLoadAsRom(const String& path);
    String getFileMD5(const String& file_path);

    void openFileEntry(const FMEntry& entry);
    void alert(const String& title, const String& message);
    // Uses inputDialog to perform file or directory rename
    // just deletes file or directory. flag force means no ask from user
    // which is used here in a recursive form to remove all files in a dir
    void deleteEntry(const FMEntry& entry, bool force = false);

    // sets singleMoveCopyEntry value and enters FM_MODE_MODE_COPY_SINGLE
    void copySingleEntry(const FMEntry& entry);
    // sets singleMoveCopyEntry value and enters FM_MODE_MOVE_SINGLE
    void moveSingleEntry(const FMEntry& entry);
    // This function ends both move and copy actions
    void pasteSingleEntry(const FMEntry& entry, String& where);
    // though should use this one for actual copying cause POSIX
    // made it a bit complicated
    bool copyPath(const String& source, const String& destination);

    // allert to not fall off on non-implemented features
    void alertNotImplemented();
    // ensure we've at least STACK_MIN_FREE_SIZE bytes free
    bool stackSizeCheck();
    // Main loop:
    void run() override;

    String currentPath;

    //  FM State:
    bool exitChildDialogs = false;
    FmMode mode = FM_MODE_VIEW;
    FMEntry singleMoveCopyEntry = {};

    // Buffer for file operations(Copy/MD5 Calc)
    unsigned char buffer[FM_CHUNK_SIZE] = {0};

    // values for md5Progress and copyProgress
    ssize_t bytesReadChunk = 0;
    size_t bytesRead = 0;
    int progress = 0;

    // Dialogs and Menus:
    lilka::Menu fileOpenWithMenu;
    lilka::Menu fileListMenu;
    lilka::Menu fileOptionsMenu;
    lilka::ProgressDialog md5Progress;
    lilka::ProgressDialog copyProgress;
    lilka::InputDialog mkdirInput;
    lilka::InputDialog renameInput;

    // Menu handlers:
    void fileOpenWithMenuShow(const FMEntry& entry);

    bool fileListMenuLoadDir(const String& path);
    void fileListMenuShow(const String& path);

    void fileOptionsMenuShow(const FMEntry& entry);

    // Input handlers:
    void mkdirInputShow(const String& path);

    void renameInputShow(const FMEntry& entry);

    // Alerts:
    void fileInfoShowAlert(const FMEntry& entry);

    std::vector<FMEntry> currentDirEntries;
};
