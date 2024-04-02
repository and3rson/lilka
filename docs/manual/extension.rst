Роз'єм розширення
=================

.. contents:: Зміст
    :local:
    :depth: 1

Загальна інформація
-------------------

Роз'єм розширення (англ. *extension header*) - це роз'єм, який дозволяє підключати до основної плати додаткові модулі, наприклад датчики, дисплеї, сервоприводи тощо.

Лілка має 12-контактний роз'єм розширення.

Ці піни можна використовувати для будь-якої цілі. Саме тому вони і називаються GPIO (General Purpose Input/Output) - вхід/вихід загального призначення.

.. raw:: html

    <svg id="ext-header" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1400 300" style="width: 100%; max-width: 800px">
        <rect x=0 y=0 width=1400 height=300 fill=#1144AA />
        <defs>
            <style type="text/css">
                <![CDATA[
                #ext-header text {
                    font-family: sans-serif;
                    fill: #00FF00;
                    text-anchor: middle;
                    dominant-baseline: middle;
                    font-size: 2.5em;
                }

                #ext-header text.gnd {
                    fill: #AAAAAA;
                    font-size: 2em;
                }

                #ext-header text.vcc {
                    fill: #FF4444;
                    font-size: 2em;
                }

                #ext-header text.neutral {
                    fill: #FFFFFF;
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
        <rect fill=none stroke=#FFFFAA stroke-width=4 x=100 y=100 width=1200 height=100 />

        <rect fill=#00FF0030 stroke=#00AA00 stroke-width=4 x=300 y=30 width=800 height=220 stroke-dasharray="5,5" />
        <text x=700 y=280>GPIO</text>

        <use xlink:href=#square x=150 y=150 />
        <use xlink:href=#pin x=250 y=150 />
        <use xlink:href=#pin x=350 y=150 />
        <use xlink:href=#pin x=450 y=150 />
        <use xlink:href=#pin x=550 y=150 />
        <use xlink:href=#pin x=650 y=150 />
        <use xlink:href=#pin x=750 y=150 />
        <use xlink:href=#pin x=850 y=150 />
        <use xlink:href=#pin x=950 y=150 />
        <use xlink:href=#pin x=1050 y=150 />
        <use xlink:href=#pin x=1150 y=150 />
        <use xlink:href=#pin x=1250 y=150 />

        <text x=150 y=70 class=gnd>GND</text>
        <text x=250 y=70 class=vcc>3.3V</text>
        <text x=350 y=70>44</text>
        <text x=450 y=70>43</text>
        <text x=350 y=230 class=neutral>RX</text>
        <text x=450 y=230 class=neutral>TX</text>
        <text x=550 y=70>48</text>
        <text x=650 y=70>47</text>
        <text x=750 y=70>21</text>
        <text x=850 y=70>14</text>
        <text x=950 y=70>13</text>
        <text x=1050 y=70>12</text>
        <text x=1150 y=70 class=vcc>3.3V</text>
        <text x=1250 y=70 class=gnd>GND</text>
    </svg>

На роз'єм розширення Лілки виведені наступні піни:

- ``3.3V`` - живлення 3.3 Вольти
- ``GND`` - земля
- ``12``, ``13``, ``14``, ``21``, ``47``, ``48`` - GPIO
- ``TX``, ``RX`` - додатковий UART. Ці піни можна використати для з'єднання Лілки з комп'ютером через перетворювач USB-UART та навіть прошивати її через цей порт. Але ви також можете без проблем використовувати ці піни для будь-яких інших цілей в додачу до перелічених вище пінів, а для прошивання використовувати USB-порт Лілки.

Вони не використовуються Лілкою для жодних внутрішніх потреб, тому ви можете використовувати їх для будь-яких цілей.

Завдяки вбудованому мультиплексору, кожен пін можна використовувати для шин I2C, SPI, UART, або просто як вхід/вихід.

Крім того, піни ``12``, ``13`` та ``14`` під'єднані до АЦП (аналого-цифрового перетворювача, ADC):

- ``12`` - ADC2, канал 1
- ``13`` - ADC2, канал 2
- ``14`` - ADC2, канал 3


Приклад використання GPIO
-------------------------

Найпростіший приклад використання GPIO - це підключити до роз'єму розширення світлодіод та резистор, щоб світлодіод світився, коли на пін подаватиметься напруга.

Наприклад, пін ``12`` можна підключити до анода світлодіода (довший вивід), а катод світлодіода (коротший вивід) - через резистор до землі (наприклад, 100 Ом).

Обчислити значення резистора для світлодіода можна за `цим посиланням <https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-led-series-resistor>`_.

.. raw:: html

    <svg id="gpio-example" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 20 20" style="width: 100%; max-width: 400px">
        <path d="M 2.5 6 L 8 6" stroke="#00AA00" stroke-width="0.25" />
        <circle cx="2" cy="6" r="0.5" fill="none" stroke="#DD0044" stroke-width="0.25" />
        <text x="4" y="4.5" font-size="1.5" font-family="Arial">GPIO12</text>
        <path d="
            M 8 6 L 10 6
            M 12 6 L 14 6
            M 12 6 L 10 5 L 10 7 Z
            M 12 5 L 12 7
            M 12.5 5.5 L 14 4 M 13 4 L 14 4 L 14 5
            M 14 5.5 L 15.5 4 M 14.5 4 L 15.5 4 L 15.5 5
        " stroke-width="0.25" fill="none" stroke="#DD0044" />
        <text x="11" y="3" font-size="2" font-family="Arial">D1</text>
        <path d="
            M 14 6 L 16 6 L 16 8
        " stroke="#00AA00" fill="none" stroke-width="0.25" />
        <path d="
            M 16 8 L 16 9
            M 15 9 L 17 9 L 17 13 L 15 13 Z
            M 16 13 L 16 14
        " stroke="#DD0044" fill="none" stroke-width="0.25" />
        <text x="13.5" y="10" font-size="2" font-family="Arial">R1</text>
        <text x="12" y="12" font-size="1" font-family="Arial">80~150 Ом</text>
        <path d="
            M 16 14 L 16 16
        " stroke="#00AA00" fill="none" stroke-width="0.25" />
        // Ground symbol
        <path d="
            M 15 16 L 17 16 L 16 17 Z
        " stroke="#00AA00" fill="none" stroke-width="0.25" />
        <text x="16" y="19" font-size="2" font-family="Arial">GND</text>

        <defs>
            <style type="text/css">
                <![CDATA[
                #gpio-example text {
                    font-family: sans-serif;
                    text-anchor: middle;
                    dominant-baseline: middle;
                }
                ]]>
            </style>
        </defs>
    </svg>

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
