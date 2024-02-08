#include <lilka.h>

void demo_epilepsy() {
    while (lilka::controller.state().start) {
    };
    while (1) {
        lilka::display.fillScreen(random(0, 0xFFFF));
        if (lilka::controller.state().start) {
            return;
        }
    }
}
