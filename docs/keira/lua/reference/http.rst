``http`` - Робота з HTTP запитами
---------------------------------

Функції для виконання HTTP запитів.

Приклад:

.. code-block:: lua
    :linenos:

    -- збереження результату в файл
    local request = {
        url = "https://a.tile.openstreetmap.org/12/2395/1381.png",
        file = "/12/2395/1381.png"
    }
    local response = http.execute(request)
    console.print("Result:", response.code)

    -- повернення результату у вигляді рядка
    request = {
        url = "https://api.appstoreconnect.apple.com/v1/devices"
    }
    local response = http.execute(request)
    console.print(response.code, response.response)

.. lua:autoclass:: http
