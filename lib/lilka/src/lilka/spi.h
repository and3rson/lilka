#ifndef LILKA_SPI_H
#define LILKA_SPI_H

#include <SPI.h>

#if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
#    define SPI1_NUM HSPI
#    define SPI2_NUM FSPI
extern SPIClass SPI1;
extern SPIClass SPI2;
#elif CONFIG_IDF_TARGET_ESP32C3
#    define SPI1_NUM FSPI
extern SPIClass SPI1;
#endif

// #if CONFIG_IDF_TARGET_ESP32S2 || CONFIG_IDF_TARGET_ESP32S3
// #define VSPI FSPI
// #endif
//
// SPIClass SPI1(VSPI);
// SPIClass SPI2(HSPI);

#endif // LILKA_SPI_H
