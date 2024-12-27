---@meta

---@class wifi
wifi = {}

---Під'єднатись до мережі Wi-Fi.
---
---@param ssid string ім'я мережі
---@param password string пароль мережі
function wifi.connect(ssid, password) end

---Від'єднатись від мережі Wi-Fi.
function wifi.disconnect() end

---Отримати статус мережі Wi-Fi.
---
---@return integer статус мережі (див. ``WiFi.status()`` у документації Arduino ESP32)
function wifi.get_status() end

---Повернути таблицю з назвами доступних мереж Wi-Fi.
---
---@return table
---@usage
--- local networks = wifi.scan()
--- for i = 0, #networks do
---     print(networks[i])
---     print('Сила сигналу: ', wifi.get_rssi(i))
---     print('Тип шифрування: ', wifi.get_encryption_type(i))
--- end
function wifi.scan() end

---Отримати силу сигналу відповідної мережі Wi-Fi.
---
---@param index integer індекс мережі (порядковий номер у списку, отриманому функцією ``wifi.scan()``)
---@return integer
function wifi.get_rssi(index) end

---Отримати тип шифрування відповідної мережі Wi-Fi.
---
---@param index integer індекс мережі (порядковий номер у списку, отриманому функцією ``wifi.scan()``)
---@return integer
function wifi.get_encryption_type(index) end

---Отримати MAC-адресу пристрою.
---
---@return string
function wifi.get_mac() end

---Отримати локальну IP-адресу пристрою.
---
---@return string
function wifi.get_local_ip() end

---Налаштувати параметри мережі Wi-Fi для статичної IP-адреси.
---
---(Див. функцію ``WiFi.config()`` у документації Arduino ESP32)
---
---@param ip string IP-адреса
---@param gateway string IP-адреса шлюзу
---@param subnet string маска підмережі
---@param dns1 string IP-адреса DNS-сервера 1
---@param dns2 string IP-адреса DNS-сервера 2
function wifi.set_config(ip, gateway, subnet, dns1, dns2) end

return wifi
