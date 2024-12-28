---@class keyboardUI
keyboardUI = {}

---Створює новий діалог для введення тексту через клавіатуру.
---
---@param prompt string Повідомлення, яке відображатиметься в діалозі.
---@return keyboardUI новий об'єкт діалогу
function keyboardUI(prompt) end

---Оновлює стан діалогу. Зазвичай викликається в циклі для оновлення діалогу.
function keyboardUI:update() end

---Малює діалог на екрані.
function keyboardUI:draw() end

---Перевіряє, чи завершено діалог.
---@return boolean true, якщо діалог завершено, false, якщо ні
function keyboardUI:isFinished() end

---Встановлює, чи слід маскувати введений текст (наприклад, для введення паролю).
---@param masked boolean true, якщо текст має бути маскований, false — якщо ні
function keyboardUI:setMasked(masked) end

---Встановлює значення для поля введення.
---@param value string нове значення поля введення
function keyboardUI:setValue(value) end

---Отримує значення, введене в діалозі.
---@return string значення введеного тексту
function keyboardUI:getValue() end


---@class alertUI
alertUI = {}

---Створює діалог з повідомленням для сповіщення користувача.
---
---@param title string Назва діалогу
---@param message string Повідомлення, яке відображатиметься в діалозі
---@return alertUI новий об'єкт діалогу
function alertUI(title, message) end

---Оновлює стан діалогу.
function alertUI:update() end

---Малює діалог на екрані.
function alertUI:draw() end

---Перевіряє, чи завершено діалог.
---@return boolean true, якщо діалог завершено, false — якщо ні
function alertUI:isFinished() end

---Встановлює нову назву для діалогу.
---@param title string нова назва діалогу
function alertUI:setTitle(title) end

---Встановлює нове повідомлення для діалогу.
---@param message string нове повідомлення для діалогу
function alertUI:setMessage(message) end

---Додає кнопку для активації (наприклад, 'a', 'b' тощо).
---@param button string кнопка для активації
function alertUI:addActivationButton(button) end

---Отримує кнопку, яку натиснув користувач.
---@return string ім'я натиснутої кнопки (наприклад, 'a', 'b')
function alertUI:getButton() end


---@class progressUI
progressUI = {}

---Створює діалог для відображення прогресу.
---
---@param title string Назва діалогу
---@param message string Повідомлення, яке відображатиметься в діалозі
---@return progressUI новий об'єкт діалогу
function progressUI(title, message) end

---Малює діалог на екрані.
function progressUI:draw() end

---Встановлює нове повідомлення для діалогу прогресу.
---@param message string нове повідомлення для діалогу
function progressUI:setMessage(message) end

---Встановлює поточний прогрес (від 0 до 100) для прогрес-бару.
---@param value integer поточний прогрес (0-100)
function progressUI:setProgress(value) end


--------------------------
-- Приклад використання --
--------------------------

-- Створення діалогу для введення тексту
local keyboard_test = keyboardUI("Скажи моє ім'я") 
keyboard_test:setValue("ТЕСТ")
while true do      
    keyboard_test:draw()      
    keyboard_test:update()  
    if keyboard_test:isFinished() then
        break
    end
end

-- Створення сповіщення
local alert_test = alertUI("Хто ти?", keyboard_test:getValue()) 
alert_test:draw()      
while true do      
    alert_test:update()     
    if alert_test:isFinished() then
        break
    end
end

alert_test:setTitle("НІ!!! Ти є:")
alert_test:setMessage("КРАЩИЙ У СВІТІ")

-- Створення діалогу з кнопками
local alert_test = alertUI("Бажаєш пограти?", "[A]ТАК\n[B]НІ") 
alert_test:addActivationButton("a")
alert_test:addActivationButton("b")
alert_test:draw()      
while true do   
    alert_test:update()     
    if alert_test:isFinished() then
        break
    end
end

which_key = alert_test:getButton()

if which_key == "a" then
    alert_test:setTitle("ГАРАЗД")
    alert_test:setMessage("ПОГРАЙМО")
elseif which_key == "b" then
    alert_test:setTitle("Добре")
    alert_test:setMessage("...")
end

-- Створення діалогу прогресу
local progress_test = progressUI("Ракета", "Запуск") 
for i = 0, 100 do
    progress_test:setProgress(i)
    progress_test:draw()
end
