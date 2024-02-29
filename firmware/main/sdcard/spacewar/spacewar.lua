-- Character sprites by https://laredgames.itch.io/coins-free

WHITE = display.color565(255, 255, 255)
BLACK = display.color565(0, 0, 0)
MAGENTA = display.color565(255, 0, 255)

ROOT = 'spacewar/'

ship_sprites = { resources.load_bitmap(ROOT .. "ship.bmp", MAGENTA) }
for i = 2, 12 do
    local angle = i - 1
    ship_sprites[i] = resources.rotate_bitmap(ship_sprites[1], angle * 30, MAGENTA)
end

Ship = {
    x = display.width / 2,
    y = display.height / 2,
    width = 32, -- Розмір спрайту - 32x32
    height = 32,
    sprites = ship_sprites,
    rotation = 0,
}

function Ship:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Ship:update(delta)
    -- self.x = self.x + 50 * delta
    self.rotation = (self.rotation + 1) % #self.sprites
end

function Ship:draw()
    display.draw_bitmap(self.sprites[self.rotation + 1], self.x - self.width / 2, self.y - self.height / 2)
    -- Якщо ми біля краю екрану, малюємо корабель ще раз на протилежному боці
    if self.x < self.width / 2 or self.x > display.width - self.width / 2 then
        display.draw_bitmap(self.sprites[self.rotation + 1], self.x - self.width / 2 + display.width, self.y - self.height / 2)
    end
    if self.y < self.height / 2 or self.y > display.height - self.height / 2 then
        display.draw_bitmap(self.sprites[self.rotation + 1], self.x - self.width / 2, self.y - self.height / 2 + display.height)
    end
end

local ship = Ship:new()

display.fill_screen(BLACK)
display.render()

function lilka._update(delta)
    display.fill_screen(BLACK)
    ship:update(delta)
    ship:draw()
    display.render()
    if controller.get_state().a.just_pressed then
        util.exit()
    end
end
