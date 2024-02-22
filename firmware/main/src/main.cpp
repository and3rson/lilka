#include <esp_wifi.h>
#include <esp_task_wdt.h>

#include <Arduino_GFX_Library.h>

#include "nes/hw_config.h"

extern "C" {
#include <nofrendo.h>
}

#include <lilka.h>
#include <lilka/icons/file.h>
#include <lilka/icons/folder.h>
#include <lilka/icons/nes.h>

#include "demos/demos.h"

void setup() {
    lilka::begin();
}

void demos_menu() {
    void (*demo_funcs[])() = {
        demo_lines, demo_disc, demo_ball, demo_epilepsy, demo_letris, demo_user_spi, demo_scan_i2c, 0,
    };

    String demos[] = {
        "Лінії", "Шайба", "М'ячик", "Епілепсія", "Летріс", "Тест SPI", "I2C-сканер", "<< Назад",
    };
    int count = sizeof(demos) / sizeof(demos[0]);
    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu("Оберіть демо:", demos, count, cursor);
        if (!demo_funcs[cursor]) {
            return;
        }
        demo_funcs[cursor]();
    }
}

void select_file(String path) {
    if (path.endsWith(".rom") || path.endsWith(".nes")) {
        char *argv[1];
        char fullFilename[256];
        strcpy(fullFilename, path.c_str());
        argv[0] = fullFilename;

        TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
        esp_task_wdt_delete(idle_0);

        Serial.print("NoFrendo start! Filename: ");
        Serial.println(argv[0]);
        nofrendo_main(1, argv);
        Serial.println("NoFrendo end!\n");
    } else if (path.endsWith(".bin")) {
        int error;
        lilka::LoaderHandle *handle = lilka::loader.createHandle(path);
        error = handle->start();
        if (error) {
            delete handle;
            lilka::ui_alert("Помилка", String("Етап: 1\nКод: ") + error);
            return;
        }
        lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
        lilka::display.setTextColor(lilka::display.color565(255, 255, 255), lilka::display.color565(0, 0, 0));
        lilka::display.setFont(u8g2_font_10x20_t_cyrillic);
        lilka::display.setTextBound(16, 0, LILKA_DISPLAY_WIDTH - 16, LILKA_DISPLAY_HEIGHT);
        while ((error = handle->process()) != 0) {
            float progress = (float)handle->getBytesWritten() / handle->getBytesTotal();
            lilka::display.setCursor(16, LILKA_DISPLAY_HEIGHT / 2 - 10);
            lilka::display.printf("Завантаження (%d%%)\n", (int)(progress * 100));
            lilka::display.println(path);
            String buf = String(handle->getBytesWritten()) + " / " + handle->getBytesTotal();
            int16_t x, y;
            uint16_t w, h;
            lilka::display.getTextBounds(buf, lilka::display.getCursorX(), lilka::display.getCursorY(), &x, &y, &w, &h);
            lilka::display.fillRect(x, y, w, h, lilka::display.color565(0, 0, 0));
            lilka::display.println(buf);
            lilka::display.fillRect(16, LILKA_DISPLAY_HEIGHT / 2 + 40, LILKA_DISPLAY_WIDTH - 32, 5, lilka::display.color565(64, 64, 64));
            lilka::display.fillRect(16, LILKA_DISPLAY_HEIGHT / 2 + 40, (LILKA_DISPLAY_WIDTH - 32) * progress, 5, lilka::display.color565(255, 128, 0));
        }
        if (error) {
            delete handle;
            lilka::ui_alert("Помилка", String("Етап: 2\nКод: ") + error);
            return;
        }
        error = handle->finishAndReboot();
        if (error) {
            delete handle;
            lilka::ui_alert("Помилка", String("Етап: 3\nКод: ") + error);
            return;
        }
    } else if (path.endsWith(".lua")) {
        int retCode = lilka::lua_run(path);
        if (retCode) {
            lilka::ui_alert("Lua", String("Увага!\nКод завершення: ") + retCode);
        }
    } else {
        // Get file size
        FILE *file = fopen(path.c_str(), "r");
        if (!file) {
            lilka::ui_alert("Помилка", "Не вдалося відкрити файл");
            return;
        }
        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fclose(file);
        lilka::ui_alert(path, String("Розмір:\n") + size + " байт");
    }
}

void sd_browser_menu(String path) {
    if (!lilka::sdcard.available()) {
        lilka::ui_alert("Помилка", "SD-карта не знайдена");
        return;
    }

    lilka::Entry entries[32];
    int numEntries = lilka::sdcard.listDir(path, entries);

    if (numEntries == -1) {
        lilka::ui_alert("Помилка", "Не вдалося прочитати директорію");
        return;
    }

    String filenames[32];
    const menu_icon_t *icons[32];
    for (int i = 0; i < numEntries; i++) {
        filenames[i] = entries[i].name;
        icons[i] = entries[i].type == lilka::EntryType::ENT_DIRECTORY ? &folder : (entries[i].name.endsWith(".rom") || entries[i].name.endsWith(".nes")) ? &nes : &file;
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;

    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu(String("SD: ") + path, filenames, numEntries, cursor, icons);
        if (cursor == numEntries - 1) {
            return;
        }
        if (entries[cursor].type == lilka::EntryType::ENT_DIRECTORY) {
            sd_browser_menu(path + entries[cursor].name + "/");
        } else {
            select_file(lilka::sdcard.abspath(path + entries[cursor].name));
        }
    }
}

void spiffs_browser_menu() {
    if (!lilka::filesystem.available()) {
        lilka::ui_alert("Помилка", "SPIFFS не підтримується");
        return;
    }

    String filenames[32];
    int numEntries = lilka::filesystem.readdir(filenames);

    if (numEntries == -1) {
        lilka::ui_alert("Помилка", "Не вдалося прочитати корінь SPIFFS");
        return;
    }

    const menu_icon_t *icons[32];
    for (int i = 0; i < numEntries; i++) {
        icons[i] = (filenames[i].endsWith(".rom") || filenames[i].endsWith(".nes")) ? &nes : &file;
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;

    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu(String("SPIFFS"), filenames, numEntries, cursor, icons);
        if (cursor == numEntries - 1) {
            return;
        }
        select_file(lilka::filesystem.abspath(filenames[cursor]));
    }
}

void system_utils_menu() {
    String menu[] = {
        "Перезавантаження", "Deep Sleep", "Версія SDK", "Версія ESP-IDF", "Інфо про пристрій", "Таблиця розділів", "<< Назад",
    };
    int cursor = 0;
    int count = sizeof(menu) / sizeof(menu[0]);
    while (1) {
        cursor = lilka::ui_menu("Системні утиліти", menu, count, cursor);
        if (cursor == 0) {
            esp_restart();
        } else if (cursor == 1) {
            esp_deep_sleep_start();
        } else if (cursor == 2) {
            lilka::ui_alert("Версія SDK", "Версія: " + String(ESP.getSdkVersion()));
        } else if (cursor == 3) {
            lilka::ui_alert("Версія ESP-IDF", "Версія: " + String(esp_get_idf_version()));
        } else if (cursor == 4) {
            char buf[256];
            sprintf(buf,
                    "Модель: %s\n"
                    "Ревізія: %d\n"
                    "Версія SDK: %s\n"
                    "Версія ESP-IDF: %s\n"
                    "Частота: %d МГц\n"
                    "Кількість ядер: %d\n",
                    ESP.getChipModel(), ESP.getChipRevision(), ESP.getSdkVersion(), esp_get_idf_version(), ESP.getCpuFreqMHz(), ESP.getChipCores());
            lilka::ui_alert("Інфо про пристрій", buf);
        } else if (cursor == 5) {
            String labels[16];
            int labelCount = lilka::sys.get_partition_labels(labels);
            lilka::ui_menu("Таблиця розділів", labels, labelCount);
        } else if (cursor == count - 1) {
            return;
        }
    }
}

void loop() {
    String menu[] = {
        "Демо", "Браузер SD-карти", "Браузер SPIFFS", "Системні утиліти", "Про систему",
    };
    int cursor = 0;
    int count = sizeof(menu) / sizeof(menu[0]);
    while (1) {
        cursor = lilka::ui_menu("Головне меню", menu, count, cursor);
        if (cursor == 0) {
            demos_menu();
        } else if (cursor == 1) {
            sd_browser_menu("/");
        } else if (cursor == 2) {
            spiffs_browser_menu();
        } else if (cursor == 3) {
            system_utils_menu();
        } else if (cursor == 4) {
            lilka::ui_alert("Лілка Demo OS", "by Андерсон\n& friends");
        }
    }
}
