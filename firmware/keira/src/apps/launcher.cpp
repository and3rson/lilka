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
        menu.update();
        menu.draw(canvas);
        queueDraw();
        int16_t index = menu.getSelectedIndex();
        if (index != -1) {
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
        taskYIELD();
    }
}

void LauncherApp::appsMenu() {
    String titles[] = {
        "Лінії",
        "Шайба",
        "Перетворення",
        "М'ячик",
        "Епілепсія",
        "Летріс",
        "Клавіатура",
        "Тест SPI",
        "I2C-сканер",
        "<< Назад",
    };
    // vector of functions
    APP_CLASS_LIST classes = {
        APP_CLASS(DemoLines),
        APP_CLASS(DiskApp),
        APP_CLASS(TransformApp),
        APP_CLASS(BallApp),
        APP_CLASS(EpilepsyApp),
        APP_CLASS(LetrisApp),
        APP_CLASS(KeyboardApp),
        APP_CLASS(UserSPIApp),
        APP_CLASS(ScanI2CApp),
    };
    int count = sizeof(titles) / sizeof(titles[0]);
    lilka::Menu menu("Демо");
    for (int i = 0; i < count; i++) {
        menu.addItem(titles[i]);
    }
    while (1) {
        menu.update();
        menu.draw(canvas);
        queueDraw();
        int16_t index = menu.getSelectedIndex();
        if (index != -1) {
            if (index == count - 1) {
                break;
            }
            AppManager::getInstance()->runApp(classes[index]());
        }
        taskYIELD();
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

    lilka::Entry entries
        [32]; // TODO - allocate dynamically (increasing to 64 causes task stack overflow which is limited by ARDUINO_LOOP_STACK_SIZE)
    int numEntries = lilka::sdcard.listDir(path, entries);

    if (numEntries == -1) {
        // lilka::ui_alert(canvas, "Помилка", "Не вдалося прочитати директорію");
        alert("Помилка", "Не вдалося прочитати директорію");
        return;
    }

    String filenames[32];
    const menu_icon_t* icons[32];
    uint16_t colors[32];
    for (int i = 0; i < numEntries; i++) {
        filenames[i] = entries[i].name;
        icons[i] = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? &folder : get_file_icon(filenames[i]);
        colors[i] = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? lilka::display.color565(255, 255, 200)
                                                                       : get_file_color(filenames[i]);
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;
    colors[numEntries - 1] = 0;

    lilka::Menu menu("SD: " + path);
    for (int i = 0; i < numEntries; i++) {
        menu.addItem(filenames[i], icons[i], colors[i]);
    }

    while (1) {
        menu.update();
        menu.draw(canvas);
        queueDraw();
        int16_t index = menu.getSelectedIndex();
        if (index != -1) {
            if (index == numEntries - 1) {
                return;
            }
            if (entries[index].type == lilka::EntryType::ENT_DIRECTORY) {
                sdBrowserMenu(path + entries[index].name + "/");
            } else {
                selectFile(lilka::sdcard.abspath(path + entries[index].name));
            }
        }
        taskYIELD();
    }
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
        menu.update();
        menu.draw(canvas);
        queueDraw();
        int16_t index = menu.getSelectedIndex();
        if (index != -1) {
            if (index == numEntries - 1) {
                return;
            }
            selectFile(lilka::filesystem.abspath(filenames[index]));
        }
    }
}

void LauncherApp::selectFile(String path) {
    if (path.endsWith(".rom") || path.endsWith(".nes")) {
        AppManager::getInstance()->runApp(new NesApp(path));
    } else if (path.endsWith(".bin")) {
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
    String titles[] = {
        "Live Lua",
        "Lua REPL",
        "<< Назад",
    };
    int count = sizeof(titles) / sizeof(titles[0]);
    lilka::Menu menu("Розробка");
    for (int i = 0; i < count; i++) {
        menu.addItem(titles[i]);
    }
    while (1) {
        menu.update();
        menu.draw(canvas);
        queueDraw();
        int16_t index = menu.getSelectedIndex();
        if (index != -1) {
            if (index == count - 1) {
                return;
            }
            if (index == 0) {
                AppManager::getInstance()->runApp(new LuaLiveRunnerApp());
            } else if (index == 1) {
                AppManager::getInstance()->runApp(new LuaReplApp());
            }
        }
    }
}

void LauncherApp::settingsMenu() {
    String titles[] = {
        "WiFi",
        "Про систему",
        "Інфо про пристрій",
        "Таблиця розділів",
        "Перезавантаження",
        "<< Назад",
    };
    int count = sizeof(titles) / sizeof(titles[0]);
    lilka::Menu menu("Системні утиліти");
    for (int i = 0; i < count; i++) {
        menu.addItem(titles[i]);
    }
    while (1) {
        menu.update();
        menu.draw(canvas);
        queueDraw();
        int16_t index = menu.getSelectedIndex();
        if (index != -1) {
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
                    partitionMenu.update();
                    partitionMenu.draw(canvas);
                    queueDraw();
                    int16_t partitionIndex = partitionMenu.getSelectedIndex();
                    if (partitionIndex != -1) {
                        if (partitionIndex == labelCount - 1) {
                            break;
                        }
                        alert(
                            labels[partitionIndex],
                            String("Адреса: 0x") +
                                String(lilka::sys.get_partition_address(labels[partitionIndex].c_str()), HEX) + "\n" +
                                "Розмір: 0x" +
                                String(lilka::sys.get_partition_size(labels[partitionIndex].c_str()), HEX)
                        );
                    }
                }
            } else if (index == 4) {
                esp_restart();
            }
        }
    }
}

void LauncherApp::alert(String title, String message) {
    lilka::Alert alert(title, message);
    alert.draw(canvas);
    queueDraw();
    while (1) {
        alert.update();
        if (alert.isDone()) {
            break;
        }
        taskYIELD();
    }
}
