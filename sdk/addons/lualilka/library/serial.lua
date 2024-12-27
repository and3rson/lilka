---@meta

---@class serial
serial = {}

---Перевіряє кількість доступних байтів для читання.
---@return integer
---@usage
--- local bytesAvailable = serial.available()
--- print("Available bytes:", bytesAvailable)
function serial.available() end

---Ініціалізує послідовний порт із заданими параметрами.
---@param baud integer швидкість передачі даних (за замовчуванням 115200)
---@param config integer конфігурація порту (за замовчуванням SERIAL_8N1)
---@usage
--- serial.begin(9600, serial.SERIAL_8N1)
function serial.begin(baud, config) end

---Зупиняє послідовний порт.
---@usage
--- serial.stop()
function serial.stop() end

---Очікує завершення передачі всіх даних.
---@usage
--- serial.flush()
function serial.flush() end

---Читає наступний байт із буфера без його видалення.
---@return integer
---@usage
--- local nextByte = serial.peek()
--- print("Next byte:", nextByte)
function serial.peek() end

---Пише дані у послідовний порт.
---@param ... any значення для виведення
---@usage
--- serial.print("Hello, ")
--- serial.print(123)
function serial.print(...) end

---Пише дані у послідовний порт з переходом на новий рядок.
---@param ... any значення для виведення
---@usage
--- serial.println("Привіт, світ!")
function serial.println(...) end

---Читає дані з послідовного порту.
---@param count integer максимальна кількість байтів для читання (опціонально)
---@return string|integer якщо `count` задано, повертає рядок; якщо ні — окремий байт
---@usage
--- local data = serial.read(10)
--- print("Data:", data)
function serial.read(count) end

---Встановлює таймаут для операцій читання/запису.
---@param timeout integer час у мілісекундах
---@usage
--- serial.setTimeout(1000)
function serial.setTimeout(timeout) end

---Записує дані в послідовний порт.
---@param ... any дані для запису (рядки, числа тощо)
---@usage
--- serial.write("Data")
--- serial.write(255)
function serial.write(...) end

---Константи конфігурації для послідовного порту:
---@field SERIAL_5N1 integer
---@field SERIAL_6N1 integer
---@field SERIAL_7N1 integer
---@field SERIAL_8N1 integer
---@field SERIAL_5N2 integer
---@field SERIAL_6N2 integer
---@field SERIAL_7N2 integer
---@field SERIAL_8N2 integer
---@field SERIAL_5E1 integer
---@field SERIAL_6E1 integer
---@field SERIAL_7E1 integer
---@field SERIAL_8E1 integer
---@field SERIAL_5E2 integer
---@field SERIAL_6E2 integer
---@field SERIAL_7E2 integer
---@field SERIAL_8E2 integer
---@field SERIAL_5O1 integer
---@field SERIAL_6O1 integer
---@field SERIAL_7O1 integer
---@field SERIAL_8O1 integer
---@field SERIAL_5O2 integer
---@field SERIAL_6O2 integer
---@field SERIAL_7O2 integer
---@field SERIAL_8O2 integer
serial.SERIAL_5N1 = 0x8000010
serial.SERIAL_6N1 = 0x8000014
serial.SERIAL_7N1 = 0x8000018
serial.SERIAL_8N1 = 0x800001c
serial.SERIAL_5N2 = 0x8000030
serial.SERIAL_6N2 = 0x8000034
serial.SERIAL_7N2 = 0x8000038
serial.SERIAL_8N2 = 0x800003c
serial.SERIAL_5E1 = 0x8000012
serial.SERIAL_6E1 = 0x8000016
serial.SERIAL_7E1 = 0x800001a
serial.SERIAL_8E1 = 0x800001e
serial.SERIAL_5E2 = 0x8000032
serial.SERIAL_6E2 = 0x8000036
serial.SERIAL_7E2 = 0x800003a
serial.SERIAL_8E2 = 0x800003e
serial.SERIAL_5O1 = 0x8000013
serial.SERIAL_6O1 = 0x8000017
serial.SERIAL_7O1 = 0x800001b
serial.SERIAL_8O1 = 0x800001f
serial.SERIAL_5O2 = 0x8000033
serial.SERIAL_6O2 = 0x8000037
serial.SERIAL_7O2 = 0x800003b
serial.SERIAL_8O2 = 0x800003f
