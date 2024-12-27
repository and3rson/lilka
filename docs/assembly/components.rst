Перелік деталей
===============

.. contents:: Зміст

.. |ua| image:: ./images/ua.png
   :alt: Україна
   :width: 16px
   :height: 16px

.. |ali| image:: ./images/ali.png
   :alt: AliExpress
   :width: 16px
   :height: 16px

.. |mouser| image:: ./images/mouser.png
   :alt: Mouser
   :width: 16px
   :height: 16px

.. raw:: html

    <script>
    function calculateTotal(el, colNumber) {
        var tbody = el.closest('tbody');
        var total = Array.prototype.slice.call(tbody.querySelectorAll('td:nth-child(' + colNumber + ')')).slice(0, -1).map(x => (parseFloat(x.innerText) || 0)).reduce((a, b) => a + b, 0);
        el.innerText = total;
    }
    </script>

.. _base:

Базовий комплект
----------------

.. list-table:: Базовий комплект
   :widths: 5 10 15 10 15 10 10 10 15 1
   :header-rows: 1

   * - К-сть
     - ID
     - Значення
     - Назва
     - |ua| В Україні
     - |ali| AliExpress
     - |mouser| Mouser
     - Ціна в |ua|
     - Примітка
     - Зображення

   * - 1
     - BZ1
     - Buzzer (2.54-7.62mm між контактами)
     - Passive Buzzer 
     - `mini-tech.com.ua <https://www.mini-tech.com.ua/ua/passivnyj-zummer>`__
     - `Посилання <https://www.aliexpress.com/item/1005005626151673.html>`__
     -
     - 5
     -
     - .. thumbnail:: ./images/parts/buzzer.jpg
          :group: base

   * - 3
     - C1, C2, C3
     - 1uF
     - 1uF Capacitor 
     - `m-teh.com.ua <https://m-teh.com.ua/kondensator-ct4-1uf-50v-x7r-10/>`__
     - `Посилання <https://www.aliexpress.com/item/1005004787479555.html>`__
     -
     - 9
     - Можна комплектувати `з цього набору <https://arduino.ua/prod2798-nabor-mnogosloinih-keramicheskih-kondensatorov-180-sht>`__
     - .. thumbnail:: ./images/parts/cap.jpg
          :group: base

   * - 1
     - J1
     - ~
     - USB Type-C to DIP Adapter 
     - `arduino.ua <https://arduino.ua/prod2783-plata-perehodnik-usb-type-c-na-dip-2-54>`__
     - `Посилання <https://www.aliexpress.com/item/1005006023156245.html>`__
     -
     - 12
     -
     - .. thumbnail:: ./images/parts/usb.jpg
          :group: base

   * - 1
     - J2
     - Ext
     - Pin Header
     - `arduino.ua <https://arduino.ua/prod332-konnektor-40-pin-papa>`__
     - `Посилання <https://www.aliexpress.com/item/4000873858801.html>`__
     -
     - 5
     - Варіант з AliExpress містить пари тато+мама, тому задовольняє ``J2`` та ``U2J1``
     - .. thumbnail:: ./images/parts/header.jpg
          :group: base

   * - 1
     - J3
     - uSD
     - SD Card Interface Module
     - `prom.ua <https://prom.ua/ua/p2148810072-modul-plata-mini.html>`__
     - `Посилання <https://www.aliexpress.com/item/1005005302035188.html>`__
     -
     - 24
     -
     - .. thumbnail:: ./images/parts/sd.jpg
          :group: base

   * - 1
     - R1
     - 10K
     - 10K Resistor
     - `arduino.ua <https://arduino.ua/prod1970-rezistor-10-kom-5-shtyk>`__
     - `Посилання <https://www.aliexpress.com/item/1005001627995396.html>`__
     -
     - 3
     -
     - .. thumbnail:: ./images/parts/r10k.jpg
          :group: base

   * - 1
     - R2
     - 100K
     - 100K Resistor
     - `arduino.ua <https://arduino.ua/prod1549-rezistor-100-kom-5-shtyk>`__
     - `Посилання <https://www.aliexpress.com/item/1005001627995396.html>`__
     -
     - 2
     -
     - .. thumbnail:: ./images/parts/r100k.jpg
          :group: base

   * - 1
     - SW1-SW8
     - Arrows + A/B/C/D
     - Button Kit (12mm)
     - - `arduino.ua <https://arduino.ua/prod2506-komplekt-knopok-12mm-s-kolpachkom-5-cvetov>`__
       - `imrad.com.ua (кнопки) <https://imrad.com.ua/ua/kfc-012-7-3f-9>`__
       - `imrad.com.ua (ковпачки) <https://imrad.com.ua/ua/mec12pr-9>`__
     - `Посилання <https://www.aliexpress.com/item/1005005315348507.html>`__
     -
     - 36
     - В комплекті від arduino.ua лише 6 шт
     - .. thumbnail:: ./images/parts/12mm.jpg
          :group: base

   * - 2
     - SW9, SW10
     - Select/Start
     - SMD Tactile Button
     - `imrad.com.ua <https://imrad.com.ua/ua/kfc-a06-6-knopka-taktovaya-6x6x6-6>`__
     - `Посилання <https://www.aliexpress.com/item/32912263133.html>`__
     -
     - 4
     -
     - .. thumbnail:: ./images/parts/6mm.jpg
          :group: base

   * - 1
     - SW11
     - Power
     - Toggle Switch
     - `imrad.com.ua <https://imrad.com.ua/ua/msk-05g2-0-6>`__
     - `Посилання <https://www.aliexpress.com/item/4000030382277.html>`__
     -
     - 10
     - Кнопка має бути кутова
     - .. thumbnail:: ./images/parts/toggle.jpg
          :group: base

   * - 1
     - SW12
     - Reset
     - SMD Tactile Button
     - `imrad.com.ua <https://imrad.com.ua/ua/kfc-a06-6-knopka-taktovaya-6x6x6-6>`__
     - `Посилання <https://www.aliexpress.com/item/32912263133.html>`__
     -
     - 2
     -
     - .. thumbnail:: ./images/parts/6mm.jpg
          :group: base

   * - 1
     - U1
     - ESP32-S3-WROOM-1-N16R8
     - Мікроконтролер
     - - `prom.ua <https://prom.ua/ua/p2051994816-esp32-dualcore-240mhz.html>`__
       - `rcscomponents.kiev.ua (проблеми з постачанням) <https://www.rcscomponents.kiev.ua/product/esp32-s3-wroom-1-n16r8_184448.html>`__
     - `Посилання <https://www.aliexpress.com/item/32901557709.html>`__
     - `Посилання <https://eu.mouser.com/ProductDetail/Espressif-Systems/ESP32-S3-WROOM-1-N16R8>`__
     - 200
     -
     - .. thumbnail:: ./images/parts/mcu.jpg
          :group: base

   * - 1
     - U2
     - 280x240 1.69\" TFT
     - 1.7\" IPS TFT Display (ST7789)
     - `arduino.ua <https://arduino.ua/prod6568-tft-displei-1-7-spi-240x280-rgb>`__
     - `Посилання <https://www.aliexpress.com/item/1005005933484586.html>`__
     -
     - 176
     -
     - .. thumbnail:: ./images/parts/display.jpg
          :group: base

   * - 1
     - U2J1
     - TFT_Socket
     - 40-pin Connector
     - `arduino.ua <https://arduino.ua/prod315-konnektor-40-pin-mama>`__
     - `Посилання <https://www.aliexpress.com/item/4000873858801.html>`__
     -
     - 7
     - Варіант з AliExpress містить пари тато+мама, тому задовольняє ``J2`` та ``U2J1``
     - .. thumbnail:: ./images/parts/socket.jpg
          :group: base

   * - 1
     - U3
     - TLV1117LV33DCYR (не плутати з TLV1117-33!)
     - TLV1117LV33DCYR Voltage Regulator
     - `kosmodrom.ua <https://kosmodrom.ua/ru/stabilizator-napryazheniya/tlv1117lv33dcyr.html>`__
     - `Посилання <https://www.aliexpress.com/item/1005006197085527.html>`__
     - `Посилання <https://eu.mouser.com/ProductDetail/Texas-Instruments/TLV1117LV33DCYR>`__
     - 26
     -
     - .. thumbnail:: ./images/parts/reg.jpg
          :group: base

   * - Разом
     -
     -
     -
     -
     -
     -
     - .. raw:: html

            <div id="total-base"></div>
            <script>calculateTotal(document.querySelector('#total-base'), 8);
            </script>
     -
     -

.. _battery:

Компоненти для батареї
----------------------

.. list-table:: Компоненти для батареї
   :widths: 5 10 15 10 15 10 10 10 15 1
   :header-rows: 1

   * - К-сть
     - ID
     - Значення
     - Назва
     - |ua| В Україні
     - |ali| AliExpress
     - |mouser| Mouser
     - Ціна в |ua|
     - Примітка
     - Зображення

   * - 1
     - D1
     - 1N4001
     - 1N4001 Diode
     - `voron.ua <https://voron.ua/uk/catalog/029199--diod_1n4001_v_lente_mic_master_instrument_corporation_do41_do41>`__
     - `Посилання <https://www.aliexpress.com/item/1005005945645677.html>`__
     -
     - 6
     -
     - .. thumbnail:: ./images/parts/1n4001.jpg
          :group: battery

   * - 1
     - J4
     - TP4056
     - TP4056 Charging Module
     - `arduino.ua <https://arduino.ua/prod1486-zaryadnii-modyl-tp4056-micro-usb-s-fynkciei-zashhiti-akkymylyatora>`__
     - `Посилання <https://www.aliexpress.com/item/4001196732254.html>`__
     -
     - 16
     - Бажано брати варіант з micro USB (займає менше місця, а гніздо USB Type-C вже є на платі Лілки)
     - .. thumbnail:: ./images/parts/tp4056.jpg
          :group: battery

   * - 1
     - Q1
     - IRLML6401 (SOT-23)
     - IRLML6401 MOSFET
     - - `rcscomponents.kiev.ua <https://www.rcscomponents.kiev.ua/product/irlml6401trpbf_34344.html>`__
       - `imrad.com.ua <https://imrad.com.ua/ua/irlml6401trpbf-1>`__
     - `Посилання <https://www.aliexpress.com/item/1005003078624760.html>`__
     - `Посилання <https://eu.mouser.com/ProductDetail/Infineon-Technologies/IRLML6401TRPBF>`__
     - 4
     -
     - .. thumbnail:: ./images/parts/irlml6401.jpg
          :group: battery

   * - 1
     - R3
     - 100K
     - 100K Resistor
     - `arduino.ua <https://arduino.ua/prod1549-rezistor-100-kom-5-shtyk>`__
     - `Посилання <https://www.aliexpress.com/item/1005001627995396.html>`__
     -
     - 2
     -
     - .. thumbnail:: ./images/parts/r100k.jpg
          :group: battery

   * - 1
     - R4
     - 33K
     - 33K Resistor
     - `justas-electronics.com.ua <https://justas-electronics.com.ua/rss0125w-33kOm/>`__
     - `Посилання <https://www.aliexpress.com/item/1005001627995396.html>`__
     -
     - 2.1
     -
     - .. thumbnail:: ./images/parts/r33k.jpg
          :group: battery

   * - 1
     - \-
     - Bat
     - Будь-яка LiPo на ваш розсуд
     - `m-teh.com.ua <https://m-teh.com.ua/li-pol-akumuliator-603048p-1000-ma-hod-3.7v-z-plato%D1%96u-zakhystu/?gad_source=1&gclid=CjwKCAiA29auBhBxEiwAnKcSqmJoC5UaOLX_kOIJX7G_EQOqEse5RDJBtxz8IvMHU9rLfGlj-MlgyhoCXgEQAvD_BwE>`__
     -
     -
     - 116
     - Рекомендуємо брати невелику батарею (6x30x48), щоб вона помістилась в корпус
     - .. thumbnail:: ./images/parts/lipo.jpg
          :group: battery

   * - Разом
     -
     -
     -
     -
     -
     -
     - .. raw:: html

            <div id="total-battery"></div>
            <script>calculateTotal(document.querySelector('#total-battery'), 8);
            </script>
     -
     -

.. _audio:

Компоненти для звуку
--------------------

.. list-table:: Компоненти для звуку
   :widths: 5 10 15 10 15 10 10 10 15 1
   :header-rows: 1

   * - К-сть
     - ID
     - Значення
     - Назва
     - |ua| В Україні
     - |ali| AliExpress
     - |mouser| Mouser
     - Ціна в |ua|
     - Примітка
     - Зображення

   * - 1
     - J5
     - MAX98357
     - Mono Audio Amplifier Module
     - `arduino.ua <https://arduino.ua/prod4112-modyl-aydioysilitelya-mono-3vt-klassa-d-na-max98357>`__
     - `Посилання <https://www.aliexpress.com/item/1005006711010527.html>`__
     - `Посилання <https://eu.mouser.com/ProductDetail/Adafruit/3006>`__
     - 92
     -
     - .. thumbnail:: ./images/parts/max98357.jpg
          :group: audio

   * - 1
     - \-
     - Speaker
     - 8 Ohm 1 W Speaker
     - `arduino.ua <https://arduino.ua/prod4280-dinamik-miniaturnii-8om-1-vt-30x20x4-2mm>`__
     - `Посилання <https://www.aliexpress.com/item/1005006461351585.html>`__
     -
     - 17
     - Або гніздо (див. наст. пункт), або будь-який інший динамік на ваш розсуд
     - .. thumbnail:: ./images/parts/speaker.jpg
          :group: audio

   * - 1
     - \-
     - Audio Jack (mono)
     - 3.5mm Audio Jack
     - `imrad.com.ua <https://imrad.com.ua/ua/pj3013d-144736>`__
     - `Посилання <https://www.aliexpress.com/item/1005006181883514.html>`__
     -
     - 8
     - Лише якщо ви вирішили використовувати навушники замість динаміка
     - .. thumbnail:: ./images/parts/jack.jpg
          :group: audio

   * - Разом
     -
     -
     -
     -
     -
     -
     - .. raw:: html

            <div id="total-audio"></div>
            <script>calculateTotal(document.querySelector('#total-audio'), 8);
            </script>
     -
     -

.. _other:

Не входять в жодний комплект
----------------------------

- MicroSD карта
- USB Type-C кабель
