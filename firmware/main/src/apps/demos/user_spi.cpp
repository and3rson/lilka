#include "user_spi.h"

UserSPIApp::UserSPIApp() : App("SPI") {}

void UserSPIApp::run() {
#ifdef SPI2_NUM
    canvas->fillScreen(canvas->color565(0, 0, 0));
    canvas->setTextBound(4, 0, LILKA_DISPLAY_WIDTH - 8, LILKA_DISPLAY_HEIGHT);
    canvas->setCursor(4, 48);
    canvas->println("SPI2 begin");
    queueDraw();

    lilka::SPI2.begin(45, 48, 47);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    canvas->println("beginTransaction");
    canvas->println("  SPI mode: 0");
    queueDraw();
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    canvas->println("  write: 0xAA");
    queueDraw();
    lilka::SPI2.write(0xAA);
    canvas->println("endTransaction");
    queueDraw();
    lilka::SPI2.endTransaction();

    vTaskDelay(10 / portTICK_PERIOD_MS);

    canvas->println("beginTransaction");
    canvas->println("  SPI mode: 3");
    queueDraw();
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
    canvas->println("  SPI mode: 0xAA");
    queueDraw();
    lilka::SPI2.write(0xAA);
    canvas->println("endTransaction");
    queueDraw();
    lilka::SPI2.endTransaction();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    canvas->println("SPI2 end");
    queueDraw();
    lilka::SPI2.end();

    canvas->println("Press A to exit");
    queueDraw();
    while (!lilka::controller.getState().a.justPressed) {
        taskYIELD();
    }
#endif
}
