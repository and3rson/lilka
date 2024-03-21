#include "ui.h"

#include "icons/shift.h"
#include "icons/shifted.h"
#include "icons/backspace.h"
#include "icons/whitespace.h"

namespace lilka {

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
