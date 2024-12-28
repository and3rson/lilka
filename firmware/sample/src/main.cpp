#include <lilka.h>

void setup() {
    lilka::begin();
}

void loop() {
    lilka::Alert alert1("Лілка", "Це - тестове повідомлення.");
    alert1.draw(&lilka::display);
    while (!alert1.isFinished()) {
        alert1.update();
    }

    lilka::Alert alert2("Привіт!", "А це - ще одне тестове повідомлення.");
    alert2.draw(&lilka::display);
    while (!alert2.isFinished()) {
        alert2.update();
    }
}
