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
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    esp_task_wdt_delete(idle_0);

    lilka_begin();
}

void start_nes_emulator(const char *filename) {}

void demo1() {
    Arduino_TFT *gfx = lilka_display_get();
    while (lilka_input_read().start) {
    };
    while (1) {
        int x1 = random(0, gfx->width());
        int y1 = random(0, gfx->height());
        int x2 = random(0, gfx->width());
        int y2 = random(0, gfx->height());
        uint16_t color = random(0, 0xFFFF);
        gfx->drawLine(x1, y1, x2, y2, color);
        if (lilka_input_read().start) {
            return;
        }
    }
}

void demo2() {
    Arduino_TFT *gfx = lilka_display_get();
    while (lilka_input_read().start) {
    };
    float x = random(16, gfx->width() - 16);
    float y = random(16, gfx->height() - 16);
    float xDir = 1;
    float yDir = 1;
    while (1) {
        x += xDir * 0.25;
        y += yDir * 0.25;
        if (x < 0 || x > gfx->width()) {
            xDir *= -1;
            // Rotate vector a little bit randomly
            float angle = ((float)random(-30, 30)) / 180 * PI;
            float xDirNew = xDir * cos(angle) - yDir * sin(angle);
            float yDirNew = xDir * sin(angle) + yDir * cos(angle);
            xDir = xDirNew;
            yDir = yDirNew;
        }
        if (y < 0 || y > gfx->height()) {
            yDir *= -1;
        }
        gfx->drawCircle(x, y, 16, random(0, 0xFFFF));
        if (lilka_input_read().start) {
            return;
        }
    }
}

void demo3() {}

void list_demos() {
    void (*demo_funcs[])() = {
        demo1,
        demo2,
        demo3,
    };

    String demos[] = {
        "Хаос",
        "М'ячик",
        "Демо 3",
        "<< Назад",
    };
    while (1) {
        int selection = lilka_ui_menu("Оберіть демо:", demos, 4);
        if (selection == 3) {
            return;
        }
        demo_funcs[selection]();
    }
}

void list_roms() {
    String filenames[32];
    int numFiles = 0;
    numFiles = lilka_filesystem_readdir(filenames, ".nes");
    filenames[numFiles++] = "<< Назад";
    while (1) {
        int file = lilka_ui_menu("Оберіть ROM:", filenames, numFiles);
        if (file == numFiles - 1) {
            return;
        }
        char *argv[1];
        char fullFilename[256];
        strcpy(fullFilename, lilka_filesystem_abspath(filenames[file]).c_str());
        argv[0] = fullFilename;

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
    while (1) {
        int selection = lilka_ui_menu("Головне меню", menu, 2);
        if (selection == 0) {
            list_demos();
        } else if (selection == 1) {
            list_roms();
        }
    }
}
