local display = require("display")
local controller = require("controller")
local util = require("util")
local resources = require("resources")

print("Printing stuff to console, yay!")

local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))

for i = 10, 1, -1 do
    display.fill_rect(0, 0, display.width, display.height, util.random(0xFFFF))
    display.set_cursor(64, 64)
    display.print("Start in " .. i .. "...")

    x = util.random(240 - 64)
    y = util.random(280 - 64)
    display.draw_bitmap(face, x, y)

    display.render()

    util.delay(250)
end

-- Now, we want to draw random lines & faces really fast directly to display, without any buffering!
display.set_buffered(false)

local key = controller.get_state()
while not key.a.just_pressed do
    local x1 = util.random(240)
    local y1 = util.random(280)
    local x2 = util.random(240)
    local y2 = util.random(280)
    local color = util.random(0xFFFF)
    display.draw_line(x1, y1, x2, y2, color)

    local x = util.random(240 - 64)
    local y = util.random(280 - 64)
    display.draw_bitmap(face, x, y, display.color565(0, 0, 0))

    key = controller.get_state()
end
