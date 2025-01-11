#include "ui.h"

#include "icons/shift.h"
#include "icons/shifted.h"
#include "icons/backspace.h"
#include "icons/whitespace.h"

namespace lilka {

#define LILKA_KB_LANGS  2
#define LILKA_KB_LAYERS 3
#define LILKA_KB_ROWS   4
#define LILKA_KB_COLS   12

#define K_L0            1
#define K_L1            2
#define K_L2            3
#define K_BS            8

/* clang-format off */
// 2 langs, 3 layers, 4 rows, 12 columns
const int16_t keyboard[LILKA_KB_LAYERS * LILKA_KB_LANGS][LILKA_KB_ROWS * LILKA_KB_COLS] = {
    // Layer 0 (base English layout)
    {
        '!',  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  // R1
        '?',  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '=',  // R2
        K_L1, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', K_BS, // R3
        K_L2, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', ' ',  // R4
    },
    // Layer 1 (shifted English layout)
    {
        0,    '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_',  // R1
        0,    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '+',  // R2
        K_L0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', K_BS, // R3
        K_L2, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', ' ',  // R4
    },
    // Layer 2 (special keys - braces, slashes, etc.)
    {
        0,    '{', '}', '[', ']', '|', '\\', ':', ';', '\'', '"', '`',  // R1
        0,    '<', '>', '?', '/', '!', '@',  '#', '$', '%',  '^', '~',  // R2
        0,    '(', ')', '-', '_', '=', '+',  ':', ';', '\'', '"', K_BS, // R3
        K_L0, '<', '>', '?', '/', 0,   0,    0,   0,   0,    0,   ' ',  // R4
    },
    // Layer 3 (base Ukrainian layout)
    {
        '1',  '2',    '3',    '4',    '5',    '6',    '7',    '8',    '9',    '0',    0x0454, 0x0491, // R1 єґ
        '\'', 0x0439, 0x0446, 0x0443, 0x043A, 0x0435, 0x043D, 0x0433, 0x0448, 0x0449, 0x0437, 0x0445, // R2 'йцукенгшщзх
        K_L1, 0x0444, 0x0456, 0x0432, 0x0430, 0x043F, 0x0440, 0x043E, 0x043B, 0x0434, 0x0436, K_BS,   // R3 фівапролдж
        K_L2, 0x044F, 0x0447, 0x0441, 0x043C, 0x0438, 0x0442, 0x044C, 0x0431, 0x044E, 0x0457,    ' ', // R4 ячсмитьбюї
    },

    // Layer 4 (shifted Ukrainian layout)
    {
        '!',  '"',    '#',    ';',    '%',    ':',    '?',    '*',    '(',    ')',    0x0404, 0x0490, // R1 ЄҐ
        '\'', 0x0419, 0x0426, 0x0423, 0x041A, 0x0415, 0x041D, 0x0413, 0x0428, 0x0429, 0x0417, 0x0425, // R2 'ЙЦУКЕНГШЩЗХ
        K_L0, 0x0424, 0x0406, 0x0412, 0x0410, 0x041F, 0x0420, 0x041E, 0x041B, 0x0414, 0x0416, K_BS,   // R3 ФІВАПРОЛДЖ
        K_L2, 0x042F, 0x0427, 0x0421, 0x041C, 0x0418, 0x0422, 0x042C, 0x0411, 0x042E, 0x0407,    ' ', // R4 ЯЧСМИТЬБЮЇ
    },

    // Layer 5 (special keys - braces, slashes, etc.)
    {
        0,    '{', '}', '[', ']', '|', '\\', ':', ';', '\'', '"', '`',  // R1
        0,    '<', '>', '?', '/', '!', '@',  '#', '$', '%',  '^', '~',  // R2
        0,    '(', ')', '-', '_', '=', '+',  ':', ';', '\'', '"', K_BS, // R3
        K_L0, '<', '>', '?', '/', 0,   0,    0,   0,   '.',  ',', ' ',  // R4
    },
};
/* clang-format on */

InputDialog::InputDialog(String title) {
    this->title = title;
    this->masked = false;

    this->done = false;
    this->value = "";

    this->layer = 0;
    this->language = 0;
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
    const int16_t* layerKeys = keyboard[language * LILKA_KB_LAYERS + layer];
    if (state.a.justPressed) {
        // TODO: Handle key press
        int16_t key = layerKeys[cy * LILKA_KB_COLS + cx];
        if (key == K_L0) {
            layer = 0;
        } else if (key == K_L1) {
            layer = 1;
        } else if (key == K_L2) {
            layer = 2;
        } else if (key == K_BS) {
            removeLastChar();
            resetBlink();
        } else if (key) {
            value += unicodeToString(key);
        }
        resetBlink();
    } else if (state.d.justPressed) {
        // change language
        language++;
        if (language >= LILKA_KB_LANGS) {
            language = 0;
        }
    } else if (state.c.justPressed) {
        // change layer
        layer++;
        if (layer >= LILKA_KB_LAYERS) {
            layer = 0;
        }
    } else if (state.b.justPressed) {
        removeLastChar();
        resetBlink();
    } else if (state.start.justPressed) {
        done = true;
    } else if (state.up.justPressed) {
        while (true) {
            cy--;
            if (cy < 0) {
                cy = LILKA_KB_ROWS - 1;
            }
            if (layerKeys[cy * LILKA_KB_COLS + cx]) break;
        }
    } else if (state.down.justPressed) {
        while (true) {
            cy++;
            if (cy > LILKA_KB_ROWS - 1) {
                cy = 0;
            }
            if (layerKeys[cy * LILKA_KB_COLS + cx]) break;
        }
    } else if (state.left.justPressed) {
        while (true) {
            cx--;
            if (cx < 0) {
                cx = LILKA_KB_COLS - 1;
            }
            if (layerKeys[cy * LILKA_KB_COLS + cx]) break;
        }
    } else if (state.right.justPressed) {
        while (true) {
            cx++;
            if (cx > LILKA_KB_COLS - 1) {
                cx = 0;
            }
            if (layerKeys[cy * LILKA_KB_COLS + cx]) break;
        }
    }
    vTaskDelay(LILKA_UI_UPDATE_DELAY_MS / portTICK_PERIOD_MS);
}

void InputDialog::draw(Arduino_GFX* canvas) {
    // Draw keyboard
    int16_t kbTop = canvas->height() / 2 - 32;
    int16_t kbHeight = canvas->height() / 2;
    int16_t kbWidth = canvas->width();

    canvas->fillRect(0, 0, canvas->width(), canvas->height(), lilka::colors::Black);
    canvas->setTextColor(lilka::colors::White);
    canvas->setFont(FONT_10x20);

    canvas->setTextBound(4, 4, canvas->width() - 8, canvas->height() - 8);
    canvas->setCursor(16, 20);
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

    const int16_t* layerKeys = keyboard[language * LILKA_KB_LAYERS + layer];

    const uint16_t buttonWidth = kbWidth / LILKA_KB_COLS;
    const uint16_t buttonHeight = kbHeight / LILKA_KB_ROWS;

    canvas->setTextBound(0, 0, canvas->width(), canvas->height());

    for (int y = 0; y < LILKA_KB_ROWS; y++) {
        for (int x = 0; x < LILKA_KB_COLS; x++) {
            // Draw rect if key is focused
            if (y == cy && x == cx) {
                canvas->fillRect(
                    x * buttonWidth, kbTop + y * buttonHeight, buttonWidth, buttonHeight, lilka::colors::Orange_red
                );
            }
            int16_t key = layerKeys[y * LILKA_KB_COLS + x];
            if (key) {
                String caption;
                if (key == K_L0 || key == K_L1 || key == K_BS || key == ' ') {
                    // Icons
                    const uint16_t* icon = 0;
                    const uint16_t iconWidth = 20;
                    const uint16_t iconHeight = 20;
                    if (key == K_L0 || key == K_L1) {
                        icon = layer == 0 ? shift_img : shifted_img;
                    } else if (key == K_BS) {
                        icon = backspace_img;
                    } else if (key == ' ') {
                        icon = whitespace_img;
                    }
                    if (icon) {
                        canvas->draw16bitRGBBitmapWithTranColor(
                            x * buttonWidth + (buttonWidth / 2) - iconWidth / 2,
                            kbTop + y * buttonHeight + (buttonHeight / 2) - iconHeight / 2,
                            const_cast<uint16_t*>(icon),
                            lilka::colors::Black,
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
                        caption = unicodeToString(key);
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

String InputDialog::unicodeToString(int16_t unicode) {
    if (unicode < 0x80) {
        return String((char)unicode);
    } else if (unicode < 0x800) {
        char utf8[3];
        utf8[0] = 0xC0 | (unicode >> 6);
        utf8[1] = 0x80 | (unicode & 0x3F);
        utf8[2] = '\0';
        return String(utf8);
    } else if (unicode < 0x10000) {
        char utf8[4];
        utf8[0] = 0xE0 | (unicode >> 12);
        utf8[1] = 0x80 | ((unicode >> 6) & 0x3F);
        utf8[2] = 0x80 | (unicode & 0x3F);
        utf8[3] = '\0';
        return String(utf8);
    }
    // Unsupported characters
    return "";
}

void InputDialog::removeLastChar() {
    if (value.length() > 0) {
        auto i = value.length() - 1;
        while (i > 0 && (value[i] & 0xC0) == 0x80) {
            i--;
        }
        value.remove(i);
    }
}

} // namespace lilka
