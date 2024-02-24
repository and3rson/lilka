``display``: Дисплей
--------------------

Основні функції для роботи з дисплеєм.

.. contents::
    :local:
    :depth: 1

Приклад:

.. code-block:: lua
    :linenos:

    display.set_cursor(0, 0)
    display.print("Hello,", "world!", 69, 420, "nice")

    local color = display.color565(255, 0, 0)
    display.draw_line(0, 0, 100, 100, color)

    local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
    display.draw_bitmap(face, 50, 80)

    display.set_buffered(true)
    display.fill_rect(0, 0, 100, 100, color)
    display.render()

    display.set_buffered(false)
    display.fill_rect(0, 0, 100, 100, color)

.. lua:module:: display

color565(r, g, b)
^^^^^^^^^^^^^^^^^

.. lua:function:: color565(r, g, b)

    Повертає 16-бітне число, що представляє колір з RGB-компонентами.

    :type r: number
    :param r: Компонента червоного кольору (від 0 до 255).
    :type g: number
    :param g: Компонента зеленого кольору (від 0 до 255).
    :type b: number
    :param b: Компонента синього кольору (від 0 до 255).

    Наприклад, ``display.color565(255, 0, 0)`` поверне 63488, що відповідає червоному кольору.

    Це потрібно, оскільки дисплей Лілки працює з 16-бітними кольорами (5-6-5), а не з 24-бітними (8-8-8).

    Всі функції, що приймають аргумент ``color``, очікують, що він буде саме 16-бітним числом.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local color = display.color565(255, 0, 0)
        display.draw_line(0, 0, 100, 100, color)

set_cursor(x, y)
^^^^^^^^^^^^^^^^

.. lua:function:: set_cursor(x, y)

    Встановлює позицію курсора.

    Позиція курсора використовується для виведення тексту на екран.

set_font(font)
^^^^^^^^^^^^^^

.. lua:function:: set_font(font)

    Встановлює шрифт для виведення тексту.

    :type font: string
    :param font: Ідентифікатор шрифту.

    Доступні шрифти:

    * ``"4x6"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#4x6
    * ``"5x7"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#5x7
    * ``"5x8"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#5x8
    * ``"6x12"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#6x12
    * ``"6x13"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#6x13
    * ``"7x13"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#7x13
    * ``"8x13"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#8x13
    * ``"9x15"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#9x15
    * ``"10x20"``: https://github.com/olikraus/u8g2/wiki/fntgrpx11#10x20

    Приклад використання:

    .. code-block:: lua
        :linenos:

        display.set_font("6x13")
        display.set_cursor(8, 32)
        display.print("Привіт,")
        display.set_cursor(8, 64)
        display.set_font("10x20")
        display.print("Лілка!")

set_text_size(size)
^^^^^^^^^^^^^^^^^^^

.. lua:function:: set_text_size(size)

    Встановлює масштабування тексту.

    :type size: number
    :param size: Масштаб тексту.

    Якщо цей параметр дорівнює 1, текст виводиться в масштабі 1:1. Якщо 2, то кожен піксель тексту буде займати 2x2 пікселі на екрані, і так далі.

set_text_color(color, [background])
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: set_text_color(color, [background])

    Встановлює кольори тексту.

    :type color: number
    :param color: Колір тексту.
    :type background: number
    :param background: Колір фону тексту. Якщо цей параметр не вказаний, фон тексту буде прозорим.

print(text, ...)
^^^^^^^^^^^^^^^^

.. lua:function:: print(text, ...)

    Виводить текст на екран.

    :type text: string
    :param text: Текст, який потрібно вивести на екран.
    :type ...: any
    :param ...: Додаткові аргументи, які будуть вставлені в текст.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        display.print("Hello,", "world!", 69, 420, "nice")

fill_screen(color)
^^^^^^^^^^^^^^^^^^

.. lua:function:: fill_screen(color)

    Заповнює екран кольором.

draw_pixel(x, y, color)
^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_pixel(x, y, color)

    Малює піксель на екрані.

draw_line(x1, y1, x2, y2, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_line(x1, y1, x2, y2, color)

    Малює лінію з координатами (x1, y1) і (x2, y2) кольором color.

    :type x1: number
    :param x1: Координата x початку лінії.
    :type y1: number
    :param y1: Координата y початку лінії.
    :type x2: number
    :param x2: Координата x кінця лінії.
    :type y2: number
    :param y2: Координата y кінця лінії.
    :type color: number
    :param color: Колір лінії.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local color = display.color565(255, 0, 0)
        display.draw_line(0, 0, 100, 100, color)

draw_rect(x, y, w, h, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_rect(x, y, w, h, color)

    Малює прямокутник з координатами (x, y) і шириною w та висотою h кольором color.

fill_rect(x, y, w, h, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: fill_rect(x, y, w, h, color)

    Малює **заповнений** прямокутник з координатами (x, y) і шириною w та висотою h кольором color.

draw_circle(x, y, r, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_circle(x, y, r, color)

    Малює коло з центром у точці (x, y) та радіусом r кольором color.

fill_circle(x, y, r, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: fill_circle(x, y, r, color)

    Малює **заповнене** коло з центром у точці (x, y) та радіусом r кольором color.

draw_triangle(x1, y1, x2, y2, x3, y3, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_triangle(x1, y1, x2, y2, x3, y3, color)

    Малює трикутник з вершинами у точках (x1, y1), (x2, y2) та (x3, y3) кольором color.

fill_triangle(x1, y1, x2, y2, x3, y3, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: fill_triangle(x1, y1, x2, y2, x3, y3, color)

    Малює **заповнений** трикутник з вершинами у точках (x1, y1), (x2, y2) та (x3, y3) кольором color.

draw_ellipse(x, y, rx, ry, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_ellipse(x, y, rx, ry, color)

    Малює еліпс з центром у точці (x, y), радіусом rx по горизонталі та радіусом ry по вертикалі кольором color.

fill_ellipse(x, y, rx, ry, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: fill_ellipse(x, y, rx, ry, color)

    Малює **заповнений** еліпс з центром у точці (x, y), радіусом rx по горизонталі та радіусом ry по вертикалі кольором color.

draw_arc(x, y, r1, r2, start_angle, end_angle, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_arc(x, y, r1, r2, start_angle, end_angle, color)

    Малює дугу кола з центром у точці (x, y), радіусами r1 та r2, кутом початку start_angle та кутом кінця end_angle кольором color.

    :type x: number
    :param x: Координата x центру дуги.
    :type y: number
    :param y: Координата y центру дуги.
    :type r1: number
    :param r1: Зовнішній радіус дуги.
    :type r2: number
    :param r2: Внутрішній радіус дуги.
    :type start_angle: number
    :param start_angle: Кут початку дуги (в градусах).
    :type end_angle: number
    :param end_angle: Кут кінця дуги (в градусах).
    :type color: number
    :param color: Колір дуги.

fill_arc(x, y, r1, r2, start_angle, end_angle, color)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: fill_arc(x, y, r1, r2, start_angle, end_angle, color)

    Малює **заповнену** дугу кола з центром у точці (x, y), радіусами r1 та r2, кутом початку start_angle та кутом кінця end_angle кольором color.

draw_bitmap(id, x, y)
^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: draw_bitmap(id, x, y)

    Виводить зображення на екран.

    :type id: number
    :param id: Ідентифікатор зображення, отриманий з :lua:func:`resources.load_bitmap`.
    :type x: number
    :param x: Координата x верхнього лівого кута зображення.
    :type y: number
    :param y: Координата y верхнього лівого кута зображення.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
        display.draw_bitmap(face, 50, 80) -- Виводить зображення на екран у точці (50, 80)

set_buffered(value)
^^^^^^^^^^^^^^^^^^^

.. lua:function:: set_buffered(value)

    Вмикає або вимикає буферизоване малювання.

    :type value: boolean
    :param value: Якщо цей параметр дорівнює true, буферизація дисплея вмикається. Якщо false, вимикається.

    Буферизація дисплея дозволяє малювати всю графіку на полотні-буфері в пам'яті, а потім виводити цей буфер на екран. Це дозволяє уникнути мерехтіння зображення на екрані.

    Щоб вивести буфер на екран, використовуйте функцію :lua:func:`display.render`.

    За замовчуванням, буферизація екрану **увімкнена**, щоб запобігти мерехтінню зображення на екрані. Це трохи збільшує затримку між малюванням графіки та виведенням її на екран, але відсутність мерехтіння зображення на екрані зазвичай в пріоритеті.

    Приклад коду, при якому буде дуже помітне мерехтіння зображення на екрані:

    .. code-block:: lua
        :linenos:

        -- Вимикаємо буферизацію екрану:
        display.set_buffered(false)

        while true do
            -- Заповнюємо екран чорним кольором:
            display.fill_screen(display.color565(0, 0, 0))
            -- Малюємо червоний прямокутник на екрані:
            display.fill_rect(0, 0, 100, 100, display.color565(255, 0, 0))
        end

    Якщо ви виконаєте цей код, ви побачите, що червоний прямокутник мерехтить на екрані.
    Ось той самий код, але з (за замовчуванням) увімкненою буферизацією екрану:

    .. code-block:: lua
        :linenos:

        while true do
            -- Заповнюємо екран чорним кольором:
            display.fill_screen(display.color565(0, 0, 0))
            -- Малюємо червоний прямокутник на екрані:
            display.fill_rect(0, 0, 100, 100, display.color565(255, 0, 0))
            -- Виводимо зображення з буфера на екран:
            display.render()
            -- Жодного мерехтіння не буде!
        end

render()
^^^^^^^^

.. lua:function:: render()

    Виводить зображення з буфера на екран.

    Якщо буферизація екрану вимкнена, ця функція повертає помилку.
