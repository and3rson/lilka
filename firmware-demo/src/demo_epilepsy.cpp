#include <lilka.h>

void demo_epilepsy() {
    while (1) {
        lilka::display.fillScreen(random(0, 0xFFFF));
        if (lilka::controller.getState().start.justPressed) {
            return;
        }
    }
}
