---@meta

---@class transforms
transforms = {}

---Створити нове афінне перетворення.
---
---@return Transform
function transforms.new() end

---@class Transform
Transform = {}

---Масштабувати перетворення по обох осях та повернути нове перетворення.
---
---@param x number масштаб по осі X
---@param y number масштаб по осі Y
---@return Transform
function Transform:scale(x, y) end

---Обернути перетворення на певний кут та повернути нове перетворення.
---
---@param angle number кут повороту (в градусах)
---@return Transform
function Transform:rotate(angle) end

---Помножити перетворення на інше перетворення та повернути результат, не змінюючи поточне перетворення.
---
---@param other Transform інше перетворення
---@return Transform
function Transform:multiply(other) end

---Отримати перетворення, яке є оберненим до поточного.
---
---@return Transform
function Transform:inverse() end

---Застосувати перетворення до точки та повернути нові координати.
---
---@param x number координата X
---@param y number координата Y
---@return number, number
---@usage
--- local transform = transforms.new():scale(2, 2):rotate(45)
--- local x, y = transform:transform(10, 10)
--- print(x, y)
function Transform:vtransform(x, y) end

---Отримати матрицю перетворення.
---
---@return table двовимірна таблиця 2x2
function Transform:get() end

---Встановити матрицю перетворення.
---
---.. warning:: Ми не радимо вам використовувати цей метод, якщо ви не знаєте, що робите. Краще використовуйте методи ``scale`` та ``rotate``.
---
---@param matrix table двовимірна таблиця 2x2
---@usage
--- local transform = transforms.new()
--- transform:set({{1, 0}, {0, 1}})
function Transform:set(matrix) end

return transforms
