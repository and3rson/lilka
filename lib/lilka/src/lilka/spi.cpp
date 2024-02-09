#include "config.h"
#include "spi.h"

namespace lilka {

SPIClass SPI1(SPI1_NUM); // Системний SPI (Дисплей та SD-карта)
#ifdef SPI2_NUM
SPIClass SPI2(SPI2_NUM); // Користувацький SPI
#endif

void spi_begin() {
    SPI1.begin(LILKA_SPI_SCK, LILKA_SPI_MISO, LILKA_SPI_MOSI);
}

} // namespace lilka
