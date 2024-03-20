#include <ff.h>

#include "launcher.h"
#include "appmanager.h"

#include "servicemanager.h"
#include "services/network.h"

#include "wifi_config.h"
#include "demos/lines.h"
#include "demos/disk.h"
#include "demos/ball.h"
#include "demos/transform.h"
#include "demos/epilepsy.h"
#include "demos/letris.h"
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

#include "icons/normalfile.h"
#include "icons/folder.h"
#include "icons/nes.h"
#include "icons/bin.h"
#include "icons/lua.h"
#include "icons/js.h"

LauncherApp::LauncherApp() : App("Menu") {
}

void LauncherApp::run() {
    lilka::Menu menu("Головне меню");
    menu.addItem("Додатки", &demos, lilka::display.color565(255, 200, 200));
    menu.addItem("Браузер SD-карти", &sdcard, lilka::display.color565(255, 255, 200));
    menu.addItem("Браузер SPIFFS", &memory, lilka::display.color565(200, 255, 200));
    menu.addItem("Розробка", &dev, lilka::display.color565(255, 224, 128));
    menu.addItem("Налаштування", &settings, lilka::display.color565(255, 200, 224));

    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == 0) {
            appsMenu();
        } else if (index == 1) {
            sdBrowserMenu("/");
        } else if (index == 2) {
            spiffsBrowserMenu();
        } else if (index == 3) {
            // dev_menu();
            devMenu();
        } else if (index == 4) {
            settingsMenu();
        }
    }
}

void LauncherApp::appsMenu() {
    APP_ITEM_LIST app_items = {
        APP_ITEM("Лінії", DemoLines),
        APP_ITEM("Диск", DiskApp),
        APP_ITEM("Перетворення", TransformApp),
        APP_ITEM("М'ячик", BallApp),
        APP_ITEM("Епілепсія", EpilepsyApp),
        APP_ITEM("Летріс", LetrisApp),
        APP_ITEM("Клавіатура", KeyboardApp),
        APP_ITEM("Тест SPI", UserSPIApp),
        APP_ITEM("I2C-сканер", ScanI2CApp),
    };
    int appCount = app_items.size();
    lilka::Menu menu("Демо");
    for (int i = 0; i < app_items.size(); i++) {
        menu.addItem(app_items[i].name);
    }
    menu.addItem("<< Назад");
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == appCount) {
            break;
        }
        AppManager::getInstance()->runApp(app_items[index].construct());
    }
}

const menu_icon_t* get_file_icon(const String& filename) {
    if (filename.endsWith(".rom") || filename.endsWith(".nes")) {
        return &nes;
    } else if (filename.endsWith(".bin")) {
        return &bin;
    } else if (filename.endsWith(".lua")) {
        return &lua;
    } else if (filename.endsWith(".js")) {
        return &js;
    } else {
        return &normalfile;
    }
}

const uint16_t get_file_color(const String& filename) {
    if (filename.endsWith(".rom") || filename.endsWith(".nes")) {
        return lilka::display.color565(255, 128, 128);
    } else if (filename.endsWith(".bin")) {
        return lilka::display.color565(128, 255, 128);
    } else if (filename.endsWith(".lua")) {
        return lilka::display.color565(128, 128, 255);
    } else if (filename.endsWith(".js")) {
        return lilka::display.color565(255, 200, 128);
    } else {
        return lilka::display.color565(200, 200, 200);
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
        uint16_t color = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? lilka::display.color565(255, 255, 200)
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
        if (index >= numEntries - 1) break;
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
    if (path.endsWith(".rom") || path.endsWith(".nes")) {
        AppManager::getInstance()->runApp(new NesApp(path));
    } else if (path.endsWith(".bin")) {
#if LILKA_VERSION < 2
        alert("Помилка", "Ця операція потребує Лілку 2.0");
        return;
#else
        int error;
        error = lilka::multiboot.start(path);
        if (error) {
            alert("Помилка", String("Етап: 1\nКод: ") + error);
            return;
        }
        lilka::ProgressDialog dialog(
            "Завантаження", path + "\nРозмір: " + String(lilka::multiboot.getBytesTotal()) + " Б"
        );
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
    } else if (path.endsWith(".lua")) {
        AppManager::getInstance()->runApp(new LuaFileRunnerApp(path));
    } else if (path.endsWith(".js")) {
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

void LauncherApp::devMenu() {
    APP_ITEM_LIST app_items = {
        APP_ITEM("Live Lua", LuaLiveRunnerApp),
        APP_ITEM("Lua REPL", LuaReplApp),
        APP_ITEM("FTP сервер", FTPServerApp),
    };
    int appCount = app_items.size();
    lilka::Menu menu("Розробка");
    for (int i = 0; i < app_items.size(); i++) {
        menu.addItem(app_items[i].name);
    }
    menu.addItem("<< Назад");
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
        }
        int16_t index = menu.getCursor();
        if (index == appCount) {
            return;
        }
        AppManager::getInstance()->runApp(app_items[index].construct());
    }
}

void LauncherApp::settingsMenu() {
    String titles[] = {
        "WiFi",
        "Про систему",
        "Інфо про пристрій",
        "Таблиця розділів",
        "Форматування SD-карти",
        "Перезавантаження",
        "<< Назад",
    };
    int count = sizeof(titles) / sizeof(titles[0]);
    lilka::Menu menu("Системні утиліти");
    for (int i = 0; i < count; i++) {
        menu.addItem(titles[i]);
    }
    while (1) {
        while (!menu.isFinished()) {
            menu.update();
            menu.draw(canvas);
            queueDraw();
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
                static_cast<NetworkService*>(ServiceManager::getInstance()->getService<NetworkService>());
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
            if (!lilka::sdcard.available()) {
                alert("Помилка", "SD-карта не знайдена");
                continue;
            }
            lilka::Alert confirm(
                "Форматування", "УВАГА: Це очистить ВСІ дані з SD-карти!\n\nПродовжити?\n\nSTART - так\nA - скасувати"
            );
            confirm.draw(canvas);
            queueDraw();
            while (!confirm.isFinished()) {
                confirm.update();
                taskYIELD();
            }
            if (confirm.getButton() != lilka::Button::START) {
                continue;
            }
            lilka::ProgressDialog dialog("Форматування", "Будь ласка, зачекайте...");
            dialog.draw(canvas);
            queueDraw();
            const uint32_t workSize = FF_MAX_SS * 4;
            void* work = ps_malloc(workSize
            ); // Buffer (4 sectors), otherwise f_mkfs tries to allocate in stack and fails due to task stack size
            FRESULT result = f_mkfs("/sd", FM_ANY, 0, work, workSize); // TODO - hardcoded mountpoint
            free(work);
            if (result != FR_OK) {
                this->alert("Помилка", "Не вдалося сформатувати SD-карту, код помилки: " + String(result));
                continue;
            }
            this->alert("Форматування", "Форматування SD-карти завершено!");

        } else if (index == 5) {
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
