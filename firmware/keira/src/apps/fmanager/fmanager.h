#pragma once
///////////////////////////////////////////////////////////////////////////////////////////////////////
// [^_^]==\~ File manager for Keira OS header file                                                   //
///////////////////////////////////////////////////////////////////////////////////////////////////////
// Used Buttons:
// A                  -> Okay/Enter
// B                  -> Back/Exit
// C                  -> Select file/multiple files
// D                  -> Options
// Start              -> Confirm button
// Select             -> Info menu
// Start              -> Reload dir
///////////////////////////////////////////////////////////////////////////////////////////////////////

// BUTTONS:  //////////////////////////////////////////////////////////////////////////////////////////
#define FM_OKAY_BUTTON         lilka::Button::A
#define FM_CONFIRM_BUTTON      lilka::Button::START
#define FM_EXIT_BUTTON         lilka::Button::B
#define FM_OPTIONS_MENU_BUTTON lilka::Button::D
#define FM_INFO_BUTTON         lilka::Button::SELECT
#define FM_SELECT_BUTTON       lilka::Button::C
#define FM_RELOAD_BUTTON       FM_CONFIRM_BUTTON
///////////////////////////////////////////////////////////////////////////////////////////////////////

// COLORS:  ///////////////////////////////////////////////////////////////////////////////////////////
#define FT_NONE_COLOR       lilka::colors::Red
#define FT_NES_ROM_COLOR    lilka::colors::Candy_pink
#define FT_BIN_COLOR        lilka::colors::Mint_green
#define FT_LUA_SCRIPT_COLOR lilka::colors::Maya_blue
#define FT_JS_SCRIPT_COLOR  lilka::colors::Butterscotch
#define FT_MOD_COLOR        lilka::colors::Plum_web
#define FT_LT_COLOR         lilka::colors::Pink_lace
#define FT_DIR_COLOR        lilka::colors::Arylide_yellow
#define FT_OTHER_COLOR      lilka::colors::Light_gray
///////////////////////////////////////////////////////////////////////////////////////////////////////

// TODO: add HideItem into lilka::Menu

// TODO : Add separate icons to new file types

// ICONS:  ////////////////////////////////////////////////////////////////////////////////////////////
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
// FILE HANDLERS:  ////////////////////////////////////////////////////////////////////////////////////
#define FM_DEFAULT_FT_NES_HANDLER(X)     AppManager::getInstance()->runApp(new NesApp(X));
#define FM_DEFAULT_FT_BIN_HANDLER(X)     fileLoadAsRom(X);
#define FM_DEFAULT_LUA_SCRIPT_HANDLER(X) AppManager::getInstance()->runApp(new LuaFileRunnerApp(X));
#define FT_DEFAULT_JS_SCRIPT_HANDLER(X)  AppManager::getInstance()->runApp(new MJSApp(X));
#define FT_DEFAULT_MOD_HANDLER(X)        AppManager::getInstance()->runApp(new ModPlayerApp(X));
#define FT_DEFAULT_LT_HANDLER(X)         AppManager::getInstance()->runApp(new LilTrackerApp(X))
#define FT_DEFAULT_DIR_HANDLER           currentPath = path;
#define FT_DEFAULT_OTHER_HANDLER         fileInfoShowAlert();
///////////////////////////////////////////////////////////////////////////////////////////////////////

// MISC SETTINGS:  ////////////////////////////////////////////////////////////////////////////////////
#define PROGRESS_FRAME_TIME              30
#define PROGRESS_FILE_LIST_NO_DRAW_COUNT 10
// There's a big chance, that task won't be suspended immediately, which could cause a bug
// If ui hangs up after trying to open file, increase this value. TODO: implent this in AppManager
#define SUSPEND_AWAIT_TIME         100
#define FM_CHUNK_SIZE              256
#define FM_MKDIR_MODE              0777
#define FM_STACK_SIZE              8192
#define FM_STACK_MIN_FREE_SIZE     100
#define FM_DEFAULT_NEW_FOLDER_NAME "Нова папка"

// STATUS BAR SETTINGS:  //////////////////////////////////////////////////////////////////////////////
#define STATUS_BAR_HEIGHT        30
#define STATUS_BAR_SAFE_DISTANCE 38
#define STATUS_BAR_WIDTH         canvas->width() - STATUS_BAR_SAFE_DISTANCE * 2
#define STATUS_BAR_TEXT_COLOR    lilka::colors::White
#define STATUS_BAR_FILL_COLOR    lilka::colors::Black
#define FM_ERRNO_TIME            5000 // time to show last error
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define ENTRY_NOT_FOUND_INDEX UINT16_MAX
#define FM_CALLBACK_CAST(X)   reinterpret_cast<lilka::PMenuItemCallback>(&FileManagerApp::X)
#define FM_CALLBACK_PTHIS     reinterpret_cast<void*>(this)

// DEPS:
#include "appmanager.h"
#include <mbedtls/md5.h>
#include "esp_log.h"
#include "esp_err.h"
#include <errno.h>
#include "app.h"
#include <dirent.h>
#include <vector>
#include <sys/stat.h>
#include <stdint.h>

// APPS:
#include "../modplayer/modplayer.h"
#include "../liltracker/liltracker.h"
#include "../lua/luarunner.h"
#include "../mjs/mjsrunner.h"
#include "../nes/nesapp.h"
// ICONS:
#include "../icons/normalfile.h"
#include "../icons/folder.h"
#include "../icons/nes.h"
#include "../icons/bin.h"
#include "../icons/lua.h"
#include "../icons/js.h"
#include "../icons/music.h"

// very bad test
// /sd/1 => /sd/1122/1
// no need with status bar
#define MAKE_SANDWICH(X) \
    if (0) AppManager::getInstance()->startToast(X)

#ifdef FMANAGER_DEBUG
#    define FM_DBG if (1)
#else
#    define FM_DBG if (0)
#endif

typedef enum { FT_NONE, FT_NES_ROM, FT_BIN, FT_LUA_SCRIPT, FT_JS_SCRIPT, FT_MOD, FT_LT, FT_DIR, FT_OTHER } FileType;
typedef enum {
    FM_MODE_VIEW, // Standard mode
    FM_MODE_SELECT, // if selectedEntries contain something
    FM_MODE_RELOAD // changes in dir happened, reload filelist
} FmMode;

#define FM_UI_CANT_DO_OP                                                  \
    if (!exitChildDialogs) alert("Помилка", "Не можу виконати операцію"); \
    FM_DBG lilka::serial_err("FM operation fail at %s:%d", __FILE__, __LINE__)

#define FM_UI_SUCCESS_OP        \
    MAKE_SANDWICH("Виконано!"); \
    FM_DBG lilka::serial_log("FM operation success at %s:%d", __FILE__, __LINE__)

#define FM_UI_ADDED_TO_BUFFER MAKE_SANDWICH("Файл додано в буфер обміну")

#define FM_CHILD_DIALOG_CHECKB \
    if (exitChildDialogs) return false;

#define FM_CHILD_DIALOG_CHECKV \
    if (exitChildDialogs) return;

#define FM_MODE_RESET         \
    changeMode(FM_MODE_VIEW); \
    exitChildDialogs = true;  \
    FM_DBG lilka::serial_log("FM mode reset at %s:%d", __FILE__, __LINE__)

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
// Probably just use select button?
//////////////////////////////////////////////////////////////
// 4 Determine suggested block size(buffer) for filesystem
// operation using statvfs() See 1. Set non buffering flag on
// file to avoid buffering by vfs implementation.
//
// Well, we anyways need a buffer size detection to consume
// just a bit less memory, also we would be sure it's
// efficient.
//////////////////////////////////////////////////////////////
// 5. Check file type by mime-type instead of extension
//////////////////////////////////////////////////////////////
// 6. Avoid recursive calls. Our stack is finite
//  Build File List[Remember dirs] -> Sort dirs by count of / in
//  their paths -> make dirs -> copy other files
//////////////////////////////////////////////////////////////
// 7. Check name in mkdirInputShow and renameInputShow
// on wrong characters
//
// see FM_FAT32_NAMES_NOT_ALOWED_CHARACTERS and
// FM_FAT32_NAMES_NOT_ALLOWED_RESERVED_NAMES macro
//////////////////////////////////////////////////////////////

typedef struct {
    FileType type;
    const menu_icon_t* icon;
    uint16_t color;
    String name;
    String path; // dir
    __off_t st_size;
    __mode_t st_mode;
    bool selected = false;
} FMEntry; // Maybe switch to class?

class FileManagerApp : public App {
public:
    explicit FileManagerApp(const String& path);

    void queueDraw();

private:
    // Converts path into FMEntry
    static FMEntry pathToEntry(const String& path);

    // changes FM mode [FM_MODE_RELOAD, FM_MODE_VIEW, FM_MODE_SELECT]
    bool changeMode(FmMode newMode);

    // loads firmware
    void fileLoadAsRom(const String& path);
    String getFileMD5(const String& file_path);

    // open current entry with default app
    void openCurrentEntry();
    void selectCurrentEntry();
    void deselectCurrentEntry();
    void clearSelectedEntries();

    void deleteEntry(const FMEntry& entry, bool force = false);

    // actual copying
    bool copyPath(const String& source, const String& destination);
    bool movePath(const String& source, const String destination);

    // allert to not fall off on non-implemented features
    void alertNotImplemented();
    // ensure we've at least STACK_MIN_FREE_SIZE bytes free
    bool stackSizeCheck();

    // Main loop:
    void run() override;

    String currentPath;
    String initalPath; // used to track when to quit

    //  FM State:
    bool exitChildDialogs = false;
    FmMode mode = FM_MODE_RELOAD;
    FMEntry singleMoveCopyEntry = {};
    FMEntry currentEntry = {};

    // Buffer for file operations(Copy/MD5 Calc)
    unsigned char buffer[FM_CHUNK_SIZE] = {};

    // values for md5Progress and copyProgress
    ssize_t bytesReadChunk = 0;
    size_t bytesRead = 0;
    int progress = 0;
    int lastProgress = -1;
    int lastFrameTime = millis();

    // Manual draw
    void drawStatusBar();

    // Menu:
    lilka::Menu fileOpenWithMenu;
    lilka::Menu fileListMenu;
    lilka::Menu fileOptionsMenu;
    lilka::Menu fileSelectionOptionsMenu;

    // Dialogs:
    lilka::ProgressDialog dirLoadProgress;
    lilka::ProgressDialog md5Progress;
    lilka::ProgressDialog copyProgress;
    lilka::InputDialog mkdirInput;
    lilka::InputDialog renameInput;

    // Menu handlers:
    void fileOpenWithMenuShow();
    bool fileListMenuLoadDir();
    void fileListMenuShow();
    void fileOptionsMenuShow();
    void fileSelectionOptionsMenuShow();

    // Alerts:
    void fileInfoShowAlert();
    void alert(const String& title, const String& message);

    // Callbacks [any]:
    void onAnyMenuBack();

    // Callbacks [fileSelectionOptionsMenu]
    void onFileSelectionOptionsMenuCopy();
    void onFileSelectionOptionsMenuMove();
    void onFileSelectionOptionsMenuDelete();
    void onFileSelectionOptionsMenuClearSelection();

    // Calbacks [fileOptionsMenu]:
    void onFileOptionsMenuOpen();
    void onFileOptionsMenuOpenWith();
    void onFileOptionsMenuMKDir();
    void onFileOptionsMenuRename();
    void onFileOptionsMenuDelete();
    void onFileOptionsMenuInfo();

    // Callbacks [fileOpenWithMenu]:
    void onFileOpenWithFileManager();
    void onFileOpenWithNESEmulator();
    void onFileOpenWithMultiBootLoader();
    void onFileOpenWithLua();
    void onFileOpenWithMJS();
    void onFileOpenWithLilTracker();
    void onFileOpenWithMODPlayer();
    // Callbacks [fileListMenu]:
    void onFileListMenuItem();

    // Checks:
    static bool isCopyOrMoveCouldBeDone(const String& src, const String& dst);
    //TODO: static bool isValidFAT32Name(const String& name);

    // Comparators:
    static bool areDirEntriesEqual(const FMEntry& ent1, const FMEntry& ent2);

    // Search:
    // Returns ENTRY_NOT_FOUND_INDEX if not found
    static uint16_t getDirEntryIndex(const std::vector<FMEntry>& vec, const FMEntry& entry);

    std::vector<FMEntry> currentDirEntries;
    std::vector<FMEntry> selectedDirEntries;

    int errnoTime = 0;
    String errnoStr;
};