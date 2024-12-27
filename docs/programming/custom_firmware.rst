Створення власної прошивки
==========================

1. Відкрийте Visual Studio Code, перейдіть на вкладку ``PlatformIO`` та виберіть ``Create New Project``.

   .. image:: ./images/custom_firmware/02_pio_new_proj_2.png
      :width: 60%

   У вікні, що відкриється, натисніть на кнопку ``New Project``.

2. Придумайте назву свого проєкту - наприклад, ``first_project``.

   У переліку boards оберіть ``Lilka v2 (Anderson & friends)``.

   У переліку frameworks оберіть ``Arduino``.

   .. image:: ./images/custom_firmware/03_pio_new_proj_sel_lilka.png
      :width: 60%

   Після цього натисніть на кнопку ``Finish``.

   .. note::

      Якщо ви не бачите в переліку ``Lilka v2 (Anderson & friends)`` або у вас виникає помилка при створенні проєкту, вам потрібно оновити пакети PlatformIO.

      Для цього виконайте команду ``pio pkg update -g -p espressif32`` в терміналі Visual Studio Code.

      Щоб відкрити термінал, в панелі ``Quick Access`` виберіть ``Miscellaneous`` і тоді натисніть на ``PlatformIO Core CLI``.
      Консоль відкриється внизу вікна Visual Studio Code.

      Після цього вам слід перезапустити Visual Studio Code.

3. Ваш новий проєкт відкриється у новому вікні Visual Studio Code, і ви побачите файл ``platformio.ini``.

   .. image:: ./images/custom_firmware/04_project_ready.png
      :width: 60%

   Це - конфігураційний файл проєкту, в якому вказані всі налаштування проєкту: платформа, фреймворк, бібліотеки тощо.

   Ми можете редагувати його вручну, а можете використовувати графічний інтерфейс PlatformIO.

4. Давайте додамо до проєкту бібліотеку :ref:`lilka <lilka-lib>`. Для цього відкрийте вкладку ``PlatformIO`` та в панелі ``Quick Access`` виберіть ``Libraries``.

   Після цього введіть у поле пошуку ``lilka``:

   .. image:: ./images/custom_firmware/05_install_lilka_lib.png
      :width: 60%

   Тепер натисніть на кнопку ``Add to Project``.

   .. image:: ./images/custom_firmware/06_install_lilka_lib.png
      :width: 60%

   Після завершення завантаження бібліотеки ви побачите, що вона з'явилась у полі ``lib_deps`` файлу ``platformio.ini``.

   .. image:: ./images/custom_firmware/08_lilka_lib_installed.png
      :width: 60%

   Це поле вказує на те, які бібліотеки використовуються у проєкті. PlatformIO автоматично завантажує та встановлює всі бібліотеки, вказані у цьому полі, а також їх залежності.

   Встановлення бібліотеки ``lilka`` автоматично встановлює різні бібліотеки для роботи з Лілкою, наприклад ``Arduino-GFX``, яка використовується для роботи з дисплеєм Лілки.

5. Напишемо простий код нашої прошивки. Відкрийте файл ``src/main.cpp`` та напишіть наступний код:

   .. code-block:: cpp

      #include <lilka.h>

      void setup() {
        // Ця функція виконається один раз при увімкненні

        // Ініціалізуємо дисплей, карту пам'яті, звук, кнопки і все на світі
        lilka::begin();

        // Заповнюємо екран білим кольором
        lilka::display.fillScreen(lilka::colors::White);
      }

      void loop() {
        // Ця функція буде виконуватись по колу

        // Читаємо стан кнопок
        lilka::State state = lilka::controller.getState();

        if (state.a.justPressed) {
          // Кнопка A щойно була натиснена
          lilka::display.fillScreen(lilka::colors::Red);
          lilka::buzzer.play(lilka::NOTE_C4);
        } else if (state.a.justReleased) {
          // Кнопка A щойно буда відпущена
          lilka::display.fillScreen(lilka::colors::Green);
          lilka::buzzer.stop();
        }  
      }

   Це - проста програма, яка при натисканні на кнопку A змінює колір екрану на червоний та відтворює звук, а при відпусканні - заповнює екран зеленим кольором та зупиняє звук.

   .. image:: ./images/custom_firmware/10_simple_program.png
      :width: 60%

6. Тепер ми можемо скомпілювати нашу прошивку та завантажити її в Лілку.

   Для цього під'єднайте Лілку до комп'ютера за допомогою USB-кабеля, відкрийте вкладку ``PlatformIO`` та натисніть на дію ``Upload``.

   .. image:: ./images/custom_firmware/12_compiling.png
      :width: 60%

   Після завершення компіляції та завантаження прошивки ви побачите, як Лілка виконує вашу програму. Спробуйте натиснути та відпустити кнопку A та переконайтеся, що все працює!
