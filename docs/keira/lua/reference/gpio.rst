``gpio`` - Керування GPIO-пінами
================================

Функції для роботи з GPIO-пінами :ref:`роз'єму розширення <extension>`.

Приклад:

.. code-block:: lua

    -- Ця програма блимає світлодіодом, під'єднаним до піна 12 через резистор.

    led_pin = 12

    -- Встановлюємо пін 12 в режим виводу
    gpio.mode(led_pin, gpio.OUTPUT)
    while true do
        -- Вмикаємо світлодіод
        gpio.write(led_pin, gpio.HIGH)
        util.sleep(0.5)
        -- Вимикаємо світлодіод
        gpio.write(led_pin, gpio.LOW)
        util.sleep(0.5)
        if controller.get_state().a.just_pressed then
            -- Кнопку A щойно натиснули, зупиняємо цикл
            break
        end
    end

.. lua:autoclass:: gpio
