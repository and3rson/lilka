--[[
    RF
    Автор: Андрій Ситников (drronnie85)
    Опис: Додаток для керування модулем розширення lilRF
--]]

Stack = require("libs.Stack")
Menu = require("libs.Menu")
Alert = require("libs.Alert")

uistack = Stack:new()
menu = nil

function lilka.init()
    -- local alert = Alert:new("Keira OS", "by Андерсон & friends")
    -- uistack:push(alert)

    menu = Menu:new({
        title = "lilRF",
        items = {
            {
                title = "Коди",
                action = codesMenu
            },
            {
                title = "Інформація",
                items = {
                    {
                        title = "Про модуль...",
                        action = aboutModule
                    },
                    {
                        title = "Про програму...",
                        action = about
                    },
                }
            },
            {
                title = "Вихід",
                action = exit
            }
        }
    })
    uistack:push(menu)
end

function lilka.update(delta)
    local item = uistack:peek()
    if (item ~= nil and type(item.update) == "function") then
        item:update(delta)
    end

    if controller.get_state().b.pressed then
        util.exit()
    end
end

function lilka.draw()
    for i, item in pairs(uistack) do
        if (item ~= nil and type(item.draw) == "function") then
            item:draw()
        end
    end
end

function codesMenu() 
end

function aboutModule() 
end

function about() 
end

function exit()
    util.exit()
end