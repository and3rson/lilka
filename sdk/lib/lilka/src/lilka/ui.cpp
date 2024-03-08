#include "ui.h"

#include "lilka/display.h"
#include "lilka/controller.h"

#include "serial.h"

namespace lilka {

#define MENU_HEIGHT 7

#define MIN(a, b) ((a) < (b) ? (a) : (b))

// int ui_menu(
//     Canvas *canvas, String title, String menu[], int menu_size, int cursor, const menu_icon_t *icons[],
//     const uint16_t colors[]
// ) {
//     controller.resetState();
//     // int16_t cursorY = cursor * 24 + 96 - 20;
//     int16_t scroll = 0;
//     while (1) {
//         if (cursor < scroll) {
//             scroll = cursor;
//             // cursorY = cursor * 24 + 96 - 20;
//         } else if (cursor > scroll + MENU_HEIGHT - 1) {
//             scroll = cursor - MENU_HEIGHT + 1;
//             // cursorY = cursor * 24 + 96 - 20;
//         }
//
//         canvas->acquireMutex();
//         // uint8_t desiredCursorY = cursor * 24 + 96 - 20;
//         canvas->fillScreen(canvas->color565(0, 0, 0));
//         int8_t angleShift = sin(millis() / 1000.0) * 16;
//         // Draw triangle in top-left
//         canvas->fillTriangle(0, 0, 48 - angleShift, 0, 0, 48 + angleShift, canvas->color565(0, 0, 255));
//         // Draw triangle in top-right
//         canvas->fillTriangle(
//             canvas->width(), 0, canvas->width() - 48 - angleShift, 0, canvas->width(), 48 - angleShift,
//             canvas->color565(255, 255, 0)
//         );
//         canvas->setCursor(32, 48);
//         canvas->setTextColor(canvas->color565(255, 255, 255));
//         canvas->setFont(FONT_6x13);
//         canvas->setTextSize(2);
//         canvas->println(title);
//         canvas->println();
//         canvas->setTextSize(1);
//         canvas->setFont(FONT_10x20);
//
//         canvas->fillRect(0, (cursor * 24 + 96 - 20) - scroll * 24, canvas->width(), 24, canvas->color565(255, 64, 0));
//         // if (cursorY < desiredCursorY) {
//         //     cursorY += ceil((float)(desiredCursorY - cursorY) / 2);
//         //     if (cursorY > desiredCursorY) {
//         //         cursorY = desiredCursorY;
//         //     }
//         // } else if (cursorY > desiredCursorY) {
//         //     cursorY -= floor((float)(cursorY - desiredCursorY) / 2);
//         //     if (cursorY < desiredCursorY) {
//         //         cursorY = desiredCursorY;
//         //     }
//         // }
//
//         for (int i = scroll; i < MIN(scroll + MENU_HEIGHT, menu_size); i++) {
//             // canvas->fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? canvas->color565(255, 64, 0) :
//             // canvas->color565(0, 0, 0));
//             int16_t screenI = i - scroll;
//             canvas->setTextBound(0, 96 + screenI * 24 - 20, canvas->width(), 24);
//             if (icons != NULL && icons[i] != NULL) {
//                 const uint16_t *icon = *icons[i];
//                 // Cast icon to non-const uint16_t * to avoid warning
//                 // TODO: Had to do this because I switched to canvas (FreeRTOS experiment)
//                 uint16_t *icon2 = (uint16_t *)icon;
//                 canvas->draw16bitRGBBitmapWithTranColor(
//                     0, 96 + screenI * 24 - 20, icon2, canvas->color565(0, 0, 0), 24, 24
//                 );
//             }
//             canvas->setCursor(32, 96 + screenI * 24);
//             if (colors != NULL && colors[i] != 0 && cursor != i) {
//                 canvas->setTextColor(colors[i]);
//             } else {
//                 canvas->setTextColor(canvas->color565(255, 255, 255));
//             }
//             // gfx->print(i == cursor ? "> " : "  ");
//             canvas->println(menu[i]);
//         }
//
//         // Draw scrollbar
//         if (menu_size > MENU_HEIGHT) {
//             int top = 96 - 20;
//             int height = MENU_HEIGHT * 24;
//             canvas->fillRect(canvas->width() - 8, top, 8, height, canvas->color565(96, 96, 96));
//             int barHeight = height * MENU_HEIGHT / menu_size;
//             int barTop = top + scroll * height / menu_size;
//             canvas->fillRect(canvas->width() - 8, barTop, 8, barHeight, canvas->color565(255, 255, 255));
//         }
//         canvas->releaseMutex();
//
//         // display.renderCanvas(canvas);
//
//         State state = controller.getState();
//
//         if (state.up.justPressed) {
//             // Move cursor up
//             cursor--;
//             if (cursor < 0) {
//                 cursor = menu_size - 1;
//             }
//         } else if (state.down.justPressed) {
//             // Move cursor down
//             cursor++;
//             if (cursor >= menu_size) {
//                 cursor = 0;
//             }
//         } else if (state.a.justPressed) {
//             // Execute selected function
//             return cursor;
//         }
//
//         taskYIELD();
//     }
// }
//
// void ui_alert(Canvas *canvas, String title, String message) {
//     int top = canvas->height() / 8;
//     int mid = canvas->height() / 8 * 2;
//     int bottom = canvas->height() / 8 * 7;
//
//     int left = canvas->width() / 8;
//     int right = canvas->width() / 8 * 7;
//     int width = right - left;
//     int xMargin = 4;
//
//     controller.resetState();
//
//     canvas->acquireMutex();
//     canvas->fillRect(left, top, width, mid - top, canvas->color565(32, 32, 128));
//     canvas->setFont(FONT_6x13);
//     canvas->setTextSize(2);
//     canvas->setTextBound(left + xMargin, top, width - xMargin * 2, mid - top);
//     canvas->setCursor(left + xMargin, top + 13 * 2);
//     canvas->println(title);
//
//     canvas->fillRect(left, mid, width, bottom - mid, canvas->color565(32, 96, 96));
//     canvas->setFont(FONT_9x15);
//     canvas->setTextSize(1);
//     canvas->setTextBound(left + xMargin, top, width - xMargin * 2, bottom - mid);
//     canvas->setCursor(left + xMargin, mid + 20);
//     canvas->println(message);
//     canvas->releaseMutex();
//     while (!controller.getState().a.justPressed) {
//         taskYIELD();
//     }
// }

Menu::Menu(String title) {
    this->title = title;
    this->scroll = 0;
    this->selectedIndex = -1;
    this->setCursor(0);
}

void Menu::addItem(String title, const menu_icon_t *icon, uint16_t color) {
    items.push_back({
        .title = title,
        .icon = icon,
        .color = color,
    });
}

void Menu::setCursor(int16_t cursor) {
    if (cursor < 0) {
        cursor = items.size() - 1;
    } else if (cursor >= items.size()) {
        cursor = 0;
    }
    this->cursor = cursor;
}

void Menu::update() {
    if (cursor < scroll) {
        scroll = cursor;
        // cursorY = cursor * 24 + 96 - 20;
    } else if (cursor > scroll + MENU_HEIGHT - 1) {
        scroll = cursor - MENU_HEIGHT + 1;
        // cursorY = cursor * 24 + 96 - 20;
    }

    State state = controller.getState();

    if (state.up.justPressed) {
        // Move cursor up
        if (cursor == 0) {
            cursor = items.size() - 1;
        } else {
            cursor--;
        }
    } else if (state.down.justPressed) {
        // Move cursor down
        cursor++;
        if (cursor >= items.size()) {
            cursor = 0;
        }
    } else if (state.a.justPressed) {
        // Execute selected function
        this->selectedIndex = cursor;
    }
}

void Menu::draw(Arduino_GFX *canvas) {
    // uint8_t desiredCursorY = cursor * 24 + 96 - 20;
    canvas->fillScreen(canvas->color565(0, 0, 0));
    int8_t angleShift = sin(millis() / 1000.0) * 16;
    // Draw triangle in top-left
    canvas->fillTriangle(0, 0, 48 - angleShift, 0, 0, 48 + angleShift, canvas->color565(0, 0, 255));
    // Draw triangle in top-right
    canvas->fillTriangle(
        canvas->width(), 0, canvas->width() - 48 - angleShift, 0, canvas->width(), 48 - angleShift,
        canvas->color565(255, 255, 0)
    );
    canvas->setCursor(32, 48);
    canvas->setTextBound(0, 0, canvas->width(), canvas->height());
    canvas->setTextColor(canvas->color565(255, 255, 255));
    canvas->setFont(FONT_6x13);
    canvas->setTextSize(2);
    canvas->println(title);
    canvas->println();
    canvas->setTextSize(1);
    canvas->setFont(FONT_10x20);

    canvas->fillRect(0, (cursor * 24 + 96 - 20) - scroll * 24, canvas->width(), 24, canvas->color565(255, 64, 0));

    uint16_t menu_size = items.size();
    for (int i = scroll; i < MIN(scroll + MENU_HEIGHT, menu_size); i++) {
        // canvas->fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? canvas->color565(255, 64, 0) :
        // canvas->color565(0, 0, 0));
        int16_t screenI = i - scroll;
        const menu_icon_t *icon = items[i].icon;
        canvas->setTextBound(0, 96 + screenI * 24 - 20, canvas->width(), 24);
        if (icon) {
            // const uint16_t *icon = *icons[i];
            // Cast icon to non-const uint16_t * to avoid warning
            // TODO: Had to do this because I switched to canvas (FreeRTOS experiment)
            // uint16_t *icon2 = (uint16_t *)icon;
            canvas->draw16bitRGBBitmapWithTranColor(
                0, 96 + screenI * 24 - 20, (uint16_t *)icon, canvas->color565(0, 0, 0), 24, 24
            );
        }
        canvas->setCursor(32, 96 + screenI * 24);
        if (items[i].color && cursor != i) {
            canvas->setTextColor(items[i].color);
        } else {
            canvas->setTextColor(canvas->color565(255, 255, 255));
        }
        // gfx->print(i == cursor ? "> " : "  ");
        canvas->println(items[i].title);
    }

    // Draw scrollbar
    if (menu_size > MENU_HEIGHT) {
        int top = 96 - 20;
        int height = MENU_HEIGHT * 24;
        canvas->fillRect(canvas->width() - 8, top, 8, height, canvas->color565(96, 96, 96));
        int barHeight = height * MENU_HEIGHT / menu_size;
        int barTop = top + scroll * height / menu_size;
        canvas->fillRect(canvas->width() - 8, barTop, 8, barHeight, canvas->color565(255, 255, 255));
    }
}

int16_t Menu::getSelectedIndex() {
    int16_t index = selectedIndex;
    selectedIndex = -1;
    return index;
}

Alert::Alert(String title, String message) {
    this->title = title;
    this->message = message;
    this->done = false;
}

void Alert::update() {
    State state = controller.getState();
    if (state.a.justPressed) {
        done = true;
    }
}

void Alert::draw(Arduino_GFX *canvas) {
    int top = canvas->height() / 8;
    int mid = canvas->height() / 8 * 2;
    int bottom = canvas->height() / 8 * 7;

    int left = canvas->width() / 8;
    int right = canvas->width() / 8 * 7;
    int width = right - left;
    int xMargin = 4;

    canvas->fillRect(left, top, width, mid - top, canvas->color565(32, 32, 128));
    canvas->setFont(FONT_6x13);
    canvas->setTextSize(2);
    canvas->setTextBound(left + xMargin, top, width - xMargin * 2, mid - top);
    canvas->setCursor(left + xMargin, top + 13 * 2);
    canvas->println(title);

    canvas->fillRect(left, mid, width, bottom - mid, canvas->color565(32, 96, 96));
    canvas->setFont(FONT_9x15);
    canvas->setTextSize(1);
    canvas->setTextBound(left + xMargin, top, width - xMargin * 2, bottom - mid);
    canvas->setCursor(left + xMargin, mid + 20);
    canvas->println(message);
}

bool Alert::isDone() {
    if (done) {
        done = false;
        return true;
    }
    return false;
}

} // namespace lilka
