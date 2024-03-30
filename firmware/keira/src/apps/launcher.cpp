#include <ff.h>
#include <sd_diskio.h>

#include "launcher.h"
#include "appmanager.h"

#include "lilka/config.h"
#include "servicemanager.h"
#include "services/network.h"

#include "wifi_config.h"
#include "demos/lines.h"
#include "demos/disk.h"
#include "demos/ball.h"
#include "demos/transform.h"
#include "demos/cube.h"
#include "demos/epilepsy.h"
#include "demos/letris.h"
#include "tamagotchi/tamagotchi.h"
#include "demos/keyboard.h"
#include "demos/user_spi.h"
#include "demos/scan_i2c.h"
#include "lua/luarunner.h"
#include "mjs/mjsrunner.h"
#include "nes/nesapp.h"
#include "ftp/ftp_server.h"

#include "icons/demos.h"
#include "icons/sdcard.h"
#include "icons/memory.h"
#include "icons/dev.h"
#include "icons/settings.h"
#include "icons/info.h"

#include "icons/app_group.h"
#include "icons/app.h"
#include "icons/normalfile.h"
#include "icons/folder.h"
#include "icons/nes.h"
#include "icons/bin.h"
#include "icons/lua.h"
#include "icons/js.h"

LauncherApp::LauncherApp() : App("Menu") {
}

ITEM_LIST app_items = {
    ITEM_SUBMENU(
        "Демо",
        {
            ITEM_APP("Лінії", DemoLines),
            ITEM_APP("Диск", DiskApp),
            ITEM_APP("Перетворення", TransformApp),
            ITEM_APP("М'ячик", BallApp),
            ITEM_APP("Куб", CubeApp),
            ITEM_APP("Епілепсія", EpilepsyApp),
        }
    ),
    ITEM_SUBMENU(
        "Тести",
        {
            ITEM_APP("Клавіатура", KeyboardApp),
            ITEM_APP("Тест SPI", UserSPIApp),
            ITEM_APP("I2C-сканер", ScanI2CApp),
        },
    ),
    ITEM_APP("Летріс", LetrisApp),
    ITEM_APP("Тамагочі", TamagotchiApp),
};

ITEM_LIST dev_items = {
    ITEM_APP("Live Lua", LuaLiveRunnerApp),
    ITEM_APP("Lua REPL", LuaReplApp),
    ITEM_APP("FTP сервер", FTPServerApp),
};

void LauncherApp::run() {
    lilka::Menu menu("Головне меню");
    menu.addItem("Додатки", &demos, lilka::colors::Pink);
    menu.addItem("Браузер SD-карти", &sdcard, lilka::colors::Arylide_yellow);
    menu.addItem("Браузер SPIFFS", &memory, lilka::colors::Dark_sea_green);
    menu.addItem("Розробка", &dev, lilka::colors::Jasmine);
    menu.addItem("Налаштування", &settings, lilka::colors::Orchid);

    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == 0) {
            appsMenu("Додатки", app_items);
        } else if (index == 1) {
            sdBrowserMenu("/");
        } else if (index == 2) {
            spiffsBrowserMenu();
        } else if (index == 3) {
            appsMenu("Розробка", dev_items);
        } else if (index == 4) {
            settingsMenu();
        }
    }
}

void LauncherApp::appsMenu(const char* title, ITEM_LIST& list) {
    int itemCount = list.size();
    lilka::Menu menu(title);
    for (int i = 0; i < list.size(); i++) {
        menu.addItem(list[i].name, list[i].type == APP_ITEM_TYPE_SUBMENU ? &app_group : &app);
    }
    menu.addItem("<< Назад");
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == itemCount) {
            break;
        }
        item_t item = list[index];
        if (item.type == APP_ITEM_TYPE_SUBMENU) {
            appsMenu(item.name, item.submenu);
        } else {
            AppManager::getInstance()->runApp(list[index].construct());
        }
    }
}

const menu_icon_t* get_file_icon(const String& filename) {
    String lowerCasedFileName = filename;
    lowerCasedFileName.toLowerCase();
    if (lowerCasedFileName.endsWith(".rom") || lowerCasedFileName.endsWith(".nes")) {
        return &nes;
    } else if (lowerCasedFileName.endsWith(".bin")) {
        return &bin;
    } else if (lowerCasedFileName.endsWith(".lua")) {
        return &lua;
    } else if (lowerCasedFileName.endsWith(".js")) {
        return &js;
    } else {
        return &normalfile;
    }
}

const uint16_t get_file_color(const String& filename) {
    String lowerCasedFileName = filename;
    lowerCasedFileName.toLowerCase();
    if (lowerCasedFileName.endsWith(".rom") || lowerCasedFileName.endsWith(".nes")) {
        return lilka::colors::Candy_pink;
    } else if (lowerCasedFileName.endsWith(".bin")) {
        return lilka::colors::Mint_green;
    } else if (lowerCasedFileName.endsWith(".lua")) {
        return lilka::colors::Maya_blue;
    } else if (lowerCasedFileName.endsWith(".js")) {
        return lilka::colors::Butterscotch;
    } else {
        return lilka::colors::Light_gray;
    }
}

void LauncherApp::sdBrowserMenu(String path) {
    if (!lilka::sdcard.available()) {
        alert("Помилка", "SD-карта не знайдена");
    }
    size_t _numEntries = lilka::sdcard.getEntryCount(path);
    if (_numEntries == 0) {
        alert("Помилка", "Директорія пуста або сталася помилка читання директорії");
        return;
    }

    lilka::Entry* entries = new lilka::Entry[_numEntries];

    int numEntries = lilka::sdcard.listDir(path, entries);
    std::unique_ptr<lilka::Entry[]> entriesPtr(entries);

    // Так як listDir має повертати -1 в разі помилки
    // а countFilesIndir size_t >= 0 додаткові перевірки не потрібні
    if (_numEntries != numEntries) {
        alert("Помилка", "Не вдалося прочитати директорію");
        return;
    }

    lilka::Menu menu("SD: " + path);
    for (int i = 0; i < numEntries; i++) {
        String filename = entries[i].name;
        const menu_icon_t* icon =
            entries[i].type == lilka::EntryType::ENT_DIRECTORY ? &folder : get_file_icon(filename);
        uint16_t color = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? lilka::colors::Arylide_yellow
                                                                            : get_file_color(filename);
        menu.addItem(filename, icon, color);
    }
    menu.addItem("<< Назад", 0, 0);

    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == numEntries) break;
        if (entries[index].type == lilka::EntryType::ENT_DIRECTORY) {
            sdBrowserMenu(path + entries[index].name + "/");
        } else {
            selectFile(lilka::sdcard.abspath(path + entries[index].name));
        }
    }

    return;
}

void LauncherApp::spiffsBrowserMenu() {
    if (!lilka::filesystem.available()) {
        alert("Помилка", "SPIFFS не підтримується");
        return;
    }

    String filenames
        [32]; // TODO - allocate dynamically (increasing to 64 causes task stack overflow which is limited by ARDUINO_LOOP_STACK_SIZE)
    int numEntries = lilka::filesystem.readdir(filenames);

    if (numEntries == -1) {
        alert("Помилка", "Не вдалося прочитати корінь SPIFFS");
        return;
    }
    const menu_icon_t* icons[32];
    uint16_t colors[32];
    for (int i = 0; i < numEntries; i++) {
        icons[i] = get_file_icon(filenames[i]);
        colors[i] = get_file_color(filenames[i]);
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;
    colors[numEntries - 1] = 0;

    lilka::Menu menu("SPIFFS");
    for (int i = 0; i < numEntries; i++) {
        menu.addItem(filenames[i], icons[i], colors[i]);
    }
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == numEntries - 1) {
            return;
        }
        selectFile(lilka::filesystem.abspath(filenames[index]));
    }
}

void LauncherApp::selectFile(String path) {
    String lowerCasedPath = path;
    lowerCasedPath.toLowerCase();
    if (lowerCasedPath.endsWith(".rom") || lowerCasedPath.endsWith(".nes")) {
        AppManager::getInstance()->runApp(new NesApp(path));
    } else if (lowerCasedPath.endsWith(".bin")) {
#if LILKA_VERSION < 2
        alert("Помилка", "Ця операція потребує Лілку 2.0");
        return;
#else
        lilka::ProgressDialog dialog("Завантаження", path + "\n\nПочинаємо...");
        dialog.draw(canvas);
        queueDraw();
        int error;
        error = lilka::multiboot.start(path);
        if (error) {
            alert("Помилка", String("Етап: 1\nКод: ") + error);
            return;
        }
        dialog.setMessage(path + "\n\nРозмір: " + String(lilka::multiboot.getBytesTotal()) + " Б");
        dialog.draw(canvas);
        queueDraw();
        while ((error = lilka::multiboot.process()) > 0) {
            int progress = lilka::multiboot.getBytesWritten() * 100 / lilka::multiboot.getBytesTotal();
            dialog.setProgress(progress);
            dialog.draw(canvas);
            queueDraw();
            if (lilka::controller.getState().a.justPressed) {
                lilka::multiboot.cancel();
                return;
            }
        }
        if (error < 0) {
            alert("Помилка", String("Етап: 2\nКод: ") + error);
            return;
        }
        error = lilka::multiboot.finishAndReboot();
        if (error) {
            alert("Помилка", String("Етап: 3\nКод: ") + error);
            return;
        }
#endif
    } else if (lowerCasedPath.endsWith(".lua")) {
        AppManager::getInstance()->runApp(new LuaFileRunnerApp(path));
    } else if (lowerCasedPath.endsWith(".js")) {
        AppManager::getInstance()->runApp(new MJSApp(path));
    } else {
        // Get file size
        FILE* file = fopen(path.c_str(), "r");
        if (!file) {
            alert("Помилка", "Не вдалося відкрити файл");
            return;
        }
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        alert(path, String("Розмір:\n") + size + " байт");
    }
}

void LauncherApp::settingsMenu() {
    String titles[] = {
        "WiFi",
        "Про систему",
        "Інфо про пристрій",
        "Таблиця розділів",
        "Форматування SD-карти",
        "Light sleep",
        "Deep sleep",
        "Перезавантаження",
        "<< Назад",
    };
    int count = sizeof(titles) / sizeof(titles[0]);
    lilka::Menu menu("Системні утиліти");
    menu.addActivationButton(lilka::Button::B);
    for (int i = 0; i < count; i++) {
        menu.addItem(titles[i]);
    }
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        if (menu.getButton() == lilka::Button::B) {
            return;
        }
        int16_t index = menu.getCursor();
        if (index == count - 1) {
            return;
        }
        if (index == 0) {
            AppManager::getInstance()->runApp(new WiFiConfigApp());
        } else if (index == 1) {
            alert("Keira OS", "by Андерсон & friends");
        } else if (index == 2) {
            char buf[256];
            NetworkService* networkService =
                static_cast<NetworkService*>(ServiceManager::getInstance()->getService<NetworkService>("network"));
            // TODO: use dynamic_cast and assert networkService != nullptr
            sprintf(
                buf,
                "Модель: %s\n"
                "Ревізія: %d\n"
                "Версія ESP-IDF: %s\n"
                "Частота: %d МГц\n"
                "Кількість ядер: %d\n"
                "IP: %s",
                ESP.getChipModel(),
                ESP.getChipRevision(),
                esp_get_idf_version(),
                ESP.getCpuFreqMHz(),
                ESP.getChipCores(),
                networkService->getIpAddr().c_str()
            );
            alert("Інфо про пристрій", buf);
        } else if (index == 3) {
            String labels[16];
            int labelCount = lilka::sys.get_partition_labels(labels);
            labels[labelCount++] = "<< Назад";
            lilka::Menu partitionMenu("Таблиця розділів");
            for (int i = 0; i < labelCount; i++) {
                partitionMenu.addItem(labels[i]);
            }
            while (1) {
                while (!partitionMenu.isFinished()) {
                    partitionMenu.update();
                    partitionMenu.draw(canvas);
                    queueDraw();
                }
                int16_t partitionIndex = partitionMenu.getCursor();
                if (partitionIndex == labelCount - 1) {
                    break;
                }
                alert(
                    labels[partitionIndex],
                    String("Адреса: 0x") +
                        String(lilka::sys.get_partition_address(labels[partitionIndex].c_str()), HEX) + "\n" +
                        "Розмір: 0x" + String(lilka::sys.get_partition_size(labels[partitionIndex].c_str()), HEX)
                );
            }
        } else if (index == 4) {
            // Init card in VFS without actually mounting it
            lilka::SPI1.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
            uint8_t pdrv = sdcard_init(LILKA_SDCARD_CS, &lilka::SPI1, 4000000);
            lilka::SPI1.endTransaction();
            if (pdrv == 0xFF) {
                alert("Помилка", "Не вдалося ініціалізувати SD-карту");
                continue;
            }

            // SD card uninitializer (RAII)
            std::unique_ptr<void, void (*)(void*)> sdcardUninit(nullptr, [](void* pdrv) {
                // C++ is beautiful and ugly at the same time
                lilka::SPI1.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
                sdcard_uninit(*static_cast<uint8_t*>(pdrv));
                lilka::SPI1.endTransaction();
            });

            // Check if the user is just messing around
            lilka::Alert confirm(
                "Форматування",
                "УВАГА: Це очистить ВСІ дані з SD-карти!\n\nПродовжити?\n\nSTART - продовжити\nA - скасувати"
            );
            confirm.addActivationButton(lilka::Button::START);
            confirm.draw(canvas);
            queueDraw();
            while (!confirm.isFinished()) {
                confirm.update();
                taskYIELD();
            }
            if (confirm.getButton() != lilka::Button::START) {
                continue;
            }

            // Allocate work buffer for FatFS library (4 sectors)
            // Otherwise f_mkfs tries to allocate in stack and fails due to task stack size
            constexpr uint32_t workbufSize = FF_MAX_SS * 4;
            uint8_t* workbuf = new uint8_t[workbufSize];
            std::unique_ptr<uint8_t[]> workbufPtr(workbuf);

            // Create partition table
            DWORD plist[] = {100, 0, 0, 0};
            FRESULT res = f_fdisk(pdrv, plist, workbuf);
            if (res != FR_OK) {
                this->alert("Помилка", "Не вдалося записати таблицю розділів, код помилки: " + String(res));
                continue;
            }
            lilka::ProgressDialog dialog("Форматування", "Будь ласка, зачекайте...");
            dialog.draw(canvas);
            queueDraw();

            // Format the partition
            res = f_mkfs("/sd", FM_ANY, 0, workbuf, workbufSize);
            if (res != FR_OK) {
                this->alert("Помилка", "Не вдалося сформатувати SD-карту, код помилки: " + String(res));
                continue;
            }

            // This code does not work since f_setlabel is not implemented.
            // Keeping it here for future reference.
            // char label[16];
            // sprintf(label, "%d:Keira", pdrv);
            // if (f_setlabel(const_cast<char*>(label)) != FR_OK) {
            //     this->alert("Помилка", "Не вдалося встановити мітку розділу");
            //     continue;
            // }

            this->alert("Форматування", "Форматування SD-карти завершено!");
        } else if (index == 5) {
            esp_light_sleep_start();
        } else if (index == 6) {
            ESP.deepSleep(0xFFFFFFFF);
        } else if (index == 7) {
            esp_restart();
        }
    }
}

void LauncherApp::alert(String title, String message) {
    lilka::Alert alert(title, message);
    alert.draw(canvas);
    queueDraw();
    while (!alert.isFinished()) {
        alert.update();
        taskYIELD();
    }
}
