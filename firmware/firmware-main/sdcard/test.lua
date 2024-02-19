local display = require("display")
local console = require("console")
local controller = require("controller")
local util = require("util")

console.print("Printing stuff to console, yay!")

display.fill_rect(50, 50, 200, 200, 0x159C)
display.draw_line(0, 0, 240, 280, 0xC951)

display.set_cursor(64, 64)
display.print("Hello, World!")
display.set_cursor(64, 128)
display.print("Press A to quit.")

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
