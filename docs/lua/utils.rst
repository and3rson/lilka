``utils`` - Утиліти
-------------------

Різні корисні функції.

.. code-block:: lua
    :linenos:

    local utils = require("utils")
    local display = require("display")

    display.set_cursor(0, 32)

    local r = utils.random(10, 20)
    display.print("Випадкове число: " .. r)

    display.print("Зачекайте 1 секунду...")
    utils.delay(1000) -- Затримує виконання програми на 1 секунду.

    r = utils.random(10, 20)
    display.print("Нове випадкове число: " .. r)

.. lua:module:: utils

random(a, [b])
^^^^^^^^^^^^^^

.. lua:function:: random(a, [b])

    Повертає випадкове число.

    :type a: number
    :type b: number

    Якщо заданий лише один аргумент, повертає випадкове число в діапазоні [0;a] (включно).

    Якщо задані обидва аргументи, повертає випадкове число в діапазоні [a;b] (включно).

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local utils = require("utils")

        local r = utils.random(10, 20)
        print(r) -- Виведе випадкове число від 10 до 20 (включно).

delay(ms)
^^^^^^^^^

.. lua:function:: delay(ms)

    Затримує виконання програми на певну кількість мілісекунд.

    :type ms: number
    :param ms: Кількість мілісекунд, на яку потрібно затримати виконання програми.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local utils = require("utils")
        local display = require("utils")

        display.set_cursor(0, 32)
        display.print("Зачекайте 1 секунду...")
        utils.delay(1000) -- Затримує виконання програми на 1 секунду.
        display.print("Готово!")
