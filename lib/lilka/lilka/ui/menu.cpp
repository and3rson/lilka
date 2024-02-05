#include "menu.h"

#include "lilka/display.h"
#include "lilka/controller.h"

int lilka_ui_menu(String title, String menu[], int menu_size, int cursor) {
    // Arduino_TFT *gfx = lilka_display_get();
    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
    while (1) {
        lilka::display.setCursor(32, 48);
        lilka::display.setTextColor(lilka::display.color565(255, 255, 255));
        lilka::display.setFont(u8g2_font_6x13_t_cyrillic);
        lilka::display.setTextSize(2);
        lilka::display.println(title);
        lilka::display.println();
        lilka::display.setTextSize(1);
        lilka::display.setFont(u8g2_font_10x20_t_cyrillic);

        for (int i = 0; i < menu_size; i++) {
            lilka::display.fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? lilka::display.color565(255, 0, 0) : lilka::display.color565(0, 0, 0));
            lilka::display.setCursor(32, 96 + i * 24);
            lilka::display.setTextColor(lilka::display.color565(255, 255, 255));
            // gfx->print(i == cursor ? "> " : "  ");
            lilka::display.println(menu[i]);
        }

        lilka::input_t input = lilka::controller.read();
        while (input.up || input.down || input.start) {
            // Wait for buttons to be released
            input = lilka::controller.read();
            delay(10);
        }

        while (!input.up && !input.down && !input.start) {
            input = lilka::controller.read();
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
