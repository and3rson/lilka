--[[
    Menu
    Автор: Андрій Ситников (drronnie85)
    Опис: Клас для малювання меню
--]]

BLACK = display.color565(0, 0, 0)
BLUE = display.color565(0, 0, 255)
YELLOW = display.color565(255, 255, 0)

local MENU_HEIGHT = 5
Menu = {
    currentIndex = 0,
    left = 0,
    top = 0,
    width = display.width,
    height = display.height,

}

function Menu:new(obj)
    obj = obj or {}
    setmetatable(obj, self)
    self.__index = self
    return obj
end

function Menu:update(uistack, delta)
    display.fill_screen(BLACK);
    local angleShift = math.sin(util.time()) * 16;
    display.fill_triangle(0, 0, 48 - angleShift, 0, 0, 48 + angleShift, BLUE);
    display.fill_triangle(display.width, 0, display.width - 48 - angleShift, 0, display.width, 48 - angleShift, YELLOW);

    local scrollbarWidth = 8
    local scrollbarLeftPadding = 4
    local postfixLeftPadding = 4
    local iconWidth = 32
    local titleTextHeight = 40
    local itemsY = 80
    local itemHeight = menu_icon_height
    local menu_size = #self.items

    local needsScrollbar = menu_size > MENU_HEIGHT

    if self.titleWidth == nil then
        self.titleWidth = display.get_text_width("6x13", self.title) * 2
    end
    local titleWidth = self.titleWidth
    local titleWidthAvailable = display.width - 64
    if (titleWidth > titleWidthAvailable) {
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


    --[[

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

--]]
end

return Menu