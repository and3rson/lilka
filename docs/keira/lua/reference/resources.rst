``resources`` - Ресурси
-----------------------

Функції для роботи з ресурсами (зображеннями, звуками тощо).

Приклад:

.. code-block:: lua
    :linenos:

    local face = resources.load_image("face.bmp", display.color565(0, 0, 0))
    display.draw_image(face, 50, 80)

.. lua:autoclass:: resources
