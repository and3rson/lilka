---@meta

---@class http
http = {}

---Виконує HTTP-запит з параметрами, вказаними у таблиці.
---
---Очікує таблицю з такими ключами:
--- - `url` (string): URL для запиту.
--- - `method` (string, необов'язково): HTTP-метод (GET, POST тощо). За замовчуванням:
---   - "GET", якщо немає тіла запиту.
---   - "POST", якщо тіло запиту вказано.
--- - `body` (string, необов'язково): Тіло запиту (для POST або інших методів).
--- - `file` (string, необов'язково): Ім'я файлу для збереження відповіді.
---
---Повертає таблицю з результатом запиту:
--- - `code` (integer): HTTP-код відповіді.
--- - `response` (string, необов'язково): Відповідь сервера (тільки якщо `file` не задано).
---
---@param options table таблиця з параметрами запиту
---@return table результат запиту
---@usage
--- local result = http.execute({
---     url = "https://example.com/api",
---     method = "GET",
--- })
--- print("HTTP Code:", result.code)
--- if result.response then
---     print("Response:", result.response)
--- end
function http.execute(options) end

---HTTP-код для успішної відповіді.
http.HTTP_CODE_OK = 200
