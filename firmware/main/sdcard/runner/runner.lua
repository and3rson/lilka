-- Character sprites by https://laredgames.itch.io/coins-free

local WHITE = display.color565(255, 255, 255)
local BLACK = display.color565(0, 0, 0)

local ROOT = 'runner/'

local Player = {
    x = 0,
    y = 0,
    width = 32, -- Розмір спрайту - 32x32
    height = 32,
    sprites = {
        -- stand = { resources.load_bitmap(ROOT .. "boy_stand.bmp") },
        stand = { resources.load_bitmap("face.bmp", BLACK) },
        run = {
            resources.load_bitmap(ROOT .. "boy_run_1.bmp", BLACK),
            resources.load_bitmap(ROOT .. "boy_run_2.bmp", BLACK),
            resources.load_bitmap(ROOT .. "boy_run_3.bmp", BLACK),
        },
    },
}

function Player:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Player:draw()
    -- Малюємо гравця на екрані так, щоб середина нижнього краю спрайту була в координатах (x, y)
    display.draw_bitmap(self.sprites.run[1], self.x - self.width / 2, self.y - self.height)
end

local player = Player:new({ x = 128, y = 128 })

-- display.fill_screen(WHITE)
-- display.draw_bitmap(sprites.run[1], 128, 128)
-- display.render()
-- util.delay(1)

-- display.fill_screen(WHITE)
-- display.render()
-- util.delay(1)
-- display.fill_screen(BLACK)
-- display.render()
-- util.delay(1)
