-- Character sprites by https://laredgames.itch.io/coins-free

WHITE = display.color565(255, 255, 255)
BLACK = display.color565(0, 0, 0)

ROOT = 'runner/'

STAND_SPRITE = resources.load_image(ROOT .. "boy_stand.bmp", BLACK)
RUN_SPRITES = {
    resources.load_image(ROOT .. "boy_run_1.bmp", BLACK),
    resources.load_image(ROOT .. "boy_run_2.bmp", BLACK),
    resources.load_image(ROOT .. "boy_run_3.bmp", BLACK),
}
JUMP_SPRITE = resources.load_image(ROOT .. "boy_jump.bmp", BLACK)

Player = {
    x = 32,
    y = 128,
    width = 32, -- Розмір спрайту - 32x32
    height = 32,
    sprites = {
        left = {
            stand = resources.flip_image_x(STAND_SPRITE),
            run = {
                resources.flip_image_x(RUN_SPRITES[1]),
                resources.flip_image_x(RUN_SPRITES[2]),
                resources.flip_image_x(RUN_SPRITES[3]),
                resources.flip_image_x(RUN_SPRITES[2]),
            },
            jump = resources.flip_image_x(JUMP_SPRITE),
        },
        right = {
            stand = STAND_SPRITE,
            run = {
                RUN_SPRITES[1],
                RUN_SPRITES[2],
                RUN_SPRITES[3],
                RUN_SPRITES[2],
            },
            jump = JUMP_SPRITE,
        },
    },
    speed_x = 0,
    speed_y = 0,
    look_direction = 1,

    is_airborne = true,
    gravity = 200,
}

function Player:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Player:update(delta)
    -- self.x = self.x + 50 * delta
    self.speed_y = self.speed_y + self.gravity * delta

    self.x = self.x + self.speed_x * delta
    self.y = self.y + self.speed_y * delta

    -- TODO: Calculcate look direction based on speed_x and/or inputs
end

function Player:draw()
    local image
    local sprites
    if self.look_direction == 1 then
        sprites = self.sprites.right
    else
        sprites = self.sprites.left
    end

    if self.is_airborne then
        image = sprites.jump
    elseif math.abs(self.speed_x) < 1 then
        image = sprites.stand[1]
    else
        image = sprites.run[math.floor(util.time() * 15) % #sprites.run + 1]
    end
    -- Малюємо гравця на екрані так, щоб середина нижнього краю спрайту була в координатах (x, y)
    display.draw_image(image, self.x - self.width / 2, self.y - self.height)
end

local player = Player:new({ x = 128, y = 128 })

display.fill_screen(BLACK)
display.render()

function lilka.update(delta)
    player:update(delta)
    local state = controller.get_state()
    if state.start.pressed then
        util.exit()
    end
end

function lilka.draw()
    display.fill_screen(BLACK)
    player:draw()
end
