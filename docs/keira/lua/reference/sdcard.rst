``sdcard`` - Робота з SD-картою
-------------------------------

Функції для роботи читання/запису файлів на SD-карті.

Приклад:

.. code-block:: lua
    :linenos:

    local file = sdcard.open("file.txt", "w")
    file:write("Hello, world!")

.. lua:autoclass:: sdcard

.. lua:autoclass:: File
