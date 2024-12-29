--[[
    Alert
    Автор: Андрій Ситников (drronnie85)
    Опис: Клас для малювання сповіщень
--]]

WHITE = display.color565(255, 255, 255)
MIDNIGHT_BLUE = display.color565(25, 25, 112)

Alert = {
    title = nil,
    message = nil
}

function Alert:new(title, message)
    obj = {
        title = title,
        message = message
    }
    setmetatable(obj, self)
    self.__index = self
    return obj
end

function Alert:draw(delta)
    local top = display.height / 8
    local mid = display.height / 8 * 2
    local bottom = display.height / 8 * 7

    local left = display.width / 8
    local right = display.width / 8 * 7
    local width = right - left
    local xMargin = 4

    title = self.title or "Інформація"
    message = self.message or ""

    display.set_text_color(WHITE)
    display.fill_rect(left, top, width, mid - top, MIDNIGHT_BLUE)
    display.set_font("6x13")
    display.set_text_size(2)
    display.set_text_bound(left + xMargin, top, width - xMargin * 2, mid - top)
    display.set_cursor(left + xMargin, top + 13 * 2)
    display.print(title)
    display.fill_rect(left, mid, width, bottom - mid, display.color565(32, 96, 96))
    display.set_font("9x15")
    display.set_text_size(1)
    display.set_text_bound(left + xMargin, mid, width - xMargin * 2, bottom - mid)
    display.set_cursor(left + xMargin, mid + 20)
    display.print(message);

end

return Alert