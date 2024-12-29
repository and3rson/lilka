--[[
    Stack
    Автор: Андрій Ситников (drronnie85)
    Опис: Клас стек (LIFO)
--]]

Stack = {}

function Stack:new()
    obj = {}
    setmetatable(obj, self)
    self.__index = self
    return obj
end

-- Додавання елемента в стек (push)
function Stack:push(item)
    table.insert(self, item) -- Додає елемент в кінець таблиці
end

-- Видалення елемента зі стеку (pop)
function Stack:pop()
    if #self == 0 then
        return nil -- Якщо стек порожній, повертає nil
    end
    return table.remove(self) -- Видаляє останній елемент і повертає його
end

-- Перегляд верхнього елемента без видалення (peek)
function Stack:peek()
    if #self == 0 then
        return nil -- Якщо стек порожній, повертає nil
    end
    return self[#self] -- Повертає останній елемент без його видалення
end

return Stack

