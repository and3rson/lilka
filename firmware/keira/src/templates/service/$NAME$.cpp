#include "$NAME$.h"

$NAME$Service::$NAME$Service() : Service("$NAME$") {
}

void $NAME$Service::run() {
    while (1) {
        // Service loop

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
