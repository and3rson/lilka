#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <FFat.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>

#include "nes/hw_config.h"

extern "C" {
#include <nofrendo.h>
}

#include <lilka.h>
#include <lilka/icons/file.h>
#include <lilka/icons/folder.h>
#include <lilka/icons/nes.h>

extern void demo_lines();
extern void demo_disc();
extern void demo_epilepsy();
extern void demo_ball();
extern void demo_letris();
extern void demo_user_spi();
extern void demo_scan_i2c();

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
        icons[i] = entries[i].type == lilka::EntryType::DIRECTORY ? &folder : (entries[i].name.endsWith(".rom") || entries[i].name.endsWith(".nes")) ? &nes : &file;
    }
    filenames[numEntries++] = "<< Назад";
    icons[numEntries - 1] = 0;

    int cursor = 0;
    while (1) {
        cursor = lilka::ui_menu(String("SD: ") + path, filenames, numEntries, cursor, icons);
        if (cursor == numEntries - 1) {
            return;
        }
        if (entries[cursor].type == lilka::EntryType::DIRECTORY) {
            sd_browser_menu(path + entries[cursor].name + "/");
        } else if (entries[cursor].name.endsWith(".rom") || entries[cursor].name.endsWith(".nes")) {
            char *argv[1];
            char fullFilename[256];
            strcpy(fullFilename, lilka::sdcard.abspath(entries[cursor].name).c_str());
            argv[0] = fullFilename;

            TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
            esp_task_wdt_delete(idle_0);

            Serial.print("NoFrendo start! Filename: ");
            Serial.println(argv[0]);
            nofrendo_main(1, argv);
            Serial.println("NoFrendo end!\n");
        } else {
            lilka::ui_alert(entries[cursor].name, "Розмір:\n" + String(entries[cursor].size) + " байт");
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
        if (filenames[cursor].endsWith(".rom") || filenames[cursor].endsWith(".nes")) {
            char *argv[1];
            char fullFilename[256];
            strcpy(fullFilename, lilka::filesystem.abspath(filenames[cursor]).c_str());
            argv[0] = fullFilename;

            TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
            esp_task_wdt_delete(idle_0);

            Serial.print("NoFrendo start! Filename: ");
            Serial.println(argv[0]);
            nofrendo_main(1, argv);
            Serial.println("NoFrendo end!\n");
        }
    }
}

void loop() {
    String menu[] = {
        "Демо",
        "Браузер SD-карти",
        "Браузер SPIFFS",
        "Про систему",
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
            lilka::ui_alert("Лілка Demo OS", "by Андерсон\n& friends");
        }
    }
}
