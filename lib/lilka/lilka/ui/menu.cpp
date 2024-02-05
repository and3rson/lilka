#include "menu.h"

#include "lilka/display.h"
#include "lilka/controller.h"

namespace lilka {

int ui_menu(String title, String menu[], int menu_size, int cursor) {
    // Arduino_TFT *gfx = lilka_display_get();
    display.fillScreen(display.color565(0, 0, 0));
    while (1) {
        display.setCursor(32, 48);
        display.setTextColor(display.color565(255, 255, 255));
        display.setFont(u8g2_font_6x13_t_cyrillic);
        display.setTextSize(2);
        display.println(title);
        display.println();
        display.setTextSize(1);
        display.setFont(u8g2_font_10x20_t_cyrillic);

        for (int i = 0; i < menu_size; i++) {
            display.fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? display.color565(255, 0, 0) : display.color565(0, 0, 0));
            display.setCursor(32, 96 + i * 24);
            display.setTextColor(display.color565(255, 255, 255));
            // gfx->print(i == cursor ? "> " : "  ");
            display.println(menu[i]);
        }

        State state = controller.state();
        while (state.up || state.down || state.start) {
            // Wait for buttons to be released
            state = controller.state();
            delay(10);
        }

        while (!state.up && !state.down && !state.start) {
            state = controller.state();
            delay(10);
        }

        if (state.up) {
            // Move cursor up
            cursor--;
            if (cursor < 0) {
                cursor = menu_size - 1;
            }
        } else if (state.down) {
            // Move cursor down
            cursor++;
            if (cursor >= menu_size) {
                cursor = 0;
            }
        } else if (state.start) {
            // Execute selected function
            return cursor;
        }
    }
}

} // namespace lilka
