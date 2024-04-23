``transform`` - Перетворення зображень
--------------------------------------

Функції для обертання та масштабування зображень.

Приклад:

.. code-block:: lua

    local image = resource.load_image("face.bmp", display.color565(0, 0, 0))
    local transform = transforms.new()
    transform = transform:scale(1.5, 0.5)
    transform = transform:rotate(45)
    display.draw_image(image, 50, 80, transform) -- малює зображення в точці (50, 80) з перетвореннями

.. lua:autoclass:: transforms

.. lua:autoclass:: Transform
