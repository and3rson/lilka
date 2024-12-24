--[[
    GPS
    Автор: Андрій Ситников (drronnie85)
    Опис: Клас для роботи з GPS модулем (NMEA 0183)
          Протестовано на модулі BN-220 через UART
--]]

GPS = {
    system = 'N',
    latitude = nil,
    longitude = nil,
    fixAt = nil,
    sats = 0,

    buffer = "",
    command = ""
}

function GPS:new(obj)
    obj = obj or {}
    setmetatable(obj, self)
    self.__index = self

    serial.begin(9600)

    return obj
end

function GPS:update(delta)
    while serial.available() > 0 do
        self.buffer = self.buffer .. serial.read(serial.available())

        local start, len, sys, time, lat, lat_dir, lon, lon_dir, sats = string.find(self.buffer,"$G(%u)GGA,([%d%.]*),([%d%.]*),(%u?),([%d%.]*),(%u?),%d*,(%d+).+%*")
        if start ~= nil then
            command = string.sub(self.buffer, start, len)
            self.buffer = ''
            
            -- система, з якої отримані координати
            self.system = sys

            -- широта
            local _, _, d, m =  string.find(lat, "(%d%d)(%d%d%.%d%d%d%d%d)")
            if d == nil or m == nil then
                self.latitude = nil
            else
                self.latitude = (tonumber(d) + tonumber(m) / 60) * (lat_dir == "S" and -1 or 1)
            end

            -- довгота
            local _, _, d, m =  string.find(lon, "(%d%d%d)(%d%d%.%d%d%d%d%d)")
            if d == nil or m == nil then
                self.longitude = nil
            else
                self.longitude = (tonumber(d) + tonumber(m) / 60) * (lon_dir == "W" and -1 or 1)
            end

            -- кількість супутників
            self.sats = tonumber(sats);

            if (self.latitude ~= nil and self.longitude ~= nil) then
                self.fixAt = util.time()
            end
        end
    end
end

return GPS