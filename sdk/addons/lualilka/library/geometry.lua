---@meta

---@class geometry
---Геометричні функції
geometry = {}

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
