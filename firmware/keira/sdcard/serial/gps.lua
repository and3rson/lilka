local lat = 0
local lon = 0
local sats = 0

local buffer = ''

function lilka.init()
    serial.begin(9600);
end

function lilka.update(delta)
    while serial.avaliable() > 0 do
        local byte = serial.read()
        buffer = buffer .. string.char(byte)
        if byte == 13 then
            local index = string.find(buffer, '$G%uGGA')
            if index ~= nil then
                buffer = string.sub(buffer, index, -1)
                index = 0
                for part in string.gmatch(buffer, '([^,]*)') do
                    if index == 2 then
                        lat = part
                    elseif index == 3 then
                        lat = lat .. part
                    elseif index == 4 then
                        lon = part
                    elseif index == 5 then
                        lon = lon .. part
                    elseif index == 7 then
                        sats = part
                    end 
                    index = index + 1
                end
            end
            buffer = ''
        end
        checkExit()
    end
    checkExit()
end

function checkExit() 
    if controller.get_state().a.pressed then
        util.exit()
    end
end

function lilka.draw()
    display.fill_screen(display.color565(0, 0, 0))
    display.set_cursor(0, 64)
    display.print('LAT:', lat)
    display.set_cursor(0, 96)
    display.print('LON:', lon)
    display.set_cursor(0, 128)
    display.print('SATS:', sats)
end
