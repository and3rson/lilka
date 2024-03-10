--[[

       mm                                                      mm mm                          mm
      ####                                                                                    ##
      ####     m#####m  ######   m####m   ##m###m    m####m    ####      ####### ##   m##     ##
     ##  ##   ##          ##    ##mmmm##  ##    ##  ##    ##     ##      ##   ## ##  ####     ##
     ######   ##          ##    ##        ##    ##  ##    ##     ##      ##   ## ##m#  ##
    m##  ##m   ##mmmm#    ##     ##mmmm#  ###mm##    ##mm##   mmm##mmm  m##mmm## ###   ##     mm
                                          ##                            ##     #
                                          ##
    Автор: Андрій "and3rson" Дунай
    Ідея: Asteroids (1979) від Atari
    Шрифти, використані в графіці: https://ukrfonts.com/info/index.php?v=20&id=5730

--]]

WHITE = display.color565(255, 255, 255)
BLACK = display.color565(0, 0, 0)
MAGENTA = display.color565(255, 0, 255)

-- TODO: Виколистовується, якщо потрібно запускати гру через режим "Live Lua"
-- ROOT = 'asteroids/'
ROOT = ''

-- lilka.show_fps = true

-------------------------------------------------------------------------------
-- Загальні константи
-------------------------------------------------------------------------------

ANGLE_COUNT = 60
ANGLE_STEP = 360 / ANGLE_COUNT

display.fill_screen(BLACK)
display.set_cursor(8, display.height / 2 - 8)
display.print("Завантаження...")
display.queue_draw()

-------------------------------------------------------------------------------
-- Завантаження ресурсів
-------------------------------------------------------------------------------

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

BANNER = {}
for i = 1, 4 do
    BANNER[i] = resources.load_image(ROOT .. "banner" .. i .. ".bmp")
end
PRESS_START = resources.load_image(ROOT .. "press_start.bmp")
YOU_ARE_DEAD = resources.load_image(ROOT .. "game_over.bmp")

-------------------------------------------------------------------------------
-- Ігрові класи
-------------------------------------------------------------------------------

--
-- Корабель, яким керує гравець
--

Ship = {
    x = display.width / 2,
    y = display.height / 2,
    width = SHIP_SPRITES[1].width, -- Розмір спрайту
    height = SHIP_SPRITES[1].height,
    sprites = SHIP_SPRITES,
    forward_sprites = SHIP_FORWARD_SPRITES,
    backward_sprites = SHIP_BACKWARD_SPRITES,
    rotation = 270, -- Кут обороту корабля в градусах за годинниковою стрілкою
    speed_x = 0,
    speed_y = 0,
    accel_forward = 0,
    angular_speed = 0,
    max_speed_x = display.width / 2,
    max_speed_y = display.width / 2,
    killed_at = -1,
    dead = false,
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

function Ship:kill()
    self.killed_at = util.time()
    self.dead = true
end

function Ship:update(delta)
    local dir_x, dir_y = math.rotate(1, 0, self.rotation)

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
    if self.dead then
        local time_since_death = util.time() - self.killed_at
        -- Малюємо коло, яке збільшується та зменшується впродовж 1 секунди
        if time_since_death < 1 then
            local radius = math.sin(time_since_death * 2 * math.pi) * 32
            display.fill_circle(self.x, self.y, radius, WHITE)
        end
    else
        local rotation_index = math.floor(self.rotation / ANGLE_STEP) + 1
        -- Координати верхнього лівого кута спрайту
        local cx = self.x - self.width / 2
        local cy = self.y - self.height / 2

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
end

--
-- Куля, якою стріляє корабель
--

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

function Bullet:kill()
    self.dead = true
end

function Bullet:draw()
    display.fill_circle(self.x, self.y, self.radius, WHITE)
end

--
-- Астероїд, який рухається по екрану
--

Asteroid = {
    x = 0,
    y = 0,
    speed_x = 0,
    speed_y = 0,
    size = 0,
    --- @type table|nil
    sprite = nil,
    offscreen = false,
    killed_at = -1,
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

function Asteroid:kill()
    self.killed_at = util.time()
    self.dead = true
end

function Asteroid:draw()
    if self.dead then
        local time_since_death = util.time() - self.killed_at
        -- Малюємо коло, яке збільшується та зменшується впродовж 1 секунди
        if time_since_death < 1 then
            local radius = math.sin(time_since_death * 2 * math.pi) * self.sprite.width / 2
            display.fill_circle(self.x, self.y, radius, WHITE)
        end
    else
        local cx = self.x - self.sprite.width / 2
        local cy = self.y - self.sprite.height / 2
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
end

-------------------------------------------------------------------------------
-- Змінні, які зберігають стан гри та всіх ігрових об'єктів.
-------------------------------------------------------------------------------

--- @type table
local ship = nil -- Ship:new()
local bullets = {}
local asteroids = {}

-- Час, коли наступний астероїд з'явиться на екрані
local next_asteroid_spawn_time = 0

-- Перелік станів гри
STATES = {
    HELLO = 0,     -- Початковий екран
    IN_GAME = 1,   -- Гра
    GAME_OVER = 2, -- Кінець гри
}
local game_state = STATES.HELLO

-------------------------------------------------------------------------------
-- Головні цикли гри
-------------------------------------------------------------------------------

--
-- Цикл обробки введення та оновлення стану гри
--
function lilka.update(delta)
    if game_state == STATES.HELLO then
        -- Якщо гравець натиснув кнопку "Старт", то починаємо гру
        if controller.get_state().start.just_pressed then
            game_state = STATES.IN_GAME
            ship = Ship:new()
            bullets = {}
            asteroids = {}
        end
    else
        -- Оновлюємо стан корабля, куль та астероїдів
        ship:update(delta)
        for _, bullet in ipairs(bullets) do
            bullet:update(delta)
        end
        for _, asteroid in ipairs(asteroids) do
            asteroid:update(delta)
        end
        -- Видаляємо мертві кулі
        for i = #bullets, 1, -1 do
            if bullets[i].dead then
                table.remove(bullets, i)
            end
        end
        -- Видаляємо мертві астероїди
        for i = #asteroids, 1, -1 do
            if asteroids[i].dead and util.time() - asteroids[i].killed_at > 1 then
                table.remove(asteroids, i)
            end
        end

        -- Отримуємо стан контролера та обробляємо введення
        local state = controller.get_state()

        -- Керуємо кораблем, якщо він не мертвий
        if not ship.dead then
            -- Рух вперед та назад
            if state.up.pressed then
                ship:set_forward_acceleration(100)
            elseif state.down.pressed then
                ship:set_forward_acceleration(-100)
            else
                ship:set_forward_acceleration(0)
            end

            -- Поворот корабля
            if state.left.pressed then
                ship:set_angular_speed(-180)
            elseif state.right.pressed then
                ship:set_angular_speed(180)
            else
                ship:set_angular_speed(0)
            end

            -- Стрільба
            if state.a.just_pressed then
                local dir_x, dir_y = math.rotate(1, 0, ship.rotation)
                local bullet = Bullet:new(ship.x + dir_x * ship.width / 2, ship.y + dir_y * ship.height / 2)
                bullet.speed_x = ship.speed_x / 2 + display.width * dir_x
                bullet.speed_y = ship.speed_y / 2 + display.width * dir_y
                table.insert(bullets, bullet)
            end

            -- Вихід з гри
            if state.start.just_pressed then
                util.exit()
            end
        end

        -- Перевіряємо, чи потрібно додати новий астероїд
        -- Якщо минув визначений час і якщо на екрані менше 8 астероїдів, то додаємо новий
        if next_asteroid_spawn_time < util.time() and #asteroids < 8 then
            local x, y
            -- Вибираємо випадкову точку за межами екрану
            if math.random() < 0.5 then
                -- Лівий або правий край екрану
                x = math.random() > 0.5 and -100 or display.width + 100
                y = math.random(0, display.height)
            else
                -- Верхній або нижній край екрану
                x = math.random(0, display.width)
                y = math.random() > 0.5 and -100 or display.height + 100
            end
            -- Вибираємо випадкову точку в межах екрану, куди буде рухатися астероїд
            local target_x = math.random(0, display.width)
            local target_y = math.random(0, display.height)

            -- Обчислюємо вектор швидкості астероїда
            local dir_x = target_x - x
            local dir_y = target_y - y
            dir_x, dir_y = math.norm(dir_x, dir_y)

            -- Додаємо астероїд
            table.insert(asteroids, Asteroid:new(x, y, dir_x * 50, dir_y * 50))
            next_asteroid_spawn_time = util.time() + math.random(1, 3)
        end

        -- Перевіряємо колізії: чи корабель зіштовхнувся з астероїдом або чи куля влучила в астероїд
        for _, asteroid in ipairs(asteroids) do
            -- Пропускаємо мертві астероїди
            if not asteroid.dead then
                -- Перевіряємо кулі
                for _, bullet in ipairs(bullets) do
                    if math.dist(asteroid.x, asteroid.y, bullet.x, bullet.y) < asteroid.sprite.width / 2 then
                        -- Якщо куля влучила в астероїд, то вони обидвоє вмирають
                        asteroid:kill()
                        bullet:kill()
                    end
                end
                if math.dist(asteroid.x, asteroid.y, ship.x, ship.y) < (asteroid.sprite.width / 3 + ship.width / 2) then
                    -- Коли корабель зіштовхується з астероїдом. Якщо корабель не мертвий, то вони обидвоє вмирають
                    -- Якщо гравець не мертвий, то він вмирає і гра переходить в стан GAME_OVER
                    if not ship.dead then
                        asteroid:kill()
                        ship:kill()
                        game_state = STATES.GAME_OVER
                    end
                end
            end
        end
        if game_state == STATES.GAME_OVER then
            -- Якщо гравець мертвий, то він може натиснути кнопку "Старт", щоб почати гру знову
            if state.start.just_pressed then
                game_state = STATES.HELLO
            end
        end
    end
end

--
-- Цикл малювання гри
--
function lilka.draw()
    if game_state == STATES.HELLO then
        -- Виводимо початковий екран
        local banner = BANNER[math.random(1, 4)]
        display.fill_screen(BLACK)
        display.draw_image(
            banner,
            display.width / 2 - banner.width / 2,
            display.height / 2 - banner.height / 2
        )
        display.draw_image(
            PRESS_START,
            display.width / 2 - PRESS_START.width / 2,
            display.height - PRESS_START.height - 32
        )
    else
        -- Малюємо корабель, кулі та астероїди
        display.fill_screen(BLACK)
        ship:draw()
        for _, bullet in ipairs(bullets) do
            bullet:draw()
        end
        for _, asteroid in ipairs(asteroids) do
            asteroid:draw()
        end
        if game_state == STATES.GAME_OVER then
            -- Якщо гравець мертвий, то виводимо повідомлення про кінець гри
            display.draw_image(
                YOU_ARE_DEAD,
                display.width / 2 - YOU_ARE_DEAD.width / 2,
                display.height / 2 - YOU_ARE_DEAD.height / 2
            )
            display.draw_image(
                PRESS_START,
                display.width / 2 - PRESS_START.width / 2,
                display.height - PRESS_START.height - 32
            )
        end
    end
end
