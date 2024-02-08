#include "ui.h"

#include "lilka/display.h"
#include "lilka/controller.h"

namespace lilka {

int ui_menu(String title, String menu[], int menu_size, int cursor, const menu_icon_t *icons[]) {
    Canvas canvas;
    canvas.begin();
    controller.resetState();
    uint8_t cursorY = cursor * 24 + 96 - 20;
    while (1) {
        uint8_t desiredCursorY = cursor * 24 + 96 - 20;
        canvas.fillScreen(canvas.color565(0, 0, 0));
        canvas.fillTriangle(0, 0, 64, 0, 0, 32, canvas.color565(0, 0, 255));
        canvas.fillTriangle(LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT, LILKA_DISPLAY_WIDTH - 64, LILKA_DISPLAY_HEIGHT, LILKA_DISPLAY_WIDTH, LILKA_DISPLAY_HEIGHT - 32, canvas.color565(255, 255, 0));
        canvas.setCursor(32, 48);
        canvas.setTextColor(canvas.color565(255, 255, 255));
        canvas.setFont(u8g2_font_6x13_t_cyrillic);
        canvas.setTextSize(2);
        canvas.println(title);
        canvas.println();
        canvas.setTextSize(1);
        canvas.setFont(u8g2_font_10x20_t_cyrillic);

        canvas.fillRect(0, cursorY, LILKA_DISPLAY_WIDTH, 24, canvas.color565(255, 64, 0));
        if (cursorY < desiredCursorY) {
            cursorY += ceil((float)(desiredCursorY - cursorY) / 2);
            if (cursorY > desiredCursorY) {
                cursorY = desiredCursorY;
            }
        } else if (cursorY > desiredCursorY) {
            cursorY -= floor((float)(cursorY - desiredCursorY) / 2);
            if (cursorY < desiredCursorY) {
                cursorY = desiredCursorY;
            }
        }

        for (int i = 0; i < menu_size; i++) {
            // canvas.fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? canvas.color565(255, 64, 0) : canvas.color565(0, 0, 0));
            canvas.setTextBound(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24);
            if (icons != NULL && icons[i] != NULL) {
                canvas.draw16bitRGBBitmap(0, 96 + i * 24 - 20, const_cast<uint16_t *>(*icons[i]), 24, 24);
            }
            canvas.setCursor(32, 96 + i * 24);
            canvas.setTextColor(canvas.color565(255, 255, 255));
            // gfx->print(i == cursor ? "> " : "  ");
            canvas.println(menu[i]);
        }
        display.renderCanvas(canvas);

        State state = controller.getState();

        if (state.up.justPressed) {
            // Move cursor up
            cursor--;
            if (cursor < 0) {
                cursor = menu_size - 1;
            }
        } else if (state.down.justPressed) {
            // Move cursor down
            cursor++;
            if (cursor >= menu_size) {
                cursor = 0;
            }
        } else if (state.start.justPressed) {
            // Execute selected function
            return cursor;
        }
    }
}

void ui_alert(String title, String message) {
    int top = LILKA_DISPLAY_HEIGHT / 8;
    int mid = LILKA_DISPLAY_HEIGHT / 8 * 2;
    int bottom = LILKA_DISPLAY_HEIGHT / 8 * 7;

    int left = LILKA_DISPLAY_WIDTH / 8;
    int right = LILKA_DISPLAY_WIDTH / 8 * 7;
    int width = right - left;
    int xMargin = 4;

    controller.resetState();

    display.fillRect(left, top, width, mid - top, display.color565(32, 32, 128));
    display.setFont(u8g2_font_6x13_t_cyrillic);
    display.setTextSize(2);
    display.setTextBound(left + xMargin, top, width - xMargin * 2, mid - top);
    display.setCursor(left + xMargin, top + 13 * 2);
    display.println(title);

    display.fillRect(left, mid, width, bottom - mid, display.color565(32, 96, 96));
    display.setFont(u8g2_font_10x20_t_cyrillic);
    display.setTextSize(1);
    display.setTextBound(left + xMargin, top, width - xMargin * 2, bottom - mid);
    display.setCursor(left + xMargin, mid + 20);
    display.println(message);
    while (!controller.getState().start.justPressed) {
        delay(10);
    }
}

} // namespace lilka
