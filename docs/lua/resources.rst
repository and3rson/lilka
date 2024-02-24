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

.. lua:autoclass:: resources
