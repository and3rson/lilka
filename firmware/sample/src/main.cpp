#include <lilka.h>

void setup() {
    lilka::begin();
}

void loop() {
    lilka::ui_alert("Привіт!", "Це - тестове повідомлення.");
    lilka::ui_alert("Привіт!", "А це - ще одне тестове повідомлення.");
}
