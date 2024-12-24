``serial`` - Робота з UART1
-------------------------------

Функції для читання/запису даних з UART1
Підключення до UART1 здійснюється через піни RX 47 та TX 48, які знаходяться на роз'ємі розширення.

Приклад:

.. code-block:: lua
    :linenos:

    local buffer = 'a'
    local up, down = false, false

    function lilka.init()
        serial.begin(115200);
    end

    function lilka.update(delta)
        while serial.available() > 0 do
            buffer = buffer .. serial.read(serial.available())
            if string.len(buffer) > 40 then
                console.print(buffer)
                buffer = 'a'
            end
        end

        local keyboard = controller.get_state()
        if keyboard.up.pressed or keyboard.down.pressed then
            serial.print("button: ")
            if keyboard.up.pressed then
                serial.println("up")
            elseif keyboard.down.pressed then
                serial.println("down")
            end
        end
        if keyboard.b.pressed then
            util.exit()
        end
    end

    function lilka.draw()
        display.fill_screen(display.color565(0, 0, 0))
        display.set_cursor(20, 20)
        display.print(buffer)
    end

.. lua:autoclass:: serial
