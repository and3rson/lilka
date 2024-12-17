local sys = 'N'
local lat = 0
local lat_dir = ''
local lon = 0
local lon_dir = ''
local sats = 0

local buffer = ''
local raw = ''

function lilka.init()
    serial.begin(9600);
end

function lilka.update(delta)
    while serial.avaliable() > 0 do
        buffer = buffer .. serial.read(serial.avaliable())

        local start, len, lsys, llat, llat_dir, llon, llon_dir, lsats = string.find(buffer,'$G(%u)GGA,[%d%.]*,([%d%.]*),(%u?),([%d%.]*),(%u?),%d*,(%d+).+%*')
        if start ~= nil then
            sys = lsys
            lat = llat
            lat_dir = llat_dir
            lon = llon
            lon_dir = llon_dir
            sats = lsats
            raw = string.sub(buffer, start, len)
            buffer = ''
        end
        checkExit()
    end
    checkExit()
end

function checkExit() 
    if controller.get_state().b.pressed then
        util.exit()
    end
end

function lilka.draw()
    display.fill_screen(display.color565(0, 0, 0))
    local y = 64
    display.set_cursor(0, y + 00)
    display.print('LAT:', lat)
    display.set_cursor(0, y + 20)
    display.print('LON:', lon)
    display.set_cursor(0, y + 40)
    display.print('SATS:', sats)
    display.set_cursor(0, y + 60)
    display.print('RAW:', raw)
    display.set_cursor(0, y + 120)
    display.print('Press B to exit...')
end
