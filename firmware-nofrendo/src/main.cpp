/* Arduino Nofrendo
 * Please check hw_config.h and display.cpp for configuration details
 */
#include <esp_wifi.h>
#include <esp_task_wdt.h>
#include <FFat.h>
#include <SPIFFS.h>
#include <SD.h>
#include <SD_MMC.h>

#include <Arduino_GFX_Library.h>

#include "hw_config.h"

extern "C" {
#include <nofrendo.h>
}

#include <lilka.h>

void setup() {
    TaskHandle_t idle_0 = xTaskGetIdleTaskHandleForCPU(0);
    esp_task_wdt_delete(idle_0);

    lilka_begin();
}

void loop() {
    String filenames[32];
    int numFiles = 0;

    numFiles = lilka_filesystem_readdir(filenames);

    // Draw menu

    int currentFile = 0;
    char *argv[1];

    Arduino_TFT *gfx = lilka_display_get();
    gfx->fillScreen(gfx->color565(0, 0, 0));
    while (1) {
        gfx->setCursor(32, 32);
        gfx->setTextColor(gfx->color565(255, 255, 255));
        gfx->setTextSize(2);
        gfx->println("Select ROM:");
        gfx->println();

        for (int i = 0; i < numFiles; i++) {
            if (i == currentFile) {
                gfx->setTextColor(gfx->color565(255, 0, 0));
            } else {
                gfx->setTextColor(gfx->color565(255, 255, 255));
            }
            gfx->println(filenames[i]);
        }
        gfx->flush();

        lilka_input_t input = lilka_input_read();
        while (input.up || input.down) {
            // Wait for cursor buttons to be released
            input = lilka_input_read();
            delay(10);
        }

        while (!input.up && !input.down && !input.start) {
            input = lilka_input_read();
            delay(10);
        }

        if (input.start) {
            char fullFilename[256];
            sprintf(fullFilename, "%s/%s", FSROOT, filenames[currentFile].c_str());
            argv[0] = fullFilename;
            break;
        } else {
            if (input.up) {
                currentFile--;
            } else if (input.down) {
                currentFile++;
            }
            if (currentFile < 0) {
                currentFile = numFiles - 1;
            } else if (currentFile >= numFiles) {
                currentFile = 0;
            }

            delay(10);
        }
    }

    Serial.print("NoFrendo start! Filename: ");
    Serial.println(argv[0]);
    // Serial.println("CPU freq: " + String(ESP.getCpuFreqMHz()) + " MHz");
    nofrendo_main(1, argv);
    Serial.println("NoFrendo end!\n");
    // }
}
