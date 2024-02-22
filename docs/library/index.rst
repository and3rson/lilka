Бібліотека ``lilka``
====================

Проєкт "Лілка" має однойменну бібліотеку ``lilka``, яка спрощує роботу з Лілкою та дозволяє вам швидко створювати власні прошивки для неї.

Ця бібліотека доступна в `реєстрі бібліотек PlatformIO <https://registry.platformio.org/libraries/and3rson/Lilka/installation>`_.

Звісно ж, ви можете використовувати Лілку і без бібліотеки ``lilka`` - ніхто не змушує вас використовувати її.
Весь код, який ви побачите в цій документації, можна написати самостійно, без використання ``lilka``.

Крім того, ви без проблем можете використовувати для роботи з Лілкою лише функції `Arduino <https://www.arduino.cc/reference/en/>`_, `ESP-IDF <https://docs.espressif.com/projects/esp-idf/en/v5.2/esp32s3/api-reference/index.html>`_ та різні сторонні бібліотеки.

Проте бібліолека ``lilka`` допоможе зробити ваші програми простішими та більш `"portable" (переносними) <https://uk.wikipedia.org/wiki/%D0%9F%D0%B5%D1%80%D0%B5%D0%BD%D0%BE%D1%81%D0%BD%D1%96%D1%81%D1%82%D1%8C_%D0%BF%D1%80%D0%BE%D0%B3%D1%80%D0%B0%D0%BC%D0%BD%D0%BE%D0%B3%D0%BE_%D0%B7%D0%B0%D0%B1%D0%B5%D0%B7%D0%BF%D0%B5%D1%87%D0%B5%D0%BD%D0%BD%D1%8F>`_, а також зменшить кількість помилок при розробці.

Також ``lilka`` спрощує ініціалізацію дисплею, кнопок, батареї, звуку, SD-карти та інших компонентів, що входять до складу Лілки. Для цього потрібно викликати лише одну функцію - ``lilka::begin()``.

.. code-block:: cpp
    :linenos:
    :emphasize-lines: 1,4

    #include <lilka.h>

    void setup() {
        lilka::begin();
        // Все залізо готове до роботи!
    }

    void loop() {
        // Заповнити екран чорним кольором
        lilka::display.fillScreen(lilka::display.color565(0, 255, 0));

        while (1) {
            // Отримати стан кнопок
            lilka::State state = lilka::controller.getState();

            if (state.a.justPressed) { // Якщо щойно була натиснута кнопка "A"...
                // Розпочати відтворення звуку на частоті 440 Гц
                lilka::buzzer.play(440);
                // Заповнити екран червоним кольором
                lilka::display.fillScreen(lilka::display.color565(255, 0, 0));
            } else if (state.a.justReleased) { // Якщо кнопка "A" щойно була відпущена...
                // Зупинити відтворення звуку
                lilka::buzzer.stop();
                // Заповнити екран зеленим кольором
                lilka::display.fillScreen(lilka::display.color565(0, 255, 0));
            }
        }
    }

Тому, незалежно від рівня вашої кваліфікації, ми радимо використовувати бібліолеку ``lilka`` для роботи з Лілкою.

.. note::

    Порада для початківців: ви могли помітити, що всі функції та об'єкти бібліотеки ``lilka`` викликаються через ``::``. Це означає, що вони належать до `простору назв <https://uk.wikipedia.org/wiki/%D0%9F%D1%80%D0%BE%D1%81%D1%82%D1%96%D1%80_%D0%BD%D0%B0%D0%B7%D0%B2>`_ ``lilka``.

    Такий синтаксис дозволяє нам уникнути плутанини з іншими бібліотеками (а також з вашим кодом), які можуть містити функції чи змінні з такими ж назвами.

    Якщо не хочете щоразу писати ``lilka::`` перед кожною функцією та об'єктом бібліотеки ``lilka``, ви можете використати директиву ``using namespace``:

    .. code-block:: cpp
        :linenos:
        :emphasize-lines: 3,12,16,20,22,25,27

        #include <lilka.h>

        using namespace lilka; // Тепер не потрібно щоразу писати "lilka::"

        void setup() {
            begin();
            // Все залізо готове до роботи!
        }

        void loop() {
            // Заповнити екран чорним кольором
            display.fillScreen(display.color565(0, 255, 0));

            while (1) {
                // Отримати стан кнопок
                State state = controller.getState();

                if (state.a.justPressed) { // Якщо щойно була натиснута кнопка "A"...
                    // Розпочати відтворення звуку на частоті 440 Гц
                    buzzer.play(440);
                    // Заповнити екран червоним кольором
                    display.fillScreen(display.color565(255, 0, 0));
                } else if (state.a.justReleased) { // Якщо кнопка "A" щойно була відпущена...
                    // Зупинити відтворення звуку
                    buzzer.stop();
                    // Заповнити екран зеленим кольором
                    display.fillScreen(display.color565(0, 255, 0));
                }
            }
        }

.. toctree::
    :maxdepth: 1

    battery
    buzzer
    controller
    display
    loader
    spi
    ui
