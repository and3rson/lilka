``math`` - Арифметичні функції
------------------------------

Різні корисні функції.

.. contents::
    :local:
    :depth: 1

.. lua:module:: math

random(a, [b])
^^^^^^^^^^^^^^

.. lua:function:: random(a, [b])

    Повертає випадкове число.

    :param a: Початок діапазону.
    :type a: int
    :param b: Кінець діапазону.
    :type b: int
    :rtype: int

    Якщо заданий лише один аргумент, повертає випадкове число в діапазоні [0;a] (включно).

    Якщо задані обидва аргументи, повертає випадкове число в діапазоні [a;b] (включно).

    Приклад використання:

    .. code-block:: lua
        :linenos:

        local r = math.random(10, 20)
        print(r) -- Виведе випадкове число від 10 до 20 (включно).

clamp(x, min, max)
^^^^^^^^^^^^^^^^^^

.. lua:function:: clamp(x, min, max)

    Обмежує число в діапазоні.

    :param x: Число, яке потрібно обмежити.
    :type x: number
    :param min: Мінімальне значення.
    :type min: number
    :param max: Максимальне значення.
    :type max: number
    :rtype: number

    Повертає число ``x``, обмежене в діапазоні [min;max].

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.clamp(8.1, 10, 20) -- Виведе 10
        print(math.clamp(15.2, 10, 20) -- Виведе 15.2
        print(math.clamp(23.3, 10, 20) -- Виведе 20

lerp(min, max, x)
^^^^^^^^^^^^^^^^^

.. lua:function:: lerp(min, max, x)

    Лінійна інтерполяція.

    :param min: Мінімальне значення.
    :type min: number
    :param max: Максимальне значення.
    :type max: number
    :param x: Число, яке потрібно інтерполювати в діапазоні [0;1].
    :type x: number
    :rtype: number

    Повертає число, яке лінійно інтерполюється між ``min`` та ``max`` відносно ``x``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.lerp(0, 100, 0.5) -- Виведе 50
        print(math.lerp(0, 100, 0.25) -- Виведе 25
        print(math.lerp(0, 100, 0.7125) -- Виведе 71.25

map(x, in_min, in_max, out_min, out_max)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. lua:function:: map(x, in_min, in_max, out_min, out_max)

    Перетворення значення з одного діапазону в інший.

    :param x: Число, яке потрібно перетворити.
    :type x: number
    :param in_min: Мінімальне значення вхідного діапазону.
    :type in_min: number
    :param in_max: Максимальне значення вхідного діапазону.
    :type in_max: number
    :param out_min: Мінімальне значення вихідного діапазону.
    :type out_min: number
    :param out_max: Максимальне значення вихідного діапазону.
    :type out_max: number
    :rtype: number

    Повертає число ``x``, перетворене з діапазону [in_min;in_max] в діапазон [out_min;out_max].

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.map(50, 0, 100, 0, 1) -- Виведе 0.5
        print(math.map(25, 0, 100, 0, 1) -- Виведе 0.25
        print(math.map(71.25, 0, 100, 0, 1) -- Виведе 0.7125

dist(x1, y1, x2, y2)
^^^^^^^^^^^^^^^^^^^^

.. lua:function:: dist(x1, y1, x2, y2)

    Відстань між двома точками.

    :param x1: X-координата першої точки.
    :type x1: number
    :param y1: Y-координата першої точки.
    :type y1: number
    :param x2: X-координата другої точки.
    :type x2: number
    :param y2: Y-координата другої точки.
    :type y2: number
    :rtype: number

    Повертає відстань між точками (x1;y1) та (x2;y2).

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.dist(0, 0, 3, 4) -- Виведе 5
        print(math.dist(0, 0, 1, 1) -- Виведе 1.4142135623731
        print(math.dist(0, 0, 0, 0) -- Виведе 0

abs(x)
^^^^^^

.. lua:function:: abs(x)

    Модуль числа.

    :param x: Число, модуль якого потрібно знайти.
    :type x: number
    :rtype: number

    Повертає модуль числа ``x``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.abs(-5) -- Виведе 5
        print(math.abs(5) -- Виведе 5
        print(math.abs(0) -- Виведе 0

sign(x)
^^^^^^^

.. lua:function:: sign(x)

    Знак числа.

    :param x: Число, знак якого потрібно знайти.
    :type x: number
    :rtype: int

    Повертає знак числа ``x``: -1, якщо число від'ємне, 0, якщо число дорівнює 0, 1, якщо число додатнє.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.sign(-5) -- Виведе -1
        print(math.sign(5) -- Виведе 1
        print(math.sign(0) -- Виведе 0

sqrt(x)
^^^^^^^

.. lua:function:: sqrt(x)

    Квадратний корінь.

    :param x: Число, квадратний корінь якого потрібно знайти.
    :type x: number
    :rtype: number

    Повертає квадратний корінь числа ``x``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.sqrt(25) -- Виведе 5
        print(math.sqrt(16) -- Виведе 4
        print(math.sqrt(8) -- Виведе 2.8284271247462
        print(math.sqrt(0) -- Виведе 0

min(table)
^^^^^^^^^^

.. lua:function:: min(table)

    Мінімальне значення в таблиці.

    :param table: Таблиця чисел.
    :type table: table
    :rtype: number

    Повертає мінімальне значення в таблиці.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.min({1.1, 2.2, 3.3, 4.4, 5.5}) -- Виведе 1.1
        print(math.min({5, 4, 3, 2, 1}) -- Виведе 1
        print(math.min({-5, -4, -3, -2, -1}) -- Виведе -5

max(table)
^^^^^^^^^^

.. lua:function:: max(table)

    Максимальне значення в таблиці.

    :param table: Таблиця чисел.
    :type table: table
    :rtype: number

    Повертає максимальне значення в таблиці.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.max({1.1, 2.2, 3.3, 4.4, 5.5}) -- Виведе 5.5
        print(math.max({5, 4, 3, 2, 1}) -- Виведе 5
        print(math.max({-5, -4, -3, -2, -1}) -- Виведе -1

sum(table)
^^^^^^^^^^

.. lua:function:: sum(table)

    Сума всіх значень в таблиці.

    :param table: Таблиця чисел.
    :type table: table
    :rtype: number

    Повертає суму всіх значень в таблиці.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.sum({1, 2, 3, 4, 5}) -- Виведе 15
        print(math.sum({5.5, 4.5, 3.3, 2.2, 1.1}) -- Виведе 16.6
        print(math.sum({-5, -4, -3, -2, -1}) -- Виведе -15

avg(table)
^^^^^^^^^^

.. lua:function:: avg(table)

    Середнє значення в таблиці.

    :param table: Таблиця чисел.
    :type table: table
    :rtype: number

    Повертає середнє значення в таблиці.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.avg({1, 2, 3, 4, 5}) -- Виведе 3
        print(math.avg({5.5, 4.5, 3.3, 2.2, 1.1}) -- Виведе 3.32
        print(math.avg({-5, -4, -3, -2, -1}) -- Виведе -3

floor(x)
^^^^^^^^

.. lua:function:: floor(x)

    Округлення вниз.

    :param x: Число, яке потрібно округлити.
    :type x: number
    :rtype: int

    Повертає найбільше ціле число, яке менше або рівне ``x``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.floor(5.5) -- Виведе 5
        print(math.floor(5.9) -- Виведе 5
        print(math.floor(-5.5) -- Виведе -6
        print(math.floor(-5.9) -- Виведе -6

ceil(x)
^^^^^^^

.. lua:function:: ceil(x)

    Округлення вгору.

    :param x: Число, яке потрібно округлити.
    :type x: number
    :rtype: int

    Повертає найменше ціле число, яке більше або рівне ``x``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.ceil(5.5) -- Виведе 6
        print(math.ceil(5.9) -- Виведе 6
        print(math.ceil(-5.5) -- Виведе -5
        print(math.ceil(-5.9) -- Виведе -5

round(x)
^^^^^^^^

.. lua:function:: round(x)

    Округлення до найближчого цілого.

    :param x: Число, яке потрібно округлити.
    :type x: number
    :rtype: int

    Повертає найближче ціле число до ``x``.

    Приклад використання:

    .. code-block:: lua
        :linenos:

        print(math.round(5.5) -- Виведе 6
        print(math.round(5.9) -- Виведе 6
        print(math.round(-5.5) -- Виведе -6
        print(math.round(-5.9) -- Виведе -6
