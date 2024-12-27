local ball_x
local ball_y
local xy_sended = false
local buffer = ''
local buffer_finished = false

function lilka.init()
    ball_x = display.width / 2
    ball_y = display.height / 2

    serial.begin(115200);
end

function lilka.update(delta)
    local dir_x = 0
    local dir_y = 0

    if serial.available() > 0 then
        if buffer_finished then
            buffer = ''
            buffer_finished = false
        end
        local byte = serial.read()
        buffer = buffer .. string.char(byte)
        buffer_finished = (byte == 13)
    end

    local state = controller.get_state()
    if state.up.pressed then
        dir_y = -1
        xy_sended = true
    elseif state.down.pressed then
        dir_y = 1
        xy_sended = true
    end
    if state.left.pressed then
        dir_x = -1
        xy_sended = true
    elseif state.right.pressed then
        dir_x = 1
        xy_sended = true
    end
    if state.a.pressed then
        util.exit()
    end

    ball_x = ball_x + dir_x * 50 * delta
    ball_y = ball_y + dir_y * 50 * delta
end

function lilka.draw()
    display.fill_screen(display.color565(0, 0, 0))
    display.set_cursor(0, 64)
    display.print('X: ', ball_x, ' Y: ', ball_y)
    display.set_cursor(0, 128)
    display.print(buffer)
    if xy_sended then
        serial.print('X: ', ball_x, ' Y: ')
        serial.println(ball_y)
        xy_sended = false
    end
end
