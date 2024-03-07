#include <lilka.h>

void demo_user_spi(lilka::Canvas *canvas) {
#ifdef SPI2_NUM
    canvas->fillScreen(canvas->color565(0, 0, 0));
    canvas->setTextBound(4, 0, LILKA_DISPLAY_WIDTH - 8, LILKA_DISPLAY_HEIGHT);
    canvas->setCursor(4, 48);

    canvas->println("SPI2 begin");
    lilka::SPI2.begin(45, 48, 47);

    delay(500);

    canvas->println("beginTransaction");
    canvas->println("  SPI mode: 0");
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    canvas->println("  write: 0xAA");
    lilka::SPI2.write(0xAA);
    canvas->println("endTransaction");
    lilka::SPI2.endTransaction();

    delay(10);

    canvas->println("beginTransaction");
    canvas->println("  SPI mode: 3");
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
    canvas->println("  SPI mode: 0xAA");
    lilka::SPI2.write(0xAA);
    canvas->println("endTransaction");
    lilka::SPI2.endTransaction();

    delay(500);

    canvas->println("SPI2 end");
    lilka::SPI2.end();

    canvas->println("Press A to exit");
    while (!lilka::controller.getState().a.justPressed) {
        delay(10);
    }
#else
    lilka::ui_alert("Помилка", "SPI2 не підтримується");
#endif
}
