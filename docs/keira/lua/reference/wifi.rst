``wifi`` - Робота з WiFi-мережами
---------------------------------

Функції для роботи з WiFi-мережами.

Приклад:

.. code-block:: lua
    :linenos:

    local networks = wifi.scan()
    for i = 0, #networks do
        print(networks[i])
        print('Сила сигналу: ', wifi.get_rssi(i))
        print('Тип шифрування: ', wifi.get_encryption_type(i))
    end

.. lua:autoclass:: wifi
