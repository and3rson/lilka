``util`` - Утиліти
------------------

Різні корисні функції.

.. contents::
    :local:
    :depth: 1

.. lua:module:: util

delay(ms)
^^^^^^^^^

.. lua:function:: delay(s)

    Затримує виконання програми на певну кількість секунд.

    :type ms: number
    :param ms: Кількість секунд, на яку потрібно затримати виконання програми.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        display.set_cursor(0, 32)
        display.print("Зачекайте півсекунди...")
        util.delay(0.5) -- Затримує виконання програми на півсекунди.
        display.print("Готово!")
