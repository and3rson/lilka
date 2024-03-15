Перелік деталей
===============

.. warning:: Ця документація знаходиться в розробці. Інформація є неповною, може бути недостовірною і/або застарілою, і може значно змінюватися в майбутньому.

.. raw:: html

    <script>
    function calculateTotal(el, colNumber) {
        var tbody = el.closest('tbody');
        var total = Array.prototype.slice.call(tbody.querySelectorAll('td:nth-child(' + colNumber + ')')).slice(0, -1).map(x => (parseFloat(x.innerText) || 0)).reduce((a, b) => a + b, 0);
        el.innerText = total;
    }
    </script>

.. list-table:: Базовий комплект
   :widths: 5 10 25 25 15 5 15
   :header-rows: 1

   * - К-сть
     - Ідентифікатор
     - Значення
     - Посилання
     - Постачальник
     - Ціна (разом)
     - Примітка

   * - 1
     - BZ1
     - Buzzer (2.54mm між контактами)
     - `Passive Buzzer <https://www.mini-tech.com.ua/ua/passivnyj-zummer>`__
     - mini-tech.com.ua
     - 5
     -

   * - 3
     - C1, C2, C3
     - 1uF
     - `1uF Capacitor <https://m-teh.com.ua/kondensator-ct4-1uf-50v-x7r-10/>`__
     - m-teh.com.ua
     - 9
     - Можна комплектувати `з цього набору <https://arduino.ua/prod2798-nabor-mnogosloinih-keramicheskih-kondensatorov-180-sht>`__

   * - 1
     - J1
     - ~
     - `USB Type-C to DIP Adapter <https://arduino.ua/prod2783-plata-perehodnik-usb-type-c-na-dip-2-54>`__
     - arduino.ua
     - 12
     -

   * - 1
     - J3
     - Ext
     - `Dual Row Connector <https://arduino.ua/prod1077-konnektor-dvyhryadnii-2h40-pin-papa>`__
     - arduino.ua
     - 9
     -

   * - 1
     - J4
     - UART
     - `Dual Row Connector <https://arduino.ua/prod1077-konnektor-dvyhryadnii-2h40-pin-papa>`__
     - arduino.ua
     - 0
     - Та саме деталь, що й J4

   * - 1
     - J5
     - uSD
     - `SD Card Interface Module <https://diyshop.com.ua/en/modul-interfejsa-mini-sd-karty-kardrider>`__
     - diyshop.com.ua
     - 24
     -

   * - 1
     - R1
     - 10K
     - `10K Resistor <https://arduino.ua/prod1970-rezistor-10-kom-5-shtyk>`__
     - arduino.ua
     - 3
     - 5 шт в комплекті

   * - 2
     - R2, R5
     - 100K
     - `100K Resistor <https://arduino.ua/prod1549-rezistor-100-kom-5-shtyk>`__
     - arduino.ua
     - 2
     - 5 шт в комплекті

   * - 1
     - R3
     - 200
     - `200 Ohm Resistor <https://arduino.ua/prod339-rezistor-200-om-5-shtyk>`__
     - arduino.ua
     - 2
     - 5 шт в комплекті

   * - 1
     - SW1-SW8
     - Arrows + A/B/C/D
     - `Button Kit (12mm) <https://arduino.ua/prod2506-komplekt-knopok-12mm-s-kolpachkom-5-cvetov>`__
     - arduino.ua
     - 27
     - Потрібно ще 2 (в комплекті лише 6)

   * - 2
     - SW9, SW10
     - Select/Start
     - `SMD Tactile Button <https://arduino.ua/prod6165-taktova-knopka-smd-4pin-6h6x9-5mm-shtok-6-0mm>`__
     - arduino.ua
     - 4
     -

   * - 1
     - SW11
     - Power
     - `Toggle Switch <https://arduino.ua/prod5124-perekluchatel-polzynkovii-ms-22d18g2-dip>`__
     - arduino.ua
     - 10
     -

   * - 1
     - SW12
     - Reset
     - `SMD Tactile Button <https://arduino.ua/prod6165-taktova-knopka-smd-4pin-6h6x9-5mm-shtok-6-0mm>`__
     - arduino.ua
     - 2
     -

   * - 1
     - U2
     - 280x240 1.69\" TFT
     - `1.7" TFT Display <https://arduino.ua/prod6568-tft-displei-1-7-spi-240x280-rgb>`__
     - arduino.ua
     - 176
     -

   * - 1
     - U2J1
     - TFT_Socket
     - `40-pin Connector <https://arduino.ua/prod315-konnektor-40-pin-mama>`__
     - arduino.ua
     - 7
     -

   * - 1
     - U3
     - TLV1117LV33DCYR (не плутати з TLV1117-33!)
     - `TLV1117LV33DCYR Voltage Regulator <https://kosmodrom.ua/ru/stabilizator-napryazheniya/tlv1117lv33dcyr.html>`__
     - kosmodrom.ua
     - 26
     -

   * - 1
     - U4
     - ESP32-S3-WROOM-1-N16R8
     - `ESP32-S3-WROOM-1-N16R8 Module <https://www.rcscomponents.kiev.ua/product/esp32-s3-wroom-1-n16r8_184448.html>`__
     - rcscomponents.kiev.ua
     - 253
     -

   * - Разом
     -
     -
     -
     -
     - .. raw:: html

            <div id="total-base"></div>
            <script>calculateTotal(document.querySelector('#total-base'), 6);
            </script>
     -


.. list-table:: Компоненти для батареї
   :widths: 5 15 15 25 15 10 15
   :header-rows: 1

   * - К-сть
     - Ідентифікатор
     - Значення
     - Посилання
     - Постачальник
     - Ціна (разом)
     - Примітка

   * - 1
     - D1
     - 1N4001
     - `1N4001 Diode <https://voron.ua/uk/catalog/029199--diod_1n4001_v_lente_mic_master_instrument_corporation_do41_do41>`__
     - voron.ua
     - 6
     - 10 шт в комплекті

   * - 1
     - J2
     - Bat
     - `Будь-яка LiPo на ваш розсуд <https://m-teh.com.ua/li-pol-akumuliator-603048p-1000-ma-hod-3.7v-z-plato%D1%96u-zakhystu/?gad_source=1&gclid=CjwKCAiA29auBhBxEiwAnKcSqmJoC5UaOLX_kOIJX7G_EQOqEse5RDJBtxz8IvMHU9rLfGlj-MlgyhoCXgEQAvD_BwE>`__
     - m-teh.com.ua
     - 116
     -

   * - 1
     - J8
     - TP4056
     - `TP4056 Charging Module <https://arduino.ua/prod1486-zaryadnii-modyl-tp4056-micro-usb-s-fynkciei-zashhiti-akkymylyatora>`__
     - arduino.ua
     - 16
     -

   * - 1
     - Q1
     - IRLML6401 (SOT-23)
     - `IRLML6401 MOSFET <https://www.rcscomponents.kiev.ua/product/irlml6401trpbf_34344.html>`__
     - rcscomponents.kiev.ua
     - 4
     -

   * - 1
     - Q2
     - TP2104 *
     -
     -
     -
     - \* Запасне місце для THT-альтернативи Q1

   * - 1
     - R4
     - 33K
     - `33K Resistor <https://justas-electronics.com.ua/rss0125w-33kOm/>`__
     - justas-electronics.com.ua
     - 2.1
     - 10 шт в комплекті

   * - Разом
     -
     -
     -
     -
     - .. raw:: html

            <div id="total-battery"></div>
            <script>calculateTotal(document.querySelector('#total-battery'), 6);
            </script>
     -

.. list-table:: Компоненти для звуку
   :widths: 5 15 15 25 15 10 15
   :header-rows: 1

   * - К-сть
     - Ідентифікатор
     - Значення
     - Посилання
     - Постачальник
     - Ціна (разом)
     - Примітка

   * - 1
     - J6
     - PCM5102A
     - `PCM5102A Audio Module <https://prom.ua/ua/p1401452703-pcm5102a-modul-tsap.html>`__
     - prom.ua
     - 299
     -

   * -
     -
     -
     - АБО
     -
     -
     -

   * - 1
     - J7
     - MAX98357
     - `Mono Audio Amplifier Module <https://arduino.ua/prod4112-modyl-aydioysilitelya-mono-3vt-klassa-d-na-max98357>`__
     - arduino.ua
     - 92
     -

   * - Разом
     -
     -
     -
     -
     - .. raw:: html

            <div id="total-sound"></div>
            <script>calculateTotal(document.querySelector('#total-sound'), 6);
            </script>
     -
