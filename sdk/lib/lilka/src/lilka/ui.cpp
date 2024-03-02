#include "ui.h"

#include "lilka/display.h"
#include "lilka/controller.h"

#include "serial.h"

namespace lilka {

#define MENU_HEIGHT 7

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int ui_menu(
    String title, String menu[], int menu_size, int cursor, const menu_icon_t *icons[], const uint16_t colors[]
) {
    Canvas canvas;
    canvas.begin();
    controller.resetState();
    // int16_t cursorY = cursor * 24 + 96 - 20;
    int16_t scroll = 0;
    while (1) {
        if (cursor < scroll) {
            scroll = cursor;
            // cursorY = cursor * 24 + 96 - 20;
        } else if (cursor > scroll + MENU_HEIGHT - 1) {
            scroll = cursor - MENU_HEIGHT + 1;
            // cursorY = cursor * 24 + 96 - 20;
        }

        // uint8_t desiredCursorY = cursor * 24 + 96 - 20;
        canvas.fillScreen(canvas.color565(0, 0, 0));
        int8_t angleShift = sin(millis() / 1000.0) * 16;
        // Draw triangle in top-left
        canvas.fillTriangle(0, 0, 48 - angleShift, 0, 0, 48 + angleShift, canvas.color565(0, 0, 255));
        // Draw triangle in top-right
        canvas.fillTriangle(
            LILKA_DISPLAY_WIDTH, 0, LILKA_DISPLAY_WIDTH - 48 - angleShift, 0, LILKA_DISPLAY_WIDTH, 48 - angleShift,
            canvas.color565(255, 255, 0)
        );
        canvas.setCursor(32, 48);
        canvas.setTextColor(canvas.color565(255, 255, 255));
        canvas.setFont(FONT_6x13);
        canvas.setTextSize(2);
        canvas.println(title);
        canvas.println();
        canvas.setTextSize(1);
        canvas.setFont(FONT_10x20);

        canvas.fillRect(0, (cursor * 24 + 96 - 20) - scroll * 24, LILKA_DISPLAY_WIDTH, 24, canvas.color565(255, 64, 0));
        // if (cursorY < desiredCursorY) {
        //     cursorY += ceil((float)(desiredCursorY - cursorY) / 2);
        //     if (cursorY > desiredCursorY) {
        //         cursorY = desiredCursorY;
        //     }
        // } else if (cursorY > desiredCursorY) {
        //     cursorY -= floor((float)(cursorY - desiredCursorY) / 2);
        //     if (cursorY < desiredCursorY) {
        //         cursorY = desiredCursorY;
        //     }
        // }

        for (int i = scroll; i < MIN(scroll + MENU_HEIGHT, menu_size); i++) {
            // canvas.fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? canvas.color565(255, 64, 0) :
            // canvas.color565(0, 0, 0));
            int16_t screenI = i - scroll;
            canvas.setTextBound(0, 96 + screenI * 24 - 20, LILKA_DISPLAY_WIDTH, 24);
            if (icons != NULL && icons[i] != NULL) {
                const uint16_t *icon = *icons[i];
                canvas.draw16bitRGBBitmapWithTranColor(
                    0, 96 + screenI * 24 - 20, icon, canvas.color565(0, 0, 0), 24, 24
                );
            }
            canvas.setCursor(32, 96 + screenI * 24);
            if (colors != NULL && colors[i] != 0 && cursor != i) {
                canvas.setTextColor(colors[i]);
            } else {
                canvas.setTextColor(canvas.color565(255, 255, 255));
            }
            // gfx->print(i == cursor ? "> " : "  ");
            canvas.println(menu[i]);
        }

        // Draw scrollbar
        if (menu_size > MENU_HEIGHT) {
            int top = 96 - 20;
            int height = MENU_HEIGHT * 24;
            canvas.fillRect(LILKA_DISPLAY_WIDTH - 8, top, 8, height, canvas.color565(96, 96, 96));
            int barHeight = height * MENU_HEIGHT / menu_size;
            int barTop = top + scroll * height / menu_size;
            canvas.fillRect(LILKA_DISPLAY_WIDTH - 8, barTop, 8, barHeight, canvas.color565(255, 255, 255));
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
        } else if (state.a.justPressed) {
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
    display.setFont(FONT_6x13);
    display.setTextSize(2);
    display.setTextBound(left + xMargin, top, width - xMargin * 2, mid - top);
    display.setCursor(left + xMargin, top + 13 * 2);
    display.println(title);

    display.fillRect(left, mid, width, bottom - mid, display.color565(32, 96, 96));
    display.setFont(FONT_9x15);
    display.setTextSize(1);
    display.setTextBound(left + xMargin, top, width - xMargin * 2, bottom - mid);
    display.setCursor(left + xMargin, mid + 20);
    display.println(message);
    while (!controller.getState().a.justPressed) {
        delay(10);
    }
}

} // namespace lilka
