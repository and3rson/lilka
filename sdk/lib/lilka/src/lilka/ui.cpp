#include "ui.h"

#include "lilka/display.h"
#include "lilka/controller.h"

#include "icons/shift.h"
#include "icons/shifted.h"
#include "icons/backspace.h"
#include "icons/whitespace.h"

namespace lilka {

#define MENU_HEIGHT 5

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

Menu::Menu(String title) {
    this->title = title;
    this->scroll = 0;
    this->setCursor(0);
    this->done = false;
    this->iconImage = new Image(24, 24, display.color565(0, 0, 0), 12, 12);
    this->iconCanvas = new Canvas(24, 24);
    this->lastCursorMove = millis();
}

Menu::~Menu() {
    delete iconImage;
    delete iconCanvas;
}

void Menu::addItem(String title, const menu_icon_t* icon, uint16_t color) {
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
    State state = controller.getState();

    if (state.up.justPressed) {
        // Move cursor up
        if (cursor == 0) {
            cursor = items.size() - 1;
        } else {
            cursor--;
        }
        lastCursorMove = millis();
    } else if (state.down.justPressed) {
        // Move cursor down
        cursor++;
        if (cursor >= items.size()) {
            cursor = 0;
        }
        lastCursorMove = millis();
    } else if (state.a.justPressed) {
        // Execute selected function
        done = true;
    }

    if (cursor < scroll) {
        scroll = cursor;
        // cursorY = cursor * 24 + 96 - 20;
    } else if (cursor > scroll + MENU_HEIGHT - 1) {
        scroll = cursor - MENU_HEIGHT + 1;
        // cursorY = cursor * 24 + 96 - 20;
    }
}

void Menu::draw(Arduino_GFX* canvas) {
    // uint8_t desiredCursorY = cursor * 24 + 96 - 20;
    canvas->fillScreen(canvas->color565(0, 0, 0));
    int8_t angleShift = sin(millis() / 1000.0) * 16;
    // Draw triangle in top-left
    canvas->fillTriangle(0, 0, 48 - angleShift, 0, 0, 48 + angleShift, canvas->color565(0, 0, 255));
    // Draw triangle in top-right
    canvas->fillTriangle(
        canvas->width(),
        0,
        canvas->width() - 48 - angleShift,
        0,
        canvas->width(),
        48 - angleShift,
        canvas->color565(255, 255, 0)
    );
    canvas->setCursor(32, 40);
    canvas->setTextBound(0, 0, canvas->width(), canvas->height());
    canvas->setTextColor(canvas->color565(255, 255, 255));
    canvas->setFont(FONT_6x13);
    canvas->setTextSize(2);
    canvas->println(title);
    canvas->println();
    canvas->setTextSize(1);
    canvas->setFont(FONT_10x20);

    canvas->fillRect(0, (cursor * 24 + 80 - 20) - scroll * 24, canvas->width(), 24, canvas->color565(255, 64, 0));

    uint16_t menu_size = items.size();
    for (int i = scroll; i < MIN(scroll + MENU_HEIGHT, menu_size); i++) {
        // canvas->fillRect(0, 96 + i * 24 - 20, LILKA_DISPLAY_WIDTH, 24, i == cursor ? canvas->color565(255, 64, 0) :
        // canvas->color565(0, 0, 0));
        int16_t screenI = i - scroll;
        const menu_icon_t* icon = items[i].icon;
        canvas->setTextBound(0, 80 + screenI * 24 - 20, canvas->width(), 24);
        if (icon) {
            // const uint16_t *icon = *icons[i];
            // Cast icon to non-const uint16_t * to avoid warning
            // TODO: Had to do this because I switched to canvas (FreeRTOS experiment)
            // uint16_t *icon2 = (uint16_t *)icon;
            // int16_t dx = 0;
            // int16_t dy = 0;
            // if (cursor == i) {
            //     dx = random(0, 3) - 1;
            //     dy = random(0, 3) - 1;
            // }
            if (cursor == i) {
                memcpy(iconImage->pixels, *icon, sizeof(menu_icon_t));
                // Transform t = Transform().rotate(millis() * 30);
                Transform t = Transform().rotate(sin((millis() - lastCursorMove) * PI / 1000) * 30);
                iconCanvas->fillScreen(canvas->color565(0, 0, 0));
                iconCanvas->drawImageTransformed(iconImage, 12, 12, t);
                canvas->draw16bitRGBBitmapWithTranColor(
                    0, 80 + screenI * 24 - 20, iconCanvas->getFramebuffer(), canvas->color565(0, 0, 0), 24, 24
                );
            } else {
                canvas->draw16bitRGBBitmapWithTranColor(
                    0, 80 + screenI * 24 - 20, const_cast<uint16_t*>(*icon), canvas->color565(0, 0, 0), 24, 24
                );
            }
        }
        canvas->setCursor(32, 80 + screenI * 24);
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
        int top = 80 - 20;
        int height = MENU_HEIGHT * 24;
        canvas->fillRect(canvas->width() - 8, top, 8, height, canvas->color565(96, 96, 96));
        int barHeight = height * MENU_HEIGHT / menu_size;
        int barTop = top + scroll * height / menu_size;
        canvas->fillRect(canvas->width() - 8, barTop, 8, barHeight, canvas->color565(255, 255, 255));
    }
}

bool Menu::isFinished() {
    if (done) {
        done = false;
        return true;
    }
    return false;
}

int16_t Menu::getCursor() {
    return cursor;
}

Alert::Alert(String title, String message) {
    this->title = title;
    this->message = message;
    this->done = false;
    this->button = Button::COUNT;
}

void Alert::setTitle(String title) {
    this->title = title;
}

void Alert::setMessage(String message) {
    this->message = message;
}

void Alert::update() {
    State state = controller.getState();
    if (state.a.justPressed || state.start.justPressed) {
        button = state.a.justPressed ? Button::A : Button::START;
        done = true;
    }
}

void Alert::draw(Arduino_GFX* canvas) {
    int top = canvas->height() / 8;
    int mid = canvas->height() / 8 * 2;
    int bottom = canvas->height() / 8 * 7;

    int left = canvas->width() / 8;
    int right = canvas->width() / 8 * 7;
    int width = right - left;
    int xMargin = 4;

    canvas->setTextColor(canvas->color565(255, 255, 255));

    canvas->fillRect(left, top, width, mid - top, canvas->color565(32, 32, 128));
    canvas->setFont(FONT_6x13);
    canvas->setTextSize(2);
    canvas->setTextBound(left + xMargin, top, width - xMargin * 2, mid - top);
    canvas->setCursor(left + xMargin, top + 13 * 2);
    canvas->println(title);

    canvas->fillRect(left, mid, width, bottom - mid, canvas->color565(32, 96, 96));
    canvas->setFont(FONT_9x15);
    canvas->setTextSize(1);
    canvas->setTextBound(left + xMargin, mid, width - xMargin * 2, bottom - mid);
    canvas->setCursor(left + xMargin, mid + 20);
    canvas->println(message);
}

bool Alert::isFinished() {
    if (done) {
        done = false;
        return true;
    }
    return false;
}

Button Alert::getButton() {
    return button;
}

ProgressDialog::ProgressDialog(String title, String message) {
    this->title = title;
    this->message = message;
    this->progress = 0;
}

void ProgressDialog::setProgress(int16_t progress) {
    this->progress = progress;
}

void ProgressDialog::setMessage(String message) {
    this->message = message;
}

void ProgressDialog::draw(Arduino_GFX* canvas) {
    int top = canvas->height() / 8;
    int mid = canvas->height() / 8 * 2;
    int bottom = canvas->height() / 8 * 7;

    int left = canvas->width() / 8;
    int right = canvas->width() / 8 * 7;
    int width = right - left;
    int xMargin = 4;

    canvas->setTextColor(canvas->color565(255, 255, 255));

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

    char buf[8];
    int16_t x, y;
    uint16_t w, h;

    sprintf(buf, "%d%%", progress);
    canvas->getTextBounds(buf, 0, 0, &x, &y, &w, &h);
    // canvas->fillRect(x, y, w, h, canvas->color565(0, 0, 0));
    // canvas->println(buf);

    int barMargin = 8;
    int barHeight = 8;

    int center = (left + right) / 2;

    canvas->fillRect(
        left + barMargin,
        bottom - barMargin - barHeight,
        width - barMargin * 2,
        barHeight,
        canvas->color565(128, 64, 64)
    );
    canvas->fillRect(
        left + barMargin,
        bottom - barMargin - barHeight,
        (width - barMargin * 2) * progress / 100,
        barHeight,
        canvas->color565(255, 128, 0)
    );
    canvas->setCursor(center - w / 2, bottom - barMargin - barHeight - barMargin);
    canvas->setTextBound(0, 0, canvas->width(), canvas->height());
    canvas->print(buf);
}

#define LILKA_KB_LAYERS 3
#define LILKA_KB_ROWS   4
#define LILKA_KB_COLS   11

#define K_L0            1
#define K_L1            2
#define K_L2            3
#define K_BS            8

// 2 layers, 4 rows, 11 columns
const uint8_t keyboard[LILKA_KB_LAYERS][LILKA_KB_ROWS * LILKA_KB_COLS] = {
    // Layer 0
    {
        '!',  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', // R1
        '?',  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', // R2
        K_L1, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', K_BS, // R3
        K_L2, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', ' ', // R4
    },
    // Layer 1 (shifted layer 0)
    {
        0,    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', // R1
        0,    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', // R2
        K_L0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', K_BS, // R3
        K_L2, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', ' ', // R4
    },
    // Layer 2 (special keys - braces, slashes, etc)
    {
        0,    '{', '}', '[', ']', '|', '\\', ':', ';', '\'', '"', // R1
        0,    '<', '>', '?', '/', '!', '@',  '#', '$', '%',  '^', // R2
        0,    '(', ')', '-', '_', '=', '+',  ':', ';', '\'', K_BS, // R3
        K_L0, '<', '>', '?', '/', 0,   0,    0,   0,   0,    ' ', // R4
    },
};

InputDialog::InputDialog(String title) {
    this->title = title;
    this->masked = false;

    this->done = false;
    this->value = "";

    this->layer = 0;
    this->cx = 0;
    this->cy = 0;

    this->lastBlink = 0;
    this->blinkPhase = true;
}

void InputDialog::setMasked(bool masked) {
    this->masked = masked;
}

void InputDialog::setValue(String value) {
    this->value = value;
}

void InputDialog::update() {
    if (millis() - lastBlink > 300) {
        lastBlink = millis();
        blinkPhase = !blinkPhase;
    }

    State state = controller.getState();
    if (state.a.justPressed) {
        // TODO: Handle key press
        const uint8_t* layerKeys = keyboard[layer];
        uint8_t key = layerKeys[cy * LILKA_KB_COLS + cx];
        if (key == K_L0) {
            layer = 0;
        } else if (key == K_L1) {
            layer = 1;
        } else if (key == K_L2) {
            layer = 2;
        } else if (key == K_BS) {
            if (value.length() > 0) {
                value.remove(value.length() - 1);
            }
        } else if (key) {
            value += (char)key;
        }
        resetBlink();
    } else if (state.b.justPressed) {
        if (value.length() > 0) {
            value.remove(value.length() - 1);
        }
        resetBlink();
    } else if (state.start.justPressed) {
        done = true;
    } else if (state.up.justPressed) {
        cy--;
        if (cy < 0) {
            cy = LILKA_KB_ROWS - 1;
        }
    } else if (state.down.justPressed) {
        cy++;
        if (cy > LILKA_KB_ROWS - 1) {
            cy = 0;
        }
    } else if (state.left.justPressed) {
        cx--;
        if (cx < 0) {
            cx = LILKA_KB_COLS - 1;
        }
    } else if (state.right.justPressed) {
        cx++;
        if (cx > LILKA_KB_COLS - 1) {
            cx = 0;
        }
    }
}

void InputDialog::draw(Arduino_GFX* canvas) {
    // Draw keyboard
    int16_t kbTop = canvas->height() / 2 - 32;
    int16_t kbHeight = canvas->height() / 2;
    int16_t kbWidth = canvas->width();

    canvas->fillRect(0, 0, canvas->width(), canvas->height(), canvas->color565(0, 0, 0));
    canvas->setTextColor(canvas->color565(255, 255, 255));
    canvas->setFont(FONT_10x20);

    canvas->setTextBound(4, 4, canvas->width() - 8, canvas->height() - 8);
    canvas->setCursor(4, 20);
    canvas->println(title);

    canvas->setTextBound(16, 16, canvas->width() - 32, canvas->height() - 32);
    canvas->setCursor(16, 48);
    if (masked) {
        for (int i = 0; i < value.length(); i++) {
            canvas->print("*");
        }
    } else {
        canvas->print(value);
    }
    if (blinkPhase) {
        canvas->print("|");
    }

    // canvas->setTextBound(0, kbTop + kbHeight, canvas->width(), canvas->height());

    const uint8_t* layerKeys = keyboard[layer];

    const uint16_t buttonWidth = kbWidth / LILKA_KB_COLS;
    const uint16_t buttonHeight = kbHeight / LILKA_KB_ROWS;

    canvas->setTextBound(0, 0, canvas->width(), canvas->height());

    for (int y = 0; y < LILKA_KB_ROWS; y++) {
        for (int x = 0; x < LILKA_KB_COLS; x++) {
            // Draw rect if key is focused
            if (y == cy && x == cx) {
                canvas->fillRect(
                    x * buttonWidth, kbTop + y * buttonHeight, buttonWidth, buttonHeight, canvas->color565(255, 64, 0)
                );
            }
            uint8_t key = layerKeys[y * LILKA_KB_COLS + x];
            if (key) {
                String caption;
                if (key == K_L0 || key == K_L1 || key == K_BS || key == ' ') {
                    // Icons
                    const uint16_t* icon = 0;
                    const uint16_t iconWidth = 20;
                    const uint16_t iconHeight = 20;
                    if (key == K_L0 || key == K_L1) {
                        icon = layer == 0 ? shift : shifted;
                    } else if (key == K_BS) {
                        icon = backspace;
                    } else if (key == ' ') {
                        icon = whitespace;
                    }
                    if (icon) {
                        canvas->draw16bitRGBBitmapWithTranColor(
                            x * buttonWidth + (buttonWidth / 2) - iconWidth / 2,
                            kbTop + y * buttonHeight + (buttonHeight / 2) - iconHeight / 2,
                            const_cast<uint16_t*>(icon),
                            canvas->color565(0, 0, 0),
                            iconWidth,
                            iconHeight
                        );
                    }
                } else {
                    // Captions
                    if (key == K_L2) {
                        caption = key == K_L0 ? "ab" : key == K_L1 ? "AB" : "!@";
                    } else if (key == K_BS) {
                        caption = "<-";
                    } else {
                        caption = (char)key;
                    }
                    int16_t x1, y1;
                    uint16_t w, h;
                    // Calculate text top-left corner and size
                    canvas->getTextBounds(caption, 0, 0, &x1, &y1, &w, &h);
                    // Print centered text
                    canvas->setCursor(
                        x * buttonWidth + (buttonWidth - w) / 2, kbTop + y * buttonHeight + (buttonHeight - h) / 2 - y1
                    );
                    canvas->print(caption);
                }
            }
        }
    }
}

bool InputDialog::isFinished() {
    if (done) {
        done = false;
        return true;
    }
    return false;
}

String InputDialog::getValue() {
    return value;
}

void InputDialog::resetBlink() {
    lastBlink = millis();
    blinkPhase = true;
}

} // namespace lilka
