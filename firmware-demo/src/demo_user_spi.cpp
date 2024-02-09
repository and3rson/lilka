#include <lilka.h>

void demo_user_spi() {
#ifdef SPI2_NUM
    lilka::display.fillScreen(lilka::display.color565(0, 0, 0));
    lilka::display.setTextBound(4, 0, LILKA_DISPLAY_WIDTH - 8, LILKA_DISPLAY_HEIGHT);
    lilka::display.setCursor(4, 48);

    lilka::display.println("SPI2 begin");
    lilka::SPI2.begin(45, 48, 47);

    delay(500);

    lilka::display.println("beginTransaction");
    lilka::display.println("  SPI mode: 0");
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
    lilka::display.println("  write: 0xAA");
    lilka::SPI2.write(0xAA);
    lilka::display.println("endTransaction");
    lilka::SPI2.endTransaction();

    delay(10);

    lilka::display.println("beginTransaction");
    lilka::display.println("  SPI mode: 3");
    lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE3));
    lilka::display.println("  SPI mode: 0xAA");
    lilka::SPI2.write(0xAA);
    lilka::display.println("endTransaction");
    lilka::SPI2.endTransaction();

    delay(500);

    lilka::display.println("SPI2 end");
    lilka::SPI2.end();

    lilka::display.println("Press start to exit");
    while (!lilka::controller.getState().start.justPressed) {
        delay(10);
    }
#else
    lilka::ui_alert("Помилка", "SPI2 не підтримується");
#endif
}
