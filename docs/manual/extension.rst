Роз'єм розширення
=================

.. contents:: Зміст
    :local:
    :depth: 1

Загальна інформація
-------------------

Роз'єм розширення (англ. *extension header*) - це роз'єм, який дозволяє підключати до основної плати додаткові модулі, наприклад датчики, дисплеї, сервоприводи тощо.

Лілка має 10-контактний роз'єм розширення. Цей роз'єм має 2 ряди по 5 контактів кожен.

Ці піни можна використовувати для будь-якої цілі. Саме тому вони і називаються GPIO (General Purpose Input/Output) - вхід/вихід загального призначення.

.. raw:: html

    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 700 400" style="width: 100%; max-width: 600px">
        <rect x=0 y=0 width=700 height=400 fill=#1144AA />
        <defs>
            <style type="text/css">
                <![CDATA[
                text {
                    font-family: sans-serif;
                    fill: #00FF00;
                    text-anchor: middle;
                    dominant-baseline: middle;
                    font-size: 2.5em;
                }
                text.gnd {
                    fill: #AAAAAA;
                    font-size: 2em;
                }
                text.vcc {
                    fill: #FF4444;
                    font-size: 2em;
                }
                ]]>
            </style>
            <g id=pin>
                <circle r=35 fill=#FFCC00 />
                <circle r=20 fill=black />
                <circle r=40 fill=none stroke=#FFCC00 stroke-width=2 />
            </g>
            <g id=square>
                <rect x=-35 y=-35 width=70 height=70 fill=#FFCC00 />
                <circle r=20 fill=black />
                <rect x=-40 y=-40 fill=none width=80 height=80 stroke=#FFCC00 stroke-width=2 />
            </g>
        </defs>
        <rect fill=none stroke=#FFFFAA stroke-width=4 x=100 y=100 width=500 height=200 />

        <use xlink:href=#pin x=150 y=150 />
        <use xlink:href=#pin x=250 y=150 />
        <use xlink:href=#pin x=350 y=150 />
        <use xlink:href=#pin x=450 y=150 />
        <use xlink:href=#pin x=550 y=150 />
        <use xlink:href=#square x=150 y=250 />
        <use xlink:href=#pin x=250 y=250 />
        <use xlink:href=#pin x=350 y=250 />
        <use xlink:href=#pin x=450 y=250 />
        <use xlink:href=#pin x=550 y=250 />

        <text x=150 y=70 class=vcc>3.3V</text>
        <text x=250 y=70>47</text>
        <text x=350 y=70>14</text>
        <text x=450 y=70>12</text>
        <text x=550 y=70 class=gnd>GND</text>
        <text x=150 y=330 class=gnd>GND</text>
        <text x=250 y=330>48</text>
        <text x=350 y=330>21</text>
        <text x=450 y=330>13</text>
        <text x=550 y=330 class=vcc>3.3V</text>

        <!-- <rect fill=#00FF0030 stroke=#00AA00 stroke-width=4 x=200 y=30 width=300 height=340 /> -->
    </svg>

На роз'єм розширення Лілки виведені наступні піни:

- ``3.3V`` - живлення 3.3 Вольти
- ``GND`` - земля
- ``12``, ``13``, ``14``, ``21``, ``47``, ``48`` - GPIO

Вони не використовуються Лілкою для жодних внутрішніх потреб, тому ви можете використовувати їх для будь-яких цілей.

Завдяки вбудованому мультиплексору, кожен пін можна використовувати для шин I2C, SPI, UART, або просто як вхід/вихід.

Крім того, піни ``12``, ``13`` та ``14`` під'єднані до АЦП (аналого-цифрового перетворювача, ADC):

- ``12`` - ADC2, канал 1
- ``13`` - ADC2, канал 2
- ``14`` - ADC2, канал 3


Приклад використання GPIO
-------------------------

Найпростіший приклад використання GPIO - це підключити до роз'єму розширення світлодіод та резистор, щоб світлодіод світився, коли на пін подаватиметься напруга.

Наприклад, пін ``12`` можна підключити до анода світлодіода (довший вивід) через резистор (наприклад, 100 Ом), а катод світлодіода (коротший вивід) - до землі.

Обчислити значення резистора для світлодіода можна за `цим посиланням <https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-led-series-resistor>`_.

Після цього можна використати наступний код для того, щоб світлодіод блимав:

.. code-block:: cpp

    #include <lilka.h>

    void setup() {
        lilka::begin();
    }

    void loop() {
        digitalWrite(12, HIGH);
        delay(500);
        digitalWrite(12, LOW);
        delay(5000);
    }

Використання GPIO для шини I2C
------------------------------

Щоб використати піни розширення для шини I2C, використайте наступний код:

.. code-block:: cpp

    #include <lilka.h>
    #include <Wire.h>

    #define SDA_PIN 13
    #define SCL_PIN 14

    void setup() {
        lilka::begin();
        Wire.begin(SDA_PIN, SCL_PIN);
    }

    void loop() {
        // Почати передачу на адресу 0x42
        Wire.beginTransmission(0x42);
        // Надіслати байт 66
        Wire.write(0x42);
        // Завершити передачу
        Wire.endTransmission();
        delay(1000);
    }

Використання GPIO для шини SPI
------------------------------

Щоб використати піни розширення для шини SPI, використайте наступний код:

.. code-block:: cpp

    #include <lilka.h>

    #define SCK_PIN 12
    #define MISO_PIN 13
    #define MOSI_PIN 14
    #define SS_PIN 21

    void setup() {
        lilka::begin();

        // Ми використовуємо SPI2, тому що SPI1 використовується для внутрішніх потреб Лілки (для дисплея та SD-картки)
        lilka::SPI2.begin(SCK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
    }

    void loop() {
        // Починаємо транзакцію: швидкість - 1 МГц, порядок байтів - MSB, режим SPI - 0
        lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
        // Активуємо пристрій
        digitalWrite(SS_PIN, LOW);
        // Надсилаємо байт 66
        lilka::SPI2.transfer(0x42);
        // Деактивуємо пристрій
        digitalWrite(SS_PIN, HIGH);
        // Завершуємо транзакцію
        lilka::SPI2.endTransaction();
        delay(1000);
    }
