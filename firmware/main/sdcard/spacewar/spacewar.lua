-- Character sprites by https://laredgames.itch.io/coins-free

WHITE = display.color565(255, 255, 255)
BLACK = display.color565(0, 0, 0)
MAGENTA = display.color565(255, 0, 255)

ROOT = 'spacewar/'

ANGLE_COUNT = 60
ANGLE_STEP = 360 / ANGLE_COUNT

SHIP_SPRITES = { resources.load_image(ROOT .. "ship.bmp", MAGENTA) }
SHIP_FORWARD_SPRITES = { resources.load_image(ROOT .. "ship_forward.bmp", MAGENTA) }
SHIP_BACKWARD_SPRITES = { resources.load_image(ROOT .. "ship_backward.bmp", MAGENTA) }
for i = 2, ANGLE_COUNT do
    local angle = i - 1
    SHIP_SPRITES[i] = resources.rotate_image(SHIP_SPRITES[1], angle * ANGLE_STEP, MAGENTA)
    SHIP_FORWARD_SPRITES[i] = resources.rotate_image(SHIP_FORWARD_SPRITES[1], angle * ANGLE_STEP, MAGENTA)
    SHIP_BACKWARD_SPRITES[i] = resources.rotate_image(SHIP_BACKWARD_SPRITES[1], angle * ANGLE_STEP, MAGENTA)
end

ASTEROID_16_SPRITES = { resources.load_image(ROOT .. "asteroid_16.bmp", MAGENTA) }
ASTEROID_32_SPRITES = { resources.load_image(ROOT .. "asteroid_32.bmp", MAGENTA) }
ASTEROID_48_SPRITES = { resources.load_image(ROOT .. "asteroid_48.bmp", MAGENTA) }
ASTEROID_64_SPRITES = { resources.load_image(ROOT .. "asteroid_64.bmp", MAGENTA) }
for i = 2, 8 do
    ASTEROID_16_SPRITES[i] = resources.rotate_image(ASTEROID_16_SPRITES[1], i * 45, MAGENTA)
    ASTEROID_32_SPRITES[i] = resources.rotate_image(ASTEROID_32_SPRITES[1], i * 45, MAGENTA)
    ASTEROID_48_SPRITES[i] = resources.rotate_image(ASTEROID_48_SPRITES[1], i * 45, MAGENTA)
    ASTEROID_64_SPRITES[i] = resources.rotate_image(ASTEROID_64_SPRITES[1], i * 45, MAGENTA)
end

Ship = {
    x = display.width / 2,
    y = display.height / 2,
    width = SHIP_SPRITES[1].width, -- Розмір спрайту
    height = SHIP_SPRITES[1].height,
    sprites = SHIP_SPRITES,
    forward_sprites = SHIP_FORWARD_SPRITES,
    backward_sprites = SHIP_BACKWARD_SPRITES,
    rotation = 0, -- Поворот корабля в градусах
    speed_x = 0,
    speed_y = 0,
    accel_forward = 0,
    angular_speed = 0,
    max_speed_x = display.width,
    max_speed_y = display.height,
    -- accel_y = 0,
}

function Ship:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Ship:set_forward_acceleration(accel)
    self.accel_forward = accel
end

function Ship:set_angular_speed(speed)
    self.angular_speed = speed
end

function Ship:update(delta)
    local dir_x = math.cos(math.rad(-self.rotation))
    local dir_y = math.sin(math.rad(-self.rotation))

    self.speed_x = self.speed_x + self.accel_forward * delta * dir_x
    self.speed_y = self.speed_y + self.accel_forward * delta * dir_y

    self.speed_x = math.clamp(self.speed_x, -self.max_speed_x, self.max_speed_x)
    self.speed_y = math.clamp(self.speed_y, -self.max_speed_y, self.max_speed_y)

    self.x = self.x + self.speed_x * delta
    self.y = self.y + self.speed_y * delta

    self.x = self.x % display.width
    self.y = self.y % display.height

    self.rotation = (self.rotation + self.angular_speed * delta) % 360
end

function Ship:draw()
    local rotation_index = math.floor(self.rotation / ANGLE_STEP) + 1
    -- Координати верхнього лівого кута спрайту
    local cx = math.floor(self.x - self.width / 2)
    local cy = math.floor(self.y - self.height / 2)

    local sprite_set
    if self.accel_forward > 0 then
        sprite_set = self.forward_sprites
    elseif self.accel_forward < 0 then
        sprite_set = self.backward_sprites
    else
        sprite_set = self.sprites
    end

    display.draw_image(sprite_set[rotation_index], cx, cy)
    -- Якщо ми біля краю екрану, малюємо корабель ще раз на протилежному боці
    if self.x < self.width / 2 or self.x > display.width - self.width / 2 then
        display.draw_image(sprite_set[rotation_index], cx + display.width, cy)
    end
    if self.y < self.height / 2 or self.y > display.height - self.height / 2 then
        display.draw_image(sprite_set[rotation_index], cx, cy + display.height)
    end
end

Bullet = {
    x = 0,
    y = 0,
    speed_x = 0,
    speed_y = 0,
    radius = 2,
    dead = false,
}

function Bullet:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Bullet:update(delta)
    if self.dead then
        return
    end
    self.x = self.x + self.speed_x * delta
    self.y = self.y + self.speed_y * delta
    if self.x < 0 or self.x > display.width or self.y < 0 or self.y > display.height then
        self.dead = true
    end
end

function Bullet:draw()
    display.draw_circle(math.floor(self.x), math.floor(self.y), self.radius, WHITE)
end

local ship = Ship:new()
local bullets = {}

display.fill_screen(BLACK)
display.render()

function lilka._update(delta)
    ship:update(delta)
    for _, bullet in ipairs(bullets) do
        bullet:update(delta)
    end
    -- Remove dead bullets
    for i = #bullets, 1, -1 do
        if bullets[i].dead then
            table.remove(bullets, i)
        end
    end

    local state = controller.get_state()
    -- if state.left.pressed then
    --     ship.rotate(-1 * delta)
    --     ship.rotation = (ship.rotation - 1) % #ship.sprites
    -- end

    if state.up.pressed then
        ship:set_forward_acceleration(50)
    elseif state.down.pressed then
        ship:set_forward_acceleration(-50)
    else
        ship:set_forward_acceleration(0)
    end

    if state.left.pressed then
        ship:set_angular_speed(90)
    elseif state.right.pressed then
        ship:set_angular_speed(-90)
    else
        ship:set_angular_speed(0)
    end

    if state.a.just_pressed then
        local bullet = Bullet:new()
        local dir_x = math.cos(math.rad(-ship.rotation))
        local dir_y = math.sin(math.rad(-ship.rotation))
        bullet.x = ship.x + dir_x * ship.width / 2
        bullet.y = ship.y + dir_y * ship.height / 2
        bullet.speed_x = 200 * dir_x
        bullet.speed_y = 200 * dir_y
        table.insert(bullets, bullet)
    end

    if state.start.just_pressed then
        util.exit()
    end

    display.fill_screen(BLACK)
    ship:draw()
    for _, bullet in ipairs(bullets) do
        bullet:draw()
    end
    display.render()
end
