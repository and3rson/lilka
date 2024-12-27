---@meta

---@class sdcard
sdcard = {}

---Повернути таблицю зі списком файлів та директорій за вказаним шляхом.
---
---@param path string шлях до директорії (відносно кореня SD-картки)
---@return table
---@usage
--- local entries = sdcard.ls("/folder")
--- for i = 0, #entries do
---     print(entries[i])
--- end
function sdcard.ls(path) end

---Видалити файл або директорію за вказаним шляхом.
---
---@param path string шлях до файлу або директорії (відносно кореня SD-картки)
---@usage
--- sdcard.remove("/folder/file.txt")
function sdcard.remove(path) end

---Перейменувати файл або директорію.
---
---@param old_path string старий шлях до файлу або директорії (відносно кореня SD-картки)
---@param new_path string новий шлях до файлу або директорії (відносно кореня SD-картки)
---
---@usage
--- sdcard.rename("/folder/file.txt", "/folder/file2.txt")
function sdcard.rename(old_path, new_path) end

---@class File
File = {}

---Відкрити файл за вказаним шляхом.
---
---@param path string шлях до файлу (відносно кореня SD-картки)
---@param mode string режим відкриття файлу (за замовчуванням r) (див. функцію ``fopen`` у документації ANSI C)
---@return File
---
---@usage
--- local file = sdcard.open("/file.txt", "a+") -- Відкриває файл для додавання тексту
--- file:write("Hello, world!\n") -- Дописує текст в кінець файлу
function sdcard.open(path, mode) end

---Повернути розмір файлу.
---
---@return integer
function File:size() end

---Повернути наявність файлу.
---
---@return boolean
function File:exists() end

---Перемістити вказівник файлу на певну позицію.
---
---@param pos integer позиція в файлі
function File:seek(pos) end

---Прочитати з файлу.
---
---@param count integer максимальна кількість байт, які потрібно прочитати
---@return string
function File:read(count) end

---Записати у файл.
---
---@param content string дані, які потрібно записати
---@usage
--- local file = sdcard.open("/file.txt", "w") -- Відкриває файл для запису
--- file:write("Hello, world!\n") -- Записує текст у файл
--- file:write("Привіт, світ!\n") -- Дописує текст у файл
function File:write(content) end

return sdcard
