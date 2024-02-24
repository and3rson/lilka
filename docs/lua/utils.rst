``utils`` - Утиліти
-------------------

Різні корисні функції.

.. contents::
    :local:
    :depth: 1

delay(ms)
^^^^^^^^^

.. lua:function:: delay(ms)

    Затримує виконання програми на певну кількість мілісекунд.

    :type ms: number
    :param ms: Кількість мілісекунд, на яку потрібно затримати виконання програми.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        display.set_cursor(0, 32)
        display.print("Зачекайте 1 секунду...")
        utils.delay(1000) -- Затримує виконання програми на 1 секунду.
        display.print("Готово!")
