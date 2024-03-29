---@meta

---@class gpio
---@field LOW integer логічне значення ``0``
---@field HIGH integer логічне значення ``1``
---@field INPUT integer режим введення
---@field OUTPUT integer режим виведення
---@field INPUT_PULLUP integer режим введення з підтяжкою вгору
gpio = {}

---Налаштовує режим GPIO-піна.
---@param pin integer номер піна
---@param mode integer режим піна, може бути ``gpio.INPUT``, ``gpio.OUTPUT`` або ``gpio.INPUT_PULLUP``
function gpio.set_mode(pin, mode) end

---Записує цифрове зрачення в GPIO-пін.
---@param pin integer номер піна
---@param value integer цифрове значення, може бути ``gpio.LOW`` або ``gpio.HIGH`` (можна використовувати ``0`` або ``1``)
function gpio.write(pin, value) end

---Читає цифрове значення з GPIO-піна. Воно може бути ``gpio.LOW`` або ``gpio.HIGH`` (можна використовувати ``0`` або ``1`` для порівняння).
---@param pin integer номер піна
---@return integer
function gpio.read(pin) end

---Читає аналогове значення з GPIO-піна. Воно може бути в діапазоні від ``0`` до ``4095`` (включно).
---
---Майте на увазі, що ця функція працює тільки з пінами, які підтримують аналогове введення.
---Детальніше про це - на сторінці :doc:`/manual/extension`.
---@param pin integer номер піна
---@return integer
function gpio.analog_read(pin) end

return gpio
