Написання програм на Lua
========================

Основна прошивка Лілки має вбудований інтерпретатор мови програмування Lua. Це дозволяє писати та виконувати програми на Lua прямо з SD-картки, без необхідності компіляції чи перепрошивання Лілки.

.. warning:: Якщо ви не знайомі з мовою Lua, рекомендуємо прочитати туторіал з Lua перед тим, як продовжувати: https://www.lua.org/pil/1.html

.. contents::
    :depth: 2
    :local:
    :backlinks: none

Приклад програми
----------------


Оскільки Lua сама по собі - це універсальна мова програмування, вона не має вбудованих функцій для роботи з дисплеєм, звуком, тощо. Тому для роботи з цими пристроями на Лілці використовуються спеціальні вбудовані модулі, які надають доступ до функцій пристроїв.

Модулі завантажуються за допомогою функції ``require``. Ось приклад простої програми на Lua, яка виводить текст "Hello, world!" на екран:

.. code-block:: lua
    :linenos:

    -- Завантажуємо модуль для роботи з дисплеєм:
    local display = require("display")
    -- Завантажуємо модуль, що містить корисні функції:
    local utils = require("utils")

    -- Заповнюємо екран чорним кольором:
    display.fill_screen(display.color565(0, 0, 0))

    -- Виводимо текст "Hello, world!" на екран:
    display.set_cursor(0, 32)
    display.print("Hello, world!")

    -- Чекаємо 2 секунди:
    utils.delay(2000)

Ви можете зберегти цей код у файл з розширенням ``.lua`` на SD-картці, а потім виконати його, обравши його в браузері SD-картки.

Нижче наведено список модулів, які доступні на Лілці, та їх функції.

Дисплей
-------

Основні функції для роботи з дисплеєм.

Приклад:

.. code-block:: lua
    :linenos:

    local display = require("display")

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

        local display = require("display")

        local color = display.color565(255, 0, 0)
        display.draw_line(0, 0, 100, 100, color)

.. lua:function:: set_cursor(x, y)

    Встановлює позицію курсора.

    :type x: number
    :param x: Координата x.
    :type y: number
    :param y: Координата y.

    Позиція курсора використовується для виведення тексту на екран.

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

        local display = require("display")

        local color = display.color565(255, 0, 0)
        display.draw_line(0, 0, 100, 100, color)

.. lua:function:: fill_rect(x, y, w, h, color)

    Заповнює прямокутник з координатами (x, y) і шириною w та висотою h кольором color.

.. lua:function:: draw_bitmap(id, x, y)

    Виводить зображення на екран.

    :type id: number
    :param id: Ідентифікатор зображення, отриманий з :lua:func:`resources.load_bitmap`.
    :type x: number
    :param x: Координата x.
    :type y: number
    :param y: Координата y.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local display = require("display")
        local resources = require("resources")

        local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
        display.draw_bitmap(face, 50, 80)

.. lua:function:: set_buffered(value)

    Вмикає або вимикає буферизацію екрану.

    :type value: boolean
    :param value: Якщо цей параметр дорівнює true, буферизація екрану вмикається. Якщо false, вимикається.

    Буферизація екрану дозволяє зберігати зображення на екрані в пам'яті, а потім виводити його на екран. Це дозволяє уникнути мерехтіння зображення на екрані.

    Щоб вивести зображення на екран, використовуйте функцію :lua:func:`display.render`.

    За замовчуванням, буферизація екрану **вимкнена**.

.. lua:function:: render()

    Виводить зображення з буфера на екран.

    Якщо буферизація екрану вимкнена, ця функція повертає помилку.

Ресурси
-------

Функції для роботи з ресурсами (зображеннями, звуками, тощо).

Приклад:

.. code-block:: lua
    :linenos:

    local display = require("display")
    local resources = require("resources")

    local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
    display.draw_bitmap(face, 50, 80)

.. lua:module:: resources

.. lua:function:: load_bitmap(filename, [transparent_color])

    Завантажує зображення з файлу.

    :type filename: string
    :param filename: Шлях до файлу з зображенням (відносно місця знаходження скрипта, що виконується).
    :type transparent_color: number
    :param transparent_color: Колір, який буде вважатися прозорим. Якщо цей параметр не вказаний, зображення виводиться без прозорості.

    Повертає ідентифікатор зображення, який можна використовувати для малювання зображення на екрані.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local display = require("display")
        local resources = require("resources")

        local face = resources.load_bitmap("face.bmp", display.color565(0, 0, 0))
        display.draw_bitmap(face, 50, 80)

Введення
--------

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

Утиліти
-------

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
