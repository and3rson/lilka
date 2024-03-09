#include "user_spi.h"

UserSPIApp::UserSPIApp() : App("SPI") {}

void UserSPIApp::run() {
#ifdef SPI2_NUM
    lilka::Canvas buffer(canvas->width(), canvas->height());
    buffer.begin();
    buffer.fillScreen(0);

    buffer.fillScreen(buffer.color565(0, 0, 0));
    buffer.setTextBound(4, 0, canvas->width() - 8, canvas->height());
    buffer.setCursor(4, 48);
    buffer.println("SPI2 begin");
    canvas->drawCanvas(&buffer);
    queueDraw();

    lilka::SPI2.begin(45, 48, 47);

    vTaskDelay(500 / portTICK_PERIOD_MS);

    buffer.println("beginTransaction");
    buffer.println("  SPI mode: 0");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    buffer.println("  write: 0xAA");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.write(0xAA);
    buffer.println("endTransaction");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.endTransaction();

    vTaskDelay(10 / portTICK_PERIOD_MS);

    buffer.println("beginTransaction");
    buffer.println("  SPI mode: 3");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
    buffer.println("  SPI mode: 0xAA");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.write(0xAA);
    buffer.println("endTransaction");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.endTransaction();

    vTaskDelay(500 / portTICK_PERIOD_MS);

    buffer.println("SPI2 end");
    canvas->drawCanvas(&buffer);
    queueDraw();
    lilka::SPI2.end();

    buffer.println("Press A to exit");
    canvas->drawCanvas(&buffer);
    queueDraw();
    while (!lilka::controller.getState().a.justPressed) {
        taskYIELD();
    }
#endif
}
