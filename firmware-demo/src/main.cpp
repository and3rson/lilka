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
#include <lilka/ui/menu.h>

void setup() {
    lilka::begin();
}

void start_nes_emulator(const char *filename) {}

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
    while (lilka::controller.state().start) {
    };
    float x = random(16, lilka::display.width() - 16);
    float y = random(16, lilka::display.height() - 16);
    float xDir = 1;
    float yDir = 1;
    while (1) {
        x += xDir * 0.25;
        y += yDir * 0.25;
        bool hit = false;
        if (x < 0 || x > lilka::display.width()) {
            xDir *= -1;
        }
        if (y < 0 || y > lilka::display.height()) {
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
        lilka::display.drawCircle(x, y, 16, random(0, 0xFFFF));
        if (lilka::controller.state().start) {
            return;
        }
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
        cursor = lilka_ui_menu("Оберіть демо:", demos, 4, cursor);
        if (cursor == 3) {
            return;
        }
        demo_funcs[cursor]();
    }
}

void roms_menu() {
    String filenames[32];
    int numFiles = 0;
    numFiles = lilka::filesystem.readdir(filenames, ".nes");
    filenames[numFiles++] = "<< Назад";
    while (1) {
        int file = lilka_ui_menu("Оберіть ROM:", filenames, numFiles, 0);
        if (file == numFiles - 1) {
            return;
        }
        char *argv[1];
        char fullFilename[256];
        strcpy(fullFilename, lilka::filesystem.abspath(filenames[file]).c_str());
        argv[0] = fullFilename;

        TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
        esp_task_wdt_delete(idle_0);

        Serial.print("NoFrendo start! Filename: ");
        Serial.println(argv[0]);
        // Serial.println("CPU freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
        nofrendo_main(1, argv);
        Serial.println("NoFrendo end!\n");
    }
}

void loop() {
    String menu[] = {
        "Демо",
        "Емулятор NES",
    };
    int cursor;
    while (1) {
        cursor = lilka_ui_menu("Головне меню", menu, 2, cursor);
        if (cursor == 0) {
            demos_menu();
        } else if (cursor == 1) {
            roms_menu();
        }
    }
}
