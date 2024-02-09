#ifndef LILKA_CONFIG_H
#define LILKA_CONFIG_H

#if LILKA_VERSION == -1

// Lilka on a breadboard (C3)

#    define LILKA_GPIO_UP 4
#    define LILKA_GPIO_DOWN 7
#    define LILKA_GPIO_LEFT -1
#    define LILKA_GPIO_RIGHT -1
#    define LILKA_GPIO_SELECT -1
#    define LILKA_GPIO_START 10
#    define LILKA_GPIO_A -1
#    define LILKA_GPIO_B -1

#    define LILKA_SPI_SCK 0
#    define LILKA_SPI_MOSI 1
#    define LILKA_SPI_MISO 8
#    define LILKA_DISPLAY_DC 3
#    define LILKA_DISPLAY_CS 21
#    define LILKA_DISPLAY_RST 2
#    define LILKA_DISPLAY_ROTATION 0
#    define LILKA_DISPLAY_WIDTH 240
#    define LILKA_DISPLAY_HEIGHT 280

#    define LILKA_SDCARD_CS 20

#elif LILKA_VERSION == 1

// Lilka v1 (C3), first prototype from pcb24.com.ua

#    define LILKA_GPIO_UP 4
#    define LILKA_GPIO_DOWN 7
#    define LILKA_GPIO_LEFT 5
#    define LILKA_GPIO_RIGHT 6
#    define LILKA_GPIO_SELECT 9
#    define LILKA_GPIO_START 10
#    define LILKA_GPIO_A 20
#    define LILKA_GPIO_B 21
#    define LILKA_GPIO_B -1

#    define LILKA_DISPLAY_DC 2
#    define LILKA_DISPLAY_CS 3
#    define LILKA_DISPLAY_SCK 8
#    define LILKA_DISPLAY_MOSI 1
#    define LILKA_DISPLAY_RST -1
#    define LILKA_DISPLAY_ROTATION 0
#    define LILKA_DISPLAY_WIDTH 240
#    define LILKA_DISPLAY_HEIGHT 280

#    define SDCARD_CS -1

#elif LILKA_VERSION == 2

// Lilka v2 (S3)

#    define LILKA_GPIO_UP 4
#    define LILKA_GPIO_DOWN 7
#    define LILKA_GPIO_LEFT 5
#    define LILKA_GPIO_RIGHT 6
#    define LILKA_GPIO_SELECT 18
#    define LILKA_GPIO_START 0
#    define LILKA_GPIO_A 14
#    define LILKA_GPIO_B 13

#    define LILKA_SPI_SCK 42
#    define LILKA_SPI_MOSI 41
#    define LILKA_SPI_MISO 2 // Was 40
#    define LILKA_DISPLAY_DC 39
#    define LILKA_DISPLAY_CS 38
#    define LILKA_DISPLAY_RST -1
#    define LILKA_DISPLAY_ROTATION 0
#    define LILKA_DISPLAY_WIDTH 240
#    define LILKA_DISPLAY_HEIGHT 280

#    define LILKA_SDCARD_CS 40 // Was 2

#    define P3 1 // ADC1, CH0
#    define P4 8
#    define P5 9
#    define P6 10
#    define P7 11 // ADC2, CH0
#    define P8 12
#    define P9 15
#    define P10 16
#    define P11 17

#else
#    error "LILKA_VERSION is not defined"
#endif

#endif // LILKA_CONFIG_H
