#include "ui.h"

namespace lilka {

#define MENU_HEIGHT 5

#define MIN(a, b)   ((a) < (b) ? (a) : (b))

Menu::Menu(String title) {
    this->title = title;
    this->scroll = 0;
    this->setCursor(0);
    this->done = false;
    this->iconImage =
        new Image(menu_icon_width, menu_icon_height, colors::Black, menu_icon_width / 2, menu_icon_height / 2);
    this->iconCanvas = new Canvas(menu_icon_width, menu_icon_height);
    this->lastCursorMove = millis();
    this->button = Button::COUNT;
    this->activationButtons.push_back(Button::A);
    this->firstRender = -1;
}

Menu::~Menu() {
    delete iconImage;
    delete iconCanvas;
}
void Menu::setTitle(String title) {
    this->title = title;
}
void Menu::addItem(
    String title, const menu_icon_t* icon, uint16_t color, String postfix, PMenuItemCallback callback,
    void* callbackData
) {
    items.push_back(
        {.title = title,
         .icon = icon,
         .color = color,
         .postfix = postfix,
         .callback = callback,
         .callbackData = callbackData}
    );
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
    auto cursorLastPos = cursor;
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
    } else if (state.left.justPressed) {
        // Scroll PageUp
        if (cursor == 0) cursor = items.size() - 1;
        else cursor = (cursor - MENU_HEIGHT) <= 0 ? cursor = 0 : (cursor - MENU_HEIGHT);

    } else if (state.right.justPressed) {
        // Scroll PageDown
        if (cursor == items.size() - 1) cursor = 0;
        else cursor = (cursor + MENU_HEIGHT) >= items.size() - 1 ? items.size() - 1 : (cursor + MENU_HEIGHT);
    }
    if (cursorLastPos != cursor) lastCursorMove = millis();
    if (cursor < scroll) {
        scroll = cursor;
    } else if (cursor > scroll + MENU_HEIGHT - 1) {
        scroll = cursor - MENU_HEIGHT + 1;
    }

    for (Button activationButton : activationButtons) {
        lilka::_StateButtons& buttonsArray = *reinterpret_cast<lilka::_StateButtons*>(&state);
        if (buttonsArray[activationButton].justPressed) {
            button = activationButton;
            done = true;
            // Should be made after done flag setup to allow to clear it by isFinished() call
            if (items[cursor].callback) { // call callback if
                items[cursor].callback(items[cursor].callbackData);
            }
        }
    }
    vTaskDelay(LILKA_UI_UPDATE_DELAY_MS / portTICK_PERIOD_MS);
}

// Scroll marquee back and forth based on time, pausing at start and end
int16_t calculateMarqueeShift(uint64_t time, uint16_t maxShift, uint16_t pixelsPerSecond) {
    constexpr uint64_t startDelay = 750;
    uint64_t midTime = maxShift * 1000 / pixelsPerSecond;
    constexpr uint64_t endDelay = 750;
    const uint64_t period = startDelay + 2 * midTime + endDelay;
    const uint64_t phase = time % period;

    int16_t shift = 0;
    if (phase < startDelay) {
        // Start delay
        shift = 0;
    } else if (phase < startDelay + midTime) {
        // Scroll
        shift = (phase - startDelay) * pixelsPerSecond / 1000;
    } else if (phase < startDelay + midTime + endDelay) {
        // End delay
        shift = maxShift;
    } else {
        // Scroll back
        shift = maxShift - (phase - startDelay - midTime - endDelay) * pixelsPerSecond / 1000;
    }
    return -shift;
}

void Menu::draw(Arduino_GFX* canvas) {
    if (this->firstRender == -1) {
        this->firstRender = millis();
    }

    constexpr int16_t scrollbarWidth = 8;
    constexpr int16_t scrollbarLeftPadding = 4;
    constexpr int16_t postfixLeftPadding = 4;
    constexpr int16_t iconWidth = 32;
    constexpr int16_t titleTextHeight = 40;
    constexpr int16_t itemsY = 80;
    constexpr int16_t itemHeight = menu_icon_height;
    uint16_t menu_size = items.size();
    const bool needsScrollbar = menu_size > MENU_HEIGHT;

    canvas->fillScreen(lilka::colors::Black);
    int8_t angleShift = sin(millis() / 1000.0) * 16;
    // Draw triangle in top-left
    canvas->fillTriangle(0, 0, 48 - angleShift, 0, 0, 48 + angleShift, lilka::colors::Blue);
    // Draw triangle in top-right
    canvas->fillTriangle(
        canvas->width(),
        0,
        canvas->width() - 48 - angleShift,
        0,
        canvas->width(),
        48 - angleShift,
        lilka::colors::Yellow
    );

    const uint16_t titleWidth = getTextWidth(FONT_6x13, title.c_str()) * 2;
    const uint16_t titleWidthAvailable = canvas->width() - 64;
    if (titleWidth > titleWidthAvailable) {
        // Marquee
        Canvas marquee(titleWidthAvailable, titleTextHeight + 8);
        marquee.fillScreen(lilka::colors::Black);
        marquee.setFont(FONT_6x13);
        marquee.setTextSize(2);
        marquee.setCursor(
            calculateMarqueeShift(millis() - firstRender, titleWidth - titleWidthAvailable, 50), titleTextHeight
        );
        marquee.setTextColor(lilka::colors::White);
        marquee.println(title);
        canvas->draw16bitRGBBitmapWithTranColor(
            32, 0, marquee.getFramebuffer(), lilka::colors::Black, marquee.width(), marquee.height()
        );
    } else {
        // Text fits
        canvas->setFont(FONT_6x13);
        canvas->setTextSize(2);
        canvas->setCursor(32, 40);
        canvas->setTextColor(lilka::colors::White);
        canvas->setTextBound(32, 8, titleWidthAvailable, titleTextHeight);
        canvas->println(title);
    }

    canvas->fillRect(
        0,
        (cursor * itemHeight + itemsY - 20) - scroll * itemHeight,
        canvas->width() - (needsScrollbar ? scrollbarWidth : 0),
        itemHeight,
        lilka::colors::Orange_red
    );

    for (int i = scroll; i < MIN(scroll + MENU_HEIGHT, menu_size); i++) {
        int16_t screenI = i - scroll;
        const menu_icon_t* icon = items[i].icon;
        canvas->setTextBound(0, itemsY + screenI * itemHeight - 20, canvas->width(), itemHeight);
        if (icon) {
            if (cursor == i) {
                memcpy(iconImage->pixels, *icon, sizeof(menu_icon_t));
                // Transform t = Transform().rotate(millis() * 30);
                Transform t = Transform().rotate(sin((millis() - lastCursorMove) * PI / 1000) * 30);
                iconCanvas->fillScreen(lilka::colors::Black);
                iconCanvas->drawImageTransformed(iconImage, 12, 12, t);
                canvas->draw16bitRGBBitmapWithTranColor(
                    0,
                    itemsY + screenI * itemHeight - 20,
                    iconCanvas->getFramebuffer(),
                    lilka::colors::Black,
                    menu_icon_width,
                    menu_icon_height
                );
            } else {
                canvas->draw16bitRGBBitmapWithTranColor(
                    0,
                    itemsY + screenI * itemHeight - 20,
                    const_cast<uint16_t*>(*icon),
                    lilka::colors::Black,
                    menu_icon_width,
                    menu_icon_height
                );
            }
        }

        uint16_t postfixWidth = 0;
        if (items[i].postfix.length()) {
            canvas->setTextSize(1);
            canvas->setFont(FONT_10x20);
            canvas->setTextColor(lilka::colors::White);
            // Calculate postfix width
            int16_t x1, y1;
            uint16_t h;
            (void)x1;
            (void)y1;
            (void)h;
            canvas->setTextBound(0, 0, canvas->width(), canvas->height());
            canvas->getTextBounds(items[i].postfix, 0, 0, &x1, &y1, &postfixWidth, &h);
            canvas->setCursor(
                canvas->width() - postfixWidth - scrollbarWidth - scrollbarLeftPadding, itemsY + screenI * itemHeight
            );
            canvas->println(items[i].postfix);
        }

        int16_t widthAvailable =
            canvas->width() - iconWidth - postfixWidth - scrollbarWidth - scrollbarLeftPadding - postfixLeftPadding;
        if (widthAvailable < 0) {
            // No space for title
            continue;
        }

        uint16_t nameWidth = getTextWidth(FONT_10x20, items[i].title.c_str()) + 1;
        if (nameWidth > widthAvailable && cursor == i) {
            // Marquee
            Canvas marquee(widthAvailable, itemHeight);
            marquee.fillScreen(lilka::colors::Black);
            marquee.setFont(FONT_10x20);
            marquee.setCursor(calculateMarqueeShift(millis() - lastCursorMove, nameWidth - widthAvailable, 50), 20);
            marquee.setTextColor(lilka::colors::White);
            marquee.println(items[i].title);
            canvas->draw16bitRGBBitmapWithTranColor(
                iconWidth,
                itemsY + screenI * itemHeight - 20,
                marquee.getFramebuffer(),
                lilka::colors::Black,
                widthAvailable,
                itemHeight
            );
        } else {
            // Text fits
            canvas->setTextSize(1);
            canvas->setFont(FONT_10x20);
            canvas->setCursor(iconWidth, itemsY + screenI * itemHeight);
            canvas->setTextBound(iconWidth, itemsY + screenI * itemHeight - 20, widthAvailable, itemHeight);
            if (items[i].color && cursor != i) {
                canvas->setTextColor(items[i].color);
            } else {
                canvas->setTextColor(lilka::colors::White);
            }
            canvas->println(items[i].title);
        }
    }

    // Draw scrollbar
    if (needsScrollbar) {
        int top = itemsY - 20;
        int height = MENU_HEIGHT * itemHeight;
        canvas->fillRect(canvas->width() - 5, top, 2, height, lilka::colors::White);
        canvas->fillRect(canvas->width() - 8, top, 8, 2, lilka::colors::White);
        canvas->fillRect(canvas->width() - 8, top + height - 2, 8, 2, lilka::colors::White);
        int barHeight = height * MENU_HEIGHT / menu_size;
        int barTop = top + scroll * height / menu_size;
        canvas->fillRect(canvas->width() - 8, barTop, 8, barHeight, lilka::colors::White);
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
bool Menu::setItem(int16_t index, String title, const menu_icon_t* icon, uint16_t color, String postfix) {
    if (index > items.size() - 1) {
        return false;
    } else {
        items[index].title = title;
        items[index].icon = icon;
        items[index].color = color;
        items[index].postfix = postfix;
        return true;
    }
}
bool Menu::getItem(int16_t index, MenuItem* menuItem) {
    if ((menuItem == NULL) || index > items.size() - 1 || index < 0) {
        return false;
    } else {
        *menuItem = items[index];
        return true;
    }
}
void Menu::clearItems() {
    setCursor(0);
    items.clear();
}
int16_t Menu::getItemCount() {
    return items.size();
}
void Menu::addActivationButton(Button activationButton) {
    activationButtons.push_back(activationButton);
}
void Menu::removeActivationButton(Button activationButton) {
    activationButtons.erase(
        std::remove(activationButtons.begin(), activationButtons.end(), activationButton), activationButtons.end()
    );
}

Button Menu::getButton() {
    return button;
}

} // namespace lilka
