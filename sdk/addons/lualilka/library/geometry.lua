---@meta

---@class geometry
---Геометричні функції
geometry = {}

---Повертає відстань між точками (x1;y1) та (x2;y2).
---@param x1 number координата x першої точки
---@param y1 number координата y першої точки
---@param x2 number координата x другої точки
---@param y2 number координата y другої точки
---@return number
---@usage
--- print(math.dist(0, 0, 3, 4) -- Виведе 5
--- print(math.dist(0, 0, 1, 1) -- Виведе 1.4142135623731
function geometry.dist(x1, y1, x2, y2) end

---Повертає true, якщо відрізки AB та CD перетинаються.
---@param ax number координата x першої точки відрізка AB
---@param ay number координата y першої точки відрізка AB
---@param bx number координата x другої точки відрізка AB
---@param by number координата y другої точки відрізка AB
---@param cx number координата x першої точки відрізка CD
---@param cy number координата y першої точки відрізка CD
---@param dx number координата x другої точки відрізка CD
---@param dy number координата y другої точки відрізка CD
---@return boolean
function geometry.intersect_lines(ax, ay, bx, by, cx, cy, dx, dy) end

---Повертає true, якщо прямокутник (ax, ay, aw, ah) перетинається з прямокутником (bx, by, bw, bh).
---@param ax number координата x верхнього лівого кута першого прямокутника
---@param ay number координата y верхнього лівого кута першого прямокутника
---@param aw number ширина першого прямокутника
---@param ah number висота першого прямокутника
---@param bx number координата x верхнього лівого кута другого прямокутника
---@param by number координата y верхнього лівого кута другого прямокутника
---@param bw number ширина другого прямокутника
---@param bh number висота другого прямокутника
---@return boolean
function geometry.intersect_aabb(ax, ay, aw, ah, bx, by, bw, bh) end
