``display``: Дисплей
--------------------

Основні функції для роботи з дисплеєм.

Приклад:

.. code-block:: lua
    :linenos:

    display.set_cursor(0, 0)
    display.print("Hello,", "world!", 69, 420, "nice")

    local color = display.color565(255, 0, 0)
    display.draw_line(0, 0, 100, 100, color)

    local face = resources.load_image("face.bmp", display.color565(0, 0, 0))
    display.draw_image(face, 50, 80)

    display.fill_rect(0, 0, 100, 100, color)

    display.queue_draw()

.. lua:autoclass:: display
