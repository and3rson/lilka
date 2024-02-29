Player = {
    x = 2,
    y = 3,
}

function Player:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Player:move(dx, dy)
    self.x = self.x + dx
    self.y = self.y + dy
end

-- local player = Player:new({})

Player:move(1, 1)

-- print(player.x)
-- player:move(1, 1)
