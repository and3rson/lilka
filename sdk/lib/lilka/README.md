# Lilka

Бібліотека для роботи з компонентами [Лілки](https://lilka.readthedocs.io/).

# Приклад використання

```cpp
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
```
