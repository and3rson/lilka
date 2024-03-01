-- Character sprites by https://laredgames.itch.io/coins-free

WHITE = display.color565(255, 255, 255)
BLACK = display.color565(0, 0, 0)
MAGENTA = display.color565(255, 0, 255)

ROOT = 'spacewar/'

ANGLE_COUNT = 60
ANGLE_STEP = 360 / ANGLE_COUNT

ship_sprites = { resources.load_bitmap(ROOT .. "ship.bmp", MAGENTA) }
for i = 2, ANGLE_COUNT do
    local angle = i - 1
    ship_sprites[i] = resources.rotate_bitmap(ship_sprites[1], angle * ANGLE_STEP, MAGENTA)
end

Ship = {
    x = display.width / 2,
    y = display.height / 2,
    width = 32, -- Розмір спрайту - 32x32
    height = 32,
    sprites = ship_sprites,
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
    display.draw_bitmap(self.sprites[rotation_index], cx, cy)
    -- Якщо ми біля краю екрану, малюємо корабель ще раз на протилежному боці
    if self.x < self.width / 2 or self.x > display.width - self.width / 2 then
        display.draw_bitmap(self.sprites[rotation_index], cx + display.width, cy)
    end
    if self.y < self.height / 2 or self.y > display.height - self.height / 2 then
        display.draw_bitmap(self.sprites[rotation_index], cx, cy + display.height)
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
    local state = controller.get_state()
    -- if state.left.pressed then
    --     ship.rotate(-1 * delta)
    --     ship.rotation = (ship.rotation - 1) % #ship.sprites
    -- end

    -- if state.up.pressed then
    --     ship:set_forward_acceleration(50)
    -- elseif state.down.pressed then
    --     -- ship:set_forward_acceleration(-50)
    -- else
    --     ship:set_forward_acceleration(0)
    -- end

    if state.up.pressed then
        ship:set_forward_acceleration(50)
    else
        ship:set_forward_acceleration(0)
    end

    if state.down.pressed then
        ship:set_angular_speed(90)
    else
        ship:set_angular_speed(0)
    end


    if state.a.just_pressed then
        util.exit()
    end
end
