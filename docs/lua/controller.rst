``controller`` - Введення
-------------------------

Функції для роботи з введенням.

Приклад:

.. code-block:: lua
    :linenos:

    local controller = require("controller")
    local display = require("display")

    display.set_cursor(32, 32)

    while true do
        local state = controller.get_stage()

        if state.a.just_pressed then
            print("A щойно натиснуто!")
        elseif state.a.just_released then
            print("A щойно відпущено!")
        end
    end

.. lua:module:: controller

controller.get_state()
^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: controller.get_state()

    Повертає стан контролера.

    Повертає таблицю з наступними полями:

    * ``up``: стан кнопки "вгору".
    * ``down``: стан кнопки "вниз".
    * ``left``: стан кнопки "вліво".
    * ``right``: стан кнопки "вправо".
    * ``a``: стан кнопки A.
    * ``b``: стан кнопки B.
    * ``c``: стан кнопки C.
    * ``d``: стан кнопки D.
    * ``select``: стан кнопки "вибір".
    * ``start``: стан кнопки "старт".

    Кожне поле має наступні підполя:

    * ``pressed``: true, якщо кнопка натиснута.
    * ``just_pressed``: true, якщо кнопка щойно натиснута вперше з моменту попереднього виклику ``controller.get_state``.
    * ``just_released``: true, якщо кнопка щойно відпущена вперше з моменту попереднього виклику ``controller.get_state``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local controller = require("controller")
        local display = require("display")

        display.set_cursor(0, 32)

        while true do
            local state = controller.get_state()

            if state.a.just_pressed then
                print("A щойно натиснуто!")
            elseif state.a.just_released then
                print("A щойно відпущено!")
            end
        end
