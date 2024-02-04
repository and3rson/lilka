#include "menu.h"

#include "lilka/display.h"
#include "lilka/input.h"

int lilka_ui_menu(String title, String menu[], int menu_size) {
    Arduino_TFT *gfx = lilka_display_get();
    gfx->fillScreen(gfx->color565(0, 0, 0));
    int cursor = 0;
    while (1) {
        gfx->setCursor(32, 48);
        gfx->setTextColor(gfx->color565(255, 255, 255));
        gfx->setFont(u8g2_font_6x13_t_cyrillic);
        gfx->setTextSize(2);
        gfx->println(title);
        gfx->println();
        gfx->setTextSize(1);
        gfx->setFont(u8g2_font_10x20_t_cyrillic);

        for (int i = 0; i < menu_size; i++) {
            gfx->fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? gfx->color565(255, 0, 0) : gfx->color565(0, 0, 0));
            gfx->setCursor(32, 96 + i * 24);
            gfx->setTextColor(gfx->color565(255, 255, 255));
            // gfx->print(i == cursor ? "> " : "  ");
            gfx->println(menu[i]);
        }

        lilka_input_t input = lilka_input_read();
        while (input.up || input.down || input.start) {
            // Wait for buttons to be released
            input = lilka_input_read();
            delay(10);
        }

        while (!input.up && !input.down && !input.start) {
            input = lilka_input_read();
            delay(10);
        }

        if (input.up) {
            // Move cursor up
            cursor--;
            if (cursor < 0) {
                cursor = menu_size - 1;
            }
        } else if (input.down) {
            // Move cursor down
            cursor++;
            if (cursor >= menu_size) {
                cursor = 0;
            }
        } else if (input.start) {
            // Execute selected function
            return cursor;
        }
    }
}
