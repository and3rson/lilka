``resources`` - Ресурси
-----------------------

Функції для роботи з ресурсами (зображеннями, звуками, тощо).

.. contents::
    :local:
    :depth: 1

Приклад:

.. code-block:: lua
    :linenos:

    local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
    display.draw_bitmap(face, 50, 80)

.. lua:module:: resources

load_bitmap(filename, [transparent_color])
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: load_bitmap(filename, [transparent_color])

    Завантажує зображення з файлу.

    :type filename: string
    :param filename: Шлях до файлу з зображенням (відносно місця знаходження скрипта, що виконується).
    :type transparent_color: number
    :param transparent_color: Колір, який буде вважатися прозорим. Якщо цей параметр не вказаний, зображення виводиться без прозорості.
    :rtype: table

    Повертає таблицю з ідентифікатором зображення (а також з його розмірами), яку можна використовувати для малювання зображення на екрані.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
        print(face.width, face.height) -- Виведе розміри зображення
        display.draw_bitmap(face, 50, 80) -- Виведе зображення на екран у позицію (50, 80)
