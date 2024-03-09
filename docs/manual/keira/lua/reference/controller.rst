``controller`` - Введення
-------------------------

Функції для роботи з введенням.

Приклад:

.. code-block:: lua
    :linenos:

    display.set_cursor(32, 32)

    while true do
        local state = controller.get_stage()

        if state.a.just_pressed then
            print("A щойно натиснуто!")
        elseif state.a.just_released then
            print("A щойно відпущено!")
        end
    end

.. lua:autoclass:: controller
