-- Character sprites by https://laredgames.itch.io/coins-free

WHITE = display.color565(255, 255, 255)
BLACK = display.color565(0, 0, 0)
MAGENTA = display.color565(255, 0, 255)

-- TODO: Get rid of this
ROOT = 'asteroids/'

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
    max_speed_x = display.width / 2,
    max_speed_y = display.width / 2,
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
    if cx < 0 then
        display.draw_image(sprite_set[rotation_index], cx + display.width, cy)
    elseif cx + self.width > display.width then
        display.draw_image(sprite_set[rotation_index], cx - display.width, cy)
    end
    if cy < 0 then
        display.draw_image(sprite_set[rotation_index], cx, cy + display.height)
    elseif cy + self.height > display.height then
        display.draw_image(sprite_set[rotation_index], cx, cy - display.height)
    end
end

Bullet = {
    x = 0,
    y = 0,
    speed_x = 0,
    speed_y = 0,
    distance_traveled = 0,
    dead = false,

    radius = 1,
    max_distance = math.sqrt(display.width * display.width + display.height * display.height) / 2,
}

function Bullet:new(x, y)
    local o = { x = x, y = y }
    setmetatable(o, self)
    self.__index = self
    return o
end

function Bullet:update(delta)
    if self.dead then
        return
    end
    local dx = self.speed_x * delta
    local dy = self.speed_y * delta
    self.x = self.x + dx
    self.y = self.y + dy
    -- Якщо куля виходить за межі екрану, вона з'являється на протилежному боці
    self.x = self.x % display.width
    self.y = self.y % display.height

    -- Коли куля долає велику відстань, вона вмирає
    self.distance_traveled = self.distance_traveled + math.sqrt(dx * dx + dy * dy)
    if self.distance_traveled > self.max_distance then
        self.dead = true
    end
end

function Bullet:draw()
    display.fill_circle(math.floor(self.x), math.floor(self.y), self.radius, WHITE)
end

Asteroid = {
    x = 0,
    y = 0,
    speed_x = 0,
    speed_y = 0,
    size = 0,
    --- @type table|nil
    sprite = nil,
    offscreen = false,
    dead = false,
}

function Asteroid:new(x, y, speed_x, speed_y)
    local size = math.random(1, 4)
    local sprite
    if size == 1 then
        sprite = ASTEROID_16_SPRITES[math.random(1, 8)]
    elseif size == 2 then
        sprite = ASTEROID_32_SPRITES[math.random(1, 8)]
    elseif size == 3 then
        sprite = ASTEROID_48_SPRITES[math.random(1, 8)]
    else
        sprite = ASTEROID_64_SPRITES[math.random(1, 8)]
    end
    local o = {
        x = x,
        y = y,
        speed_x = speed_x,
        speed_y = speed_y,
        size = size,
        sprite = sprite,
        offscreen = x < sprite.width / 2 or x > display.width - sprite.width / 2 or y < sprite.width / 2 or
            y > display.height - sprite.height / 2,
    }
    setmetatable(o, self)
    self.__index = self
    return o
end

function Asteroid:update(delta)
    if self.dead then
        return
    end
    self.x = self.x + self.speed_x * delta
    self.y = self.y + self.speed_y * delta
    if self.offscreen then
        self.offscreen = self.x < self.sprite.width / 2 or self.x > display.width - self.sprite.width / 2 or
            self.y < self.sprite.width / 2 or self.y > display.height - self.sprite.height / 2
    else
        self.x = self.x % display.width
        self.y = self.y % display.height
    end
end

function Asteroid:draw()
    local cx = math.floor(self.x - self.sprite.width / 2)
    local cy = math.floor(self.y - self.sprite.height / 2)
    display.draw_image(self.sprite, cx, cy)
    if not self.offscreen then
        if cx < 0 then
            display.draw_image(self.sprite, cx + display.width, cy)
        elseif cx + self.sprite.width > display.width then
            display.draw_image(self.sprite, cx - display.width, cy)
        end
        if cy < 0 then
            display.draw_image(self.sprite, cx, cy + display.height)
        elseif cy + self.sprite.height > display.height then
            display.draw_image(self.sprite, cx, cy - display.height)
        end
    end
end

local ship = Ship:new()
local bullets = {}
local asteroids = {}

display.fill_screen(BLACK)
display.render()

local next_asteroid_spawn_time = 0

function lilka.update(delta)
    ship:update(delta)
    for _, bullet in ipairs(bullets) do
        bullet:update(delta)
    end
    for _, asteroid in ipairs(asteroids) do
        asteroid:update(delta)
    end
    -- Remove dead bullets
    for i = #bullets, 1, -1 do
        if bullets[i].dead then
            table.remove(bullets, i)
        end
    end
    -- Remove dead asteroids
    for i = #asteroids, 1, -1 do
        if asteroids[i].dead then
            table.remove(asteroids, i)
        end
    end

    local state = controller.get_state()
    -- if state.left.pressed then
    --     ship.rotate(-1 * delta)
    --     ship.rotation = (ship.rotation - 1) % #ship.sprites
    -- end

    if state.up.pressed then
        ship:set_forward_acceleration(100)
    elseif state.down.pressed then
        ship:set_forward_acceleration(-100)
    else
        ship:set_forward_acceleration(0)
    end

    if state.left.pressed then
        ship:set_angular_speed(180)
    elseif state.right.pressed then
        ship:set_angular_speed(-180)
    else
        ship:set_angular_speed(0)
    end

    if state.a.just_pressed then
        local dir_x = math.cos(math.rad(-ship.rotation))
        local dir_y = math.sin(math.rad(-ship.rotation))
        local bullet = Bullet:new(ship.x + dir_x * ship.width / 2, ship.y + dir_y * ship.height / 2)
        bullet.speed_x = ship.speed_x / 2 + display.width * dir_x
        bullet.speed_y = ship.speed_y / 2 + display.width * dir_y
        table.insert(bullets, bullet)
    end

    if state.start.just_pressed then
        util.exit()
    end

    -- Якщо минув визначений час і якщо на екрані менше 5 астероїдів, то додаємо новий
    if next_asteroid_spawn_time < util.time() and #asteroids < 5 then
        local x, y
        -- Spawn asteroid outside of screen bounds
        if math.random() < 0.5 then
            -- Лівий або правий край екрану
            x = math.random() > 0.5 and -100 or display.width + 100
            y = math.random(0, display.height)
        else
            -- Верхній або нижній край екрану
            x = math.random(0, display.width)
            y = math.random() > 0.5 and -100 or display.height + 100
        end
        -- Set direction towards the random point on the screen
        local target_x = math.random(0, display.width)
        local target_y = math.random(0, display.height)

        local dir_x = target_x - x
        local dir_y = target_y - y
        dir_x, dir_y = math.norm(dir_x, dir_y)

        table.insert(asteroids, Asteroid:new(x, y, dir_x * 50, dir_y * 50))
        next_asteroid_spawn_time = util.time() + math.random(2, 4)
    end

    -- Check for collisions
    for _, asteroid in ipairs(asteroids) do
        for _, bullet in ipairs(bullets) do
            if math.dist(asteroid.x, asteroid.y, bullet.x, bullet.y) < asteroid.sprite.width / 2 then
                asteroid.dead = true
                bullet.dead = true
            end
        end
        if math.dist(asteroid.x, asteroid.y, ship.x, ship.y) < asteroid.sprite.width / 2 then
            -- TODO: Game over
            util.exit()
        end
    end
end

function lilka.draw()
    display.fill_screen(BLACK)
    ship:draw()
    for _, bullet in ipairs(bullets) do
        bullet:draw()
    end
    for _, asteroid in ipairs(asteroids) do
        asteroid:draw()
    end
end
