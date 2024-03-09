#ifndef LILKA_SPI_H
#define LILKA_SPI_H

#include <SPI.h>

namespace lilka {

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// Лілка v2+
#    define SPI1_NUM HSPI
#    define SPI2_NUM FSPI
extern SPIClass SPI1; // Системний SPI (Дисплей та SD-карта)
extern SPIClass SPI2; // Користувацький SPI

#elif CONFIG_IDF_TARGET_ESP32C3
// Лілка v0 і v1
#    define SPI1_NUM FSPI
extern SPIClass SPI1; // Системний SPI (Дисплей та SD-карта)

#endif

extern void spi_begin();

} // namespace lilka

#endif // LILKA_SPI_H
