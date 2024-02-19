local display = require("display")
local console = require("console")
local controller = require("controller")
local util = require("util")

console.print("Printing stuff to console, yay!")

for i = 10, 1, -1 do
    display.fill_rect(0, 0, display.width, display.height, util.random(0xFFFF))
    display.set_cursor(64, 64)
    display.print("Start in " .. i .. "...")
    display.render()
    util.delay(250)
end

-- Now, we draw lines really fast directly to display!
display.set_buffered(false)

local key = controller.get_state()
-- Loop while key.a.just_pressed is false
while not key.a.just_pressed do
    key = controller.get_state()
    local x1 = util.random(240)
    local y1 = util.random(280)
    local x2 = util.random(240)
    local y2 = util.random(280)
    local color = util.random(0xFFFF)
    display.draw_line(x1, y1, x2, y2, color)
end
