--[[
    GPS Tracker
    Автор: Андрій Ситников (drronnie85)
    Опис: Модуль для відображення мапи з GPS координатами
    Керування: 
        Стрілки - переміщення
        C, D    - маштабування
        B       - вихід
        Start   - перейти на координати з GPS (при наявності)
--]]

GPS = require("libs.GPS")
Tracker = require("modules.Tracker")

tracker = nil

function lilka.init()
    state = state or {
        latitude = 50.44953,
        longitude = 30.52537,
        zoom = 13,
    }
    
    tracker = Tracker:new({
        root = "/gps",
        gps = GPS:new(),
        zoom = state.zoom,
        latitude = state.latitude,
        longitude = state.longitude,
        inited = state.latitude ~= nil and state.longitude ~= nil
    })
end

function lilka.update(delta)
    tracker:update(delta)
    
    state.latitude = tracker.latitude
    state.longitude = tracker.longitude
    state.zoom = tracker.zoom

    if controller.get_state().b.pressed then
        util.exit()
    end
end

function lilka.draw()
    tracker:draw()
end