local ball_x
local ball_y

local ball = resources.load_image("ball.bmp", display.color565(255, 255, 255))

function lilka.init()
    -- Ця функція викликається один раз при запуску програми.
    -- Цей код можна було б виконати в глобальному контексті (поза цією функцією), як ми це зробили з "ball",
    -- але ініціалізація гри буде очевиднішою, якщо вона відбувається тут.
    ball_x = display.width / 2
    ball_y = display.height / 2
end

function lilka.update(delta)
    local dir_x = 0
    local dir_y = 0

    -- Обробляємо введення користувача:
    local state = controller.get_state()
    if state.up.pressed then
        dir_y = -1
    elseif state.down.pressed then
        dir_y = 1
    end
    if state.left.pressed then
        dir_x = -1
    elseif state.right.pressed then
        dir_x = 1
    end
    if state.a.pressed then
        -- Вихід з програми:
        util.exit()
    end

    -- Переміщуємо м'яч зі швидкістю 50 пікселів на секунду
    ball_x = ball_x + dir_x * 50 * delta
    ball_y = ball_y + dir_y * 50 * delta
end

function lilka.draw()
    -- Малюємо графіку:
    display.fill_screen(display.color565(0, 0, 0))
    display.draw_image(ball, ball_x, ball_y)

    -- Після виконання цієї функції Лілка автоматично відобразить все, що ми намалювали на екрані.
end
