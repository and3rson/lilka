local ball_x = display.width / 2
local ball_y = display.height / 2

local ball = resources.load_bitmap("ball.bmp", display.color565(255, 255, 255))

function _update()
    local ball_speed_x = 0
    local ball_speed_y = 0

    -- Обробляємо введення користувача:
    if controller.up.pressed then
        ball_speed_y = -4
    elseif controller.down.pressed then
        ball_speed_y = 4
    end
    if controller.left.pressed then
        ball_speed_x = -4
    elseif controller.right.pressed then
        ball_speed_x = 4
    end
    if controller.a.pressed then
        -- Вихід з програми:
        util.exit()
    end

    -- Оновлюємо стан гри:
    ball_x = ball_x + ball_speed_x
    ball_y = ball_y + ball_speed_y

    -- Малюємо графіку:
    display.fill_screen(display.color565(0, 0, 0))
    display.draw_bitmap(ball, ball_x, ball_y)

    -- Оновлюємо екран:
    display.render()
end
