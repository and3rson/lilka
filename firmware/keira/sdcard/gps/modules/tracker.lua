--[[
    Tracker
    Автор: Андрій Ситников (drronnie85)
--]]

TILE_WIDTH,     TILE_HEIGHT    = 256,              256
SCREEN_WIDTH,   SCREEN_HEIGHT  = display.width,    display.height
CENTER_X,       CENTER_Y       = SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2
FONT_WIDTH,     FONT_HEIGHT    = 10,               20
FONT_OFFSSET_X, FONT_OFFSSET_Y = 0,                -4
FONT_MARGIN_X,  FONT_MARGIN_Y  = 4,                1

BLACK = display.color565(0, 0, 0)
WHITE = display.color565(255, 255, 255)
RED = display.color565(255, 0, 0)
GREEN = display.color565(0, 255, 0)
YELLOW = display.color565(255, 255, 0)

Tracker = {
    root = '',

    gps = nil,
    latitude = nil,
    longitude = nil,

    zoom = nil,
    zoomChangedAt = nil,
    zoomMin = 10,
    zoomMax = 20,

    latDegPerPx = nil,
    lonDegPerPx = nil,
    positionBase = nil,
    positionGPS = { 
        tileX = nil,
        tileY = nil,
        offsetX = nil,
        offsetY = nil
    },
    positionManual = { 
        tileX = nil,
        tileY = nil,
        offsetX = nil,
        offsetY = nil
    },
    tiles = {},
}

-- Конвертація градусів у радіани
local function toRadians(degrees)
    return degrees * math.pi / 180
end

function latLonToTile(lat, lon, zoom)
    -- Обчислення X та Y координат тайлу
    local lat_rad = toRadians(lat)
    local n = 2 ^ zoom
    local tileX = math.floor((lon + 180) / 360 * n)
    local tileY = math.floor((1 - math.log(math.tan(lat_rad) + 1 / math.cos(lat_rad)) / math.pi) / 2 * n)

    -- Глобальні піксельні координати
    local pixelX = ((lon + 180) / 360 * n * TILE_WIDTH)
    local pixelY = ((1 - math.log(math.tan(lat_rad) + 1 / math.cos(lat_rad)) / math.pi) / 2 * n * TILE_HEIGHT)
    
    -- Обчислення зміщення в межах тайла
    local offsetX = math.floor(pixelX % TILE_WIDTH)
    local offsetY = math.floor(pixelY % TILE_HEIGHT)

    return { 
        tileX = tileX,
        tileY = tileY,
        offsetX = offsetX,
        offsetY = offsetY
    }
end

function degreesPerPixel(lat, zoom)
    -- Кількість тайлів
    local n = 2 ^ zoom
    -- Довгота на піксель
    local deg_lon_per_pixel = 360 / (n * TILE_WIDTH)
    -- Широта на піксель
    local lat_rad = toRadians(lat)
    local deg_lat_per_pixel = 360 / (n * TILE_HEIGHT) * math.cos(lat_rad)

    return deg_lon_per_pixel, deg_lat_per_pixel
end

function draw_text(text, x, y, color, background)
    if (color == nil) then
        color = WHITE
    end
    if (background == nil) then
        background = BLACK
    end

    local textWidth = FONT_WIDTH * string.len(text)
    x = x - textWidth / 2
    y = y - FONT_HEIGHT / 2
    display.fill_rect(x - FONT_MARGIN_X, y - FONT_MARGIN_Y, textWidth + FONT_MARGIN_X * 2, FONT_HEIGHT + FONT_MARGIN_Y * 2, background)
    display.set_text_color(color)
    display.set_cursor(x + FONT_OFFSSET_X, y + FONT_HEIGHT + FONT_OFFSSET_Y)
    display.print(text)
end

function Tracker:new(o)
    o = o or {}
    setmetatable(o, self)
    self.__index = self
    return o
end

function Tracker:update(delta)
    if self.gps ~= nil then
        self.gps:update(delta)
        if self.gps.latitude ~= nil and self.gps.longitude ~= nil then
            self.positionGPS = latLonToTile(self.gps.latitude, self.gps.longitude, self.zoom)
        end
    end

    if self.latitude ~= nil and self.longitude ~= nil then
        local keyboard = controller.get_state()

        -- якщо є координати GPS та натиснуто Start, то переходимо на них
        if keyboard.start.just_pressed and self.gps ~= nil and self.gps.latitude ~= nil and self.gps.longitude ~= nil then
            self.latitude, self.longitude = self.gps.latitude, self.gps.longitude
            self.positionBase = nil
        else
            -- корегуємо zoom (d+, c-)
            local dir_zoom = 0
            if keyboard.c.just_pressed then
                dir_zoom = 1
            elseif keyboard.d.just_pressed then
                dir_zoom = -1
            end
            if dir_zoom ~= 0 then
                self.zoomChangedAt = util.time()
                self.zoom = self.zoom + dir_zoom
                self.zoom = math.clamp(self.zoom, self.zoomMin, self.zoomMax)
                self.positionBase = nil
            end

            -- переміщення по карті (стрілки)
            local dir_lon = 0
            if keyboard.left.pressed then
                dir_lon = -1
            elseif keyboard.right.pressed then
                dir_lon = 1
            end

            local dir_lat = 0
            if keyboard.up.pressed then
                dir_lat = 1
            elseif keyboard.down.pressed then
                dir_lat = -1
            end

            if dir_lat ~= 0 and self.latDegPerPx ~= null then
                self.latitude = self.latitude + dir_lat * self.latDegPerPx * 20 * delta
                self.positionBase = nil
            end
            if dir_lon ~= 0 and self.lonDegPerPx ~= null then
                self.longitude = self.longitude + dir_lon * self.latDegPerPx * 20 * delta
                self.positionBase = nil
            end
        end

        if self.positionBase == nil then
            self.positionManual = latLonToTile(self.latitude, self.longitude, self.zoom)
            self.latDegPerPx, self.lonDegPerPx = degreesPerPixel(self.latitude, self.zoom)
            self:calculateTiles()
        end
    end
end

function Tracker:calculateTiles()
    local firstTileX, firstTileY = self.positionManual.tileX, self.positionManual.tileY

    -- визначення X першого тайла і його позиції
    local firstTilePosX = CENTER_X - self.positionManual.offsetX
    while firstTilePosX > 0 do
        firstTilePosX = firstTilePosX - TILE_WIDTH
        firstTileX = firstTileX - 1
    end

    -- визначення Y першого тайла і його позиції
    local firstTilePosY = CENTER_Y - self.positionManual.offsetY
    while firstTilePosY > 0 do
        firstTilePosY = firstTilePosY - TILE_HEIGHT
        firstTileY = firstTileY - 1
    end
    self.positionBase = {
        tileX = firstTileX,
        tileY = firstTileY,
        offsetX = firstTilePosX,
        offsetY = firstTilePosY
    }

    -- визначення кількості тайлів по X
    local tilesX = 0
    while firstTilePosX < SCREEN_WIDTH do
        firstTilePosX = firstTilePosX + TILE_WIDTH
        tilesX = tilesX + 1
    end
    self.positionBase.tilesX = tilesX

    -- визначення кількості тайлів по Y
    local tilesY = 0
    while firstTilePosY < SCREEN_HEIGHT do
        firstTilePosY = firstTilePosY + TILE_HEIGHT
        tilesY = tilesY + 1
    end
    self.positionBase.tilesY = tilesY

    -- формування переліку потірбних тайлів
    local tiles = {}
    for x = 0, tilesX - 1 do
        for y = 0, tilesY - 1 do
            local key = string.format("%d/%d/%d", self.zoom, firstTileX + x, firstTileY + y) 
            tiles[key] = 1
        end
    end

    -- видалення зайвих тайлів
    for key, value in pairs(self.tiles) do
        if tiles[key] == nil then
            self.tiles[key] = nil
        end
    end
    
    -- загрузка тайлів
    for key, value in pairs(tiles) do
        if self.tiles[key] == nil then
            -- завантаження тайлу
            local path = string.format("tiles/%s.png", key)
            console.print("Loading file " .. path .. "...")
            -- draw_text(string.format("L %s.png", key), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, GREEN)
            -- display.queue_draw()

            local success, tile = pcall(resources.load_image, path, BLACK)
            if success then
                self.tiles[key] = tile
            else
                -- якщо тайлу немає, то завантажуємо його з сервера
                local url = string.format("https://a.tile.openstreetmap.org/%s.png", key)
                console.print("Downloading " .. url .. " to " .. path .. "...")
                draw_text(string.format("DL %s.png", key), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, YELLOW)
                display.queue_draw()

                buzzer.play(100, 100)
                util.sleep(0.1)

                local request = {
                    url = url,
                    file = self.root .. "/" .. path
                }
                local response = http.execute(request)
                console.print("Result:", response.code)
                self.tiles[key] = resources.load_image(path, BLACK)
            end
        end
    end
end

function Tracker:draw()
    display.fill_screen(BLACK)
    if self.positionBase ~= nil then
        -- малюємо тайли
        local firstTileX, firstTileY = self.positionBase.tileX, self.positionBase.tileY
        for x = 0, self.positionBase.tilesX - 1 do
            for y = 0, self.positionBase.tilesY - 1 do
                local tileX, tileY = self.positionBase.tileX + x, self.positionBase.tileY + y
                local offsetX, offsetY = self.positionBase.offsetX + x * TILE_WIDTH, self.positionBase.offsetY + y * TILE_HEIGHT
                local key = string.format("%d/%d/%d", self.zoom, tileX, tileY) 
                if self.tiles[key] ~= nil then
                    display.draw_image(self.tiles[key], offsetX, offsetY)

                    if tileX == self.positionGPS.tileX and tileY == self.positionGPS.tileY then
                        self.positionGPS.x = offsetX + self.positionGPS.offsetX
                        self.positionGPS.y = offsetY + self.positionGPS.offsetY
                    end

                    if tileX == self.positionManual.tileX and tileY == self.positionManual.tileY then
                        self.positionManual.x = offsetX + self.positionManual.offsetX
                        self.positionManual.y = offsetY + self.positionManual.offsetY
                    end
                end
            end
        end

        -- малюємо курсори
        local x, y = self.positionGPS.x, self.positionGPS.y
        if x ~= nil and y ~= nil then
            display.draw_line(x - FONT_HEIGHT / 2, y, x + FONT_HEIGHT / 2, y, BLACK)
            display.draw_line(x, y - FONT_HEIGHT / 2, x, y + FONT_HEIGHT / 2, BLACK)
        end
        x, y = self.positionManual.x, self.positionManual.y
        if x ~= nil and y ~= nil then
            display.draw_line(x - FONT_HEIGHT / 2, y, x + FONT_HEIGHT / 2, y, RED)
            display.draw_line(x, y - FONT_HEIGHT / 2, x, y + FONT_HEIGHT / 2, RED)
        end
    end

    -- малюємо координати
    draw_text(string.format("%.5f  %.5f", self.latitude, self.longitude), SCREEN_WIDTH / 2, FONT_HEIGHT / 2, WHITE)
    if self.gps.latitude ~= nil and self.gps.longitude ~= nil then
        draw_text(string.format("%.5f  %.5f  %d", self.gps.latitude, self.gps.longitude, self.gps.sats), SCREEN_WIDTH / 2, SCREEN_HEIGHT - FONT_HEIGHT / 2, YELLOW)
    end

    -- малюємо zoom
    if self.zoomChangedAt ~= nil and util.time() - self.zoomChangedAt < 5 then
        if self.zoomMax > self.zoom then
            draw_text("C", SCREEN_WIDTH - FONT_WIDTH, SCREEN_HEIGHT / 2 - FONT_HEIGHT * 2, WHITE)
            draw_text(string.format("%d", self.zoom + 1), SCREEN_WIDTH - FONT_WIDTH, SCREEN_HEIGHT / 2 - FONT_HEIGHT, WHITE)
        end
        if self.zoomMin < self.zoom then
            draw_text(string.format("%d", self.zoom - 1), SCREEN_WIDTH - FONT_WIDTH, SCREEN_HEIGHT / 2 + FONT_HEIGHT, WHITE)
            draw_text("D", SCREEN_WIDTH - FONT_WIDTH, SCREEN_HEIGHT / 2 + FONT_HEIGHT * 2, WHITE)
        end
        draw_text(string.format("%d", self.zoom), SCREEN_WIDTH - FONT_WIDTH, SCREEN_HEIGHT / 2, GREEN)
    end
end

return Tracker