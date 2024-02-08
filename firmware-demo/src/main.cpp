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

void setup() {
    lilka::begin();
}

void demo1() {
    while (lilka::controller.state().start) {
    };
    while (1) {
        int x1 = random(0, lilka::display.width());
        int y1 = random(0, lilka::display.height());
        int x2 = random(0, lilka::display.width());
        int y2 = random(0, lilka::display.height());
        uint16_t color = random(0, 0xFFFF);
        lilka::display.drawLine(x1, y1, x2, y2, color);
        if (lilka::controller.state().start) {
            return;
        }
    }
}

void demo2() {
    lilka::Canvas canvas;
    canvas.begin();
    while (lilka::controller.state().start) {
    };
    float x = random(16, canvas.width() - 16);
    float y = random(16, canvas.height() - 16);
    float xDir = 1;
    float yDir = 1;
    while (1) {
        canvas.fillScreen(canvas.color565(0, 0, 0));
        x += xDir * 0.25;
        y += yDir * 0.25;
        bool hit = false;
        if (x < 0 || x > canvas.width()) {
            xDir *= -1;
        }
        if (y < 0 || y > canvas.height()) {
            yDir *= -1;
        }
        if (hit) {
            // Rotate vector a little bit randomly
            float angle = ((float)random(-15, 15)) / 180 * PI;
            float xDirNew = xDir * cos(angle) - yDir * sin(angle);
            float yDirNew = xDir * sin(angle) + yDir * cos(angle);
            xDir = xDirNew;
            yDir = yDirNew;
        }
        canvas.drawCircle(x, y, 16, random(0, 0xFFFF));
        if (lilka::controller.state().start) {
            return;
        }
        lilka::display.renderCanvas(canvas);
    }
}

void demo3() {
    while (lilka::controller.state().start) {
    };
    while (1) {
        lilka::display.fillScreen(random(0, 0xFFFF));
        if (lilka::controller.state().start) {
            return;
        }
    }
}

void demos_menu() {
    void (*demo_funcs[])() = {
        demo1,
        demo2,
        demo3,
    };

    String demos[] = {
        "Хаос",
        "М'ячик",
        "Епілепсія",
        "<< Назад",
    };
    int cursor;
    while (1) {
        cursor = lilka::ui_menu("Оберіть демо:", demos, 4, cursor);
        if (cursor == 3) {
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
    menu_icon_t *icons[32];
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

void loop() {
    String menu[] = {
        "Демо",
        // "Емулятор NES",
        "Браузер SD-карти",
        "Про систему",
    };
    int cursor;
    while (1) {
        cursor = lilka::ui_menu("Головне меню", menu, 4, cursor);
        if (cursor == 0) {
            demos_menu();
        } else if (cursor == 1) {
            sd_browser_menu("/");
        } else if (cursor == 2) {
            lilka::ui_alert("Лілка", "by Андерсон\n& friends");
        }
    }
}
