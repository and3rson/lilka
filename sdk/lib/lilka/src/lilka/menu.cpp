#include "ui.h"

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
    this->button = Button::COUNT;
    this->activationButtons.push_back(Button::A);
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
    }

    if (cursor < scroll) {
        scroll = cursor;
        // cursorY = cursor * 24 + 96 - 20;
    } else if (cursor > scroll + MENU_HEIGHT - 1) {
        scroll = cursor - MENU_HEIGHT + 1;
        // cursorY = cursor * 24 + 96 - 20;
    }

    for (Button activationButton : activationButtons) {
        lilka::_StateButtons& buttonsArray = *reinterpret_cast<lilka::_StateButtons*>(&state);
        if (buttonsArray[activationButton].justPressed) {
            button = activationButton;
            done = true;
        }
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

void Menu::addActivationButton(Button activationButton) {
    activationButtons.push_back(activationButton);
}

Button Menu::getButton() {
    return button;
}

} // namespace lilka
