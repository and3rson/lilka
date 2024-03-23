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
     - Назва
     - Посилання
     - Ціна (разом)
     - Примітка

   * - 1
     - BZ1
     - Buzzer (2.54mm між контактами)
     - Passive Buzzer 
     - `mini-tech.com.ua <https://www.mini-tech.com.ua/ua/passivnyj-zummer>`__
     - 5
     -

   * - 3
     - C1, C2, C3
     - 1uF
     - 1uF Capacitor 
     - `m-teh.com.ua <https://m-teh.com.ua/kondensator-ct4-1uf-50v-x7r-10/>`__
     - 9
     - Можна комплектувати `з цього набору <https://arduino.ua/prod2798-nabor-mnogosloinih-keramicheskih-kondensatorov-180-sht>`__

   * - 1
     - J1
     - ~
     - USB Type-C to DIP Adapter 
     - `arduino.ua <https://arduino.ua/prod2783-plata-perehodnik-usb-type-c-na-dip-2-54>`__
     - 12
     -

   * - 1
     - J4
     - Ext
     - Pin Header
     - `arduino.ua <https://arduino.ua/prod332-konnektor-40-pin-papa>`__
     - 5
     -

   * - 1
     - J5
     - uSD
     - SD Card Interface Module
     - `diyshop.com.ua <https://diyshop.com.ua/en/modul-interfejsa-mini-sd-karty-kardrider>`__
     - 24
     -

   * - 1
     - R1
     - 10K
     - 10K Resistor
     - `arduino.ua <https://arduino.ua/prod1970-rezistor-10-kom-5-shtyk>`__
     - 3
     - 5 шт в комплекті

   * - 2
     - R2, R3
     - 100K
     - 100K Resistor
     - `arduino.ua <https://arduino.ua/prod1549-rezistor-100-kom-5-shtyk>`__
     - 2
     - 5 шт в комплекті

   * - 1
     - SW1-SW8
     - Arrows + A/B/C/D
     - Button Kit (12mm)
     - - `arduino.ua <https://arduino.ua/prod2506-komplekt-knopok-12mm-s-kolpachkom-5-cvetov>`__
       - `imrad.com.ua (кнопки) <https://imrad.com.ua/ua/kfc-012-7-3f-9>`__, `imrad.com.ua (ковпачки) <https://imrad.com.ua/ua/mec12pr-9>`__
     - 36
     - В комплекті від arduino.ua лише 6 шт

   * - 2
     - SW9, SW10
     - Select/Start
     - SMD Tactile Button
     - `imrad.com.ua <https://imrad.com.ua/ua/kfc-a06-6-knopka-taktovaya-6x6x6-6>`__
     - 4
     -

   * - 1
     - SW11
     - Power
     - Toggle Switch
     - `arduino.ua <https://arduino.ua/prod5124-perekluchatel-polzynkovii-ms-22d18g2-dip>`__
     - 10
     -

   * - 1
     - SW12
     - Reset
     - SMD Tactile Button
     - `imrad.com.ua <https://imrad.com.ua/ua/kfc-a06-6-knopka-taktovaya-6x6x6-6>`__
     - 2
     -

   * - 1
     - U1
     - ESP32-S3-WROOM-1-N16R8
     - ESP32-S3-WROOM-1-N16R8 Module
     - - `prom.ua <https://prom.ua/ua/p2051994816-esp32-dualcore-240mhz.html>`__
       - `rcscomponents.kiev.ua (проблеми з постачанням) <https://www.rcscomponents.kiev.ua/product/esp32-s3-wroom-1-n16r8_184448.html>`__
     - 200
     -

   * - 1
     - U2
     - 280x240 1.69\" TFT
     - 1.7\" TFT Display (ST7789)
     - `arduino.ua <https://arduino.ua/prod6568-tft-displei-1-7-spi-240x280-rgb>`__
     - 176
     -

   * - 1
     - U2J1
     - TFT_Socket
     - 40-pin Connector
     - `arduino.ua <https://arduino.ua/prod315-konnektor-40-pin-mama>`__
     - 7
     -

   * - 1
     - U3
     - TLV1117LV33DCYR (не плутати з TLV1117-33!)
     - TLV1117LV33DCYR Voltage Regulator
     - `kosmodrom.ua <https://kosmodrom.ua/ru/stabilizator-napryazheniya/tlv1117lv33dcyr.html>`__
     - 26
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
     - Назва
     - Посилання
     - Ціна (разом)
     - Примітка

   * - 1
     - D1
     - 1N4001
     - 1N4001 Diode
     - `voron.ua <https://voron.ua/uk/catalog/029199--diod_1n4001_v_lente_mic_master_instrument_corporation_do41_do41>`__
     - 6
     - 10 шт в комплекті

   * - 1
     - J2, J3
     - Bat
     - Будь-яка LiPo на ваш розсуд
     - `m-teh.com.ua <https://m-teh.com.ua/li-pol-akumuliator-603048p-1000-ma-hod-3.7v-z-plato%D1%96u-zakhystu/?gad_source=1&gclid=CjwKCAiA29auBhBxEiwAnKcSqmJoC5UaOLX_kOIJX7G_EQOqEse5RDJBtxz8IvMHU9rLfGlj-MlgyhoCXgEQAvD_BwE>`__
     - 116
     -

   * - 1
     - J8
     - TP4056
     - TP4056 Charging Module
     - `arduino.ua <https://arduino.ua/prod1486-zaryadnii-modyl-tp4056-micro-usb-s-fynkciei-zashhiti-akkymylyatora>`__
     - 16
     -

   * - 1
     - Q1
     - IRLML6401 (SOT-23)
     - IRLML6401 MOSFET
     - `rcscomponents.kiev.ua <https://www.rcscomponents.kiev.ua/product/irlml6401trpbf_34344.html>`__

       `imrad.com.ua <https://imrad.com.ua/ua/irlml6401trpbf-1>`__
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
     - 33K Resistor
     - `justas-electronics.com.ua <https://justas-electronics.com.ua/rss0125w-33kOm/>`__
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
     - Назва
     - Посилання
     - Ціна (разом)
     - Примітка

   * - 1
     - J6
     - PCM5102A
     - PCM5102A Audio Module
     - `prom.ua <https://prom.ua/ua/p1401452703-pcm5102a-modul-tsap.html>`__
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
     - Mono Audio Amplifier Module
     - `arduino.ua <https://arduino.ua/prod4112-modyl-aydioysilitelya-mono-3vt-klassa-d-na-max98357>`__
     - 92
     -
