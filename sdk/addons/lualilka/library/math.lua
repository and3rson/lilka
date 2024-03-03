---@meta

---@class math
---@field pi number Число π
---@field e number Число e
---@field tau number Число τ (2π)
math = {}

---Повертає випадкове число.
---
---* Якщо заданий лише один аргумент, повертає випадкове ЦІЛЕ число в діапазоні [0;a] (включно).
---* Якщо задані обидва аргументи, повертає випадкове ЦІЛЕ число в діапазоні [a;b] (включно).
---* Якщо не задані аргументи, повертає випадкове ДРОБОВЕ число в діапазоні [0;1] (включно).
---@param a? integer початок діапазону (включно)
---@param b? integer кінець діапазону (включно)
---@return number
---@usage
--- local r = math.random(10, 20)
--- print(r) -- Виведе випадкове число від 10 до 20 (включно). Можливо, це буде 13? А можливо, русня - не люди?
function math.random(a, b) end

---Обмежує число x в діапазоні між min та max (включно).
---
---@param x number число, яке потрібно обмежити
---@param min number мінімальне значення
---@param max number максимальне значення
---@return number
---@usage
--- print(math.clamp(8.1, 10, 20)) -- Виведе 10
--- print(math.clamp(15.2, 10, 20)) -- Виведе 15.2
--- print(math.clamp(23.3, 10, 20)) -- Виведе 20
function math.clamp(x, min, max) end

---Лінійна інтерполяція.
---
---Повертає значення, яке лінійно інтерполюється між min та max з коефіцієнтом t.
---@param min number мінімальне значення
---@param max number максимальне значення
---@param t number коефіцієнт інтерполяції (від 0 до 1)
---@return number
---@usage
--- print(math.lerp(0, 100, 0.5)) -- Виведе 50
--- print(math.lerp(0, 100, 0.25)) -- Виведе 25
--- print(math.lerp(0, 100, 0.7125)) -- Виведе 71.25
function math.lerp(min, max, t) end

---Перетворення значення з одного діапазону в інший.
---
---Повертає число ``x``, перетворене з діапазону [in_min;in_max] в діапазон [out_min;out_max].
---@param x number число, яке потрібно перетворити
---@param in_min number початок вхідного діапазону
---@param in_max number кінець вхідного діапазону
---@param out_min number початок вихідного діапазону
---@param out_max number кінець вихідного діапазону
---@return number
---@usage
--- print(math.map(50, 0, 100, 0, 1)) -- Виведе 0.5
--- print(math.map(25, 0, 100, 0, 1)) -- Виведе 0.25
--- print(math.map(71.25, 0, 100, 0, 1)) -- Виведе 0.7125
function math.map(x, in_min, in_max, out_min, out_max) end

---Повертає модуль числа ``x``.
---@param x number число
---@return number
function math.abs(x) end

---Повертає знак числа ``x``: -1, якщо число від'ємне, 0, якщо число дорівнює 0, 1, якщо число додатнє.
---@param x number число
---@return number
---@usage
--- print(math.sign(-5)) -- Виведе -1
--- print(math.sign(5)) -- Виведе 1
--- print(math.sign(0)) -- Виведе 0
function math.sign(x) end

---Повертає квадратний корінь числа ``x``.
---@param x number число
---@return number
function math.sqrt(x) end

---Повертає число ``x`` в степені ``exp``.
---@param base number число
---@param exp number степінь
---@return number
---@usage
--- print(math.pow(2, 3)) -- Виведе 8
--- print(math.pow(64, 0.5)) -- Виведе 8
function math.pow(base, exp) end

---Повертає мінімальне значення з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.min({1.1, 2.2, 3.3, 4.4, 5.5})) -- Виведе 1.1
--- print(math.min({5, 4, 3, 2, 1})) -- Виведе 1
--- print(math.min({-5, -4, -3, -2, -1})) -- Виведе -5
function math.min(values) end

---Повертає максимальне значення з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.max({1.1, 2.2, 3.3, 4.4, 5.5})) -- Виведе 5.5
--- print(math.max({5, 4, 3, 2, 1})) -- Виведе 5
--- print(math.max({-5, -4, -3, -2, -1})) -- Виведе -1
function math.max(values) end

---Повертає суму всіх чисел з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.sum({1, 2, 3, 4, 5})) -- Виведе 15
--- print(math.sum({1.1, 2.2, 3.3, 4.4, 5.5})) -- Виведе 16.5
--- print(math.sum({-5, -4, -3, -2, -1})) -- Виведе -15
function math.sum(values) end

---Повертає середнє значення всіх чисел з таблиці.
---@param values table таблиця чисел
---@return number
---@usage
--- print(math.avg({1, 2, 3, 4, 5})) -- Виведе 3
--- print(math.avg({1.1, 2.2, 3.3, 4.4})) -- Виведе 2.75
--- print(math.avg({-5, -4, -3, -2})) -- Виведе -3.5
function math.avg(values) end

---Округлює число ``x`` вниз.
---
---Повертає найбільше ціле число, яке менше або дорівнює ``x``.
---@param x number число
---@return integer
---@usage
--- print(math.floor(1.1)) -- Виведе 1
--- print(math.floor(1.9)) -- Виведе 1
--- print(math.floor(-1.1)) -- Виведе -2
--- print(math.floor(-1.9)) -- Виведе -2
function math.floor(x) end

---Округлює число ``x`` вгору.
---
---Повертає найменше ціле число, яке більше або дорівнює ``x``.
---@param x number число
---@return integer
---@usage
--- print(math.ceil(1.1)) -- Виведе 2
--- print(math.ceil(1.9)) -- Виведе 2
--- print(math.ceil(-1.1)) -- Виведе -1
--- print(math.ceil(-1.9)) -- Виведе -1
function math.ceil(x) end

---Округлює число ``x`` до найближчого цілого.
---@param x number число
---@return integer
---@usage
--- print(math.round(1.1)) -- Виведе 1
--- print(math.round(1.9)) -- Виведе 2
--- print(math.round(-1.1)) -- Виведе -1
--- print(math.round(-1.9)) -- Виведе -2
function math.round(x) end

---Повертає значення синуса кута ``x`` (в радіанах).
---@param x number кут (в радіанах)
---@return number
---@usage
--- print(math.sin(0)) -- Виведе 0
--- print(math.sin(math.pi / 2)) -- Виведе 1
function math.sin(x) end

---Повертає значення косинуса кута ``x`` (в радіанах).
---@param x number кут (в радіанах)
---@return number
---@usage
--- print(math.cos(0)) -- Виведе 1
--- print(math.cos(math.pi)) -- Виведе -1
function math.cos(x) end

---Повертає значення тангенса кута ``x`` (в радіанах).
---@param x number кут (в радіанах)
---@return number
---@usage
--- print(math.tan(0)) -- Виведе 0
--- print(math.tan(math.pi / 4)) -- Виведе 1
--- print(math.tan(math.pi / 2)) -- Виведе якесь дуже велике число, оскільки тангенс не визначений для кутів, що дорівнюють π/2.
---                              -- Чому? Подумайте самі: трикутник з кутом 180 градусів - це взагалі трикутник?
function math.tan(x) end

---Повертає значення арксинуса числа ``x``.
---@param x number число
---@return number
function math.asin(x) end

---Повертає значення арккосинуса числа ``x``.
---@param x number число
---@return number
function math.acos(x) end

---Повертає значення арктангенса числа ``x``.
---@param x number число
---@return number
function math.atan(x) end

---Повертає значення арктангенса числа ``y``/``x``.
---
---Цей метод дуже зручно використовувати для обчислення кута між віссю x та точкою (x, y).
---А взагалі, якщо ви розумієте, що таке арктангенс, то, можливо, вам варто спробувати писати код на C? :)
---@param y number число y
---@param x number число x
function math.atan2(y, x) end

---Повертає значення логарифму числа ``x``.
---@param x number число
---@param base? number основа логарифму (за замовчуванням - число ``e``)
---@return number
function math.log(x, base) end

---Перетворює кут ``x`` з радіанів в градуси.
---@param x number кут (в радіанах)
---@return number
function math.deg(x) end

---Перетворює кут ``x`` з градусів в радіани.
---@param x number кут (в градусах)
---@return number
function math.rad(x) end

---Нормалізує вектор (x, y) до одиничної довжини.
---@param x number координата x
---@param y number координата y
---@return number, number
function math.norm(x, y) end

---Повертає довжину вектора (x, y).
---@param x number координата x
---@param y number координата y
---@return number
---@usage
--- print(math.len(3, 4)) -- Виведе 5
--- print(math.len(1, 1)) -- Виведе 1.4142135623731
function math.len(x, y) end

---Повертає відстань між точками (x1;y1) та (x2;y2).
---@param x1 number координата x першої точки
---@param y1 number координата y першої точки
---@param x2 number координата x другої точки
---@param y2 number координата y другої точки
---@return number
---@usage
--- print(math.dist(0, 0, 3, 4) -- Виведе 5
--- print(math.dist(0, 0, 1, 1) -- Виведе 1.4142135623731
function math.dist(x1, y1, x2, y2) end

---Повертає вектор (x, y), обернутий на кут ``angle`` **за годинниковою стрілкою** (якщо уявити, що вісь Y вказує вниз, як це прийнято в комп'ютерній графіці).
---@param x number координата x
---@param y number координата y
---@param angle number кут (в градусах)
---@return number, number
---@usage
--- local x, y = math.rotate(1, 0, 45)
--- print(x, y) -- Виведе 0, 1
function math.rotate(x, y, angle) end

return math
