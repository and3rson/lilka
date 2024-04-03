#ifndef LILKA_CONFIG_H
#define LILKA_CONFIG_H

#include <stdint.h>

#if LILKA_VERSION == -1

// Lilka on a breadboard (C3)

#    define LILKA_GPIO_UP          4
#    define LILKA_GPIO_DOWN        7
#    define LILKA_GPIO_LEFT        -1
#    define LILKA_GPIO_RIGHT       -1
#    define LILKA_GPIO_SELECT      -1
#    define LILKA_GPIO_START       10
#    define LILKA_GPIO_A           -1
#    define LILKA_GPIO_B           -1
#    define LILKA_GPIO_C           -1
#    define LILKA_GPIO_D           -1

#    define LILKA_SPI_SCK          0
#    define LILKA_SPI_MOSI         1
#    define LILKA_SPI_MISO         8
#    define LILKA_DISPLAY_DC       3
#    define LILKA_DISPLAY_CS       21
#    define LILKA_DISPLAY_RST      2
#    define LILKA_DISPLAY_ROTATION 0
#    define LILKA_DISPLAY_WIDTH    240
#    define LILKA_DISPLAY_HEIGHT   280

#    define LILKA_SDCARD_CS        20

#elif LILKA_VERSION == 1

// Lilka v1 (C3), first prototype from pcb24.com.ua

#    define LILKA_GPIO_UP          4
#    define LILKA_GPIO_DOWN        7
#    define LILKA_GPIO_LEFT        5
#    define LILKA_GPIO_RIGHT       6
#    define LILKA_GPIO_SELECT      9
#    define LILKA_GPIO_START       10
#    define LILKA_GPIO_A           20
#    define LILKA_GPIO_B           21
#    define LILKA_GPIO_C           -1
#    define LILKA_GPIO_D           -1

#    define LILKA_DISPLAY_DC       2
#    define LILKA_DISPLAY_CS       3
#    define LILKA_SPI_SCK          8
#    define LILKA_SPI_MOSI         1
#    define LILKA_DISPLAY_RST      -1
#    define LILKA_DISPLAY_ROTATION 0
#    define LILKA_DISPLAY_WIDTH    240
#    define LILKA_DISPLAY_HEIGHT   280

#    define LILKA_SDCARD_CS        -1

#elif LILKA_VERSION == 2

// Lilka v2 (S3)

// Кнопки
#    define LILKA_GPIO_UP                38
#    define LILKA_GPIO_DOWN              41
#    define LILKA_GPIO_LEFT              39
#    define LILKA_GPIO_RIGHT             40
#    define LILKA_GPIO_SELECT            0 // Режим прошивання
#    define LILKA_GPIO_START             4
#    define LILKA_GPIO_A                 5
#    define LILKA_GPIO_B                 6
#    define LILKA_GPIO_C                 10
#    define LILKA_GPIO_D                 9
// Сон
#    define LILKA_SLEEP                  46
// SPI
#    define LILKA_SPI_SCK                18
#    define LILKA_SPI_MOSI               17
#    define LILKA_SPI_MISO               8
// Дисплей
#    define LILKA_DISPLAY_DC             15
#    define LILKA_DISPLAY_CS             7
#    define LILKA_DISPLAY_RST            -1
#    define LILKA_DISPLAY_ROTATION       3
#    define LILKA_DISPLAY_WIDTH          240 // Display dimensions in unrotated state
#    define LILKA_DISPLAY_HEIGHT         280 // (will be adjusted by rotation inside Arduino_GFX)
// uSD-карта
#    define LILKA_SDCARD_CS              16
// Рівень батареї
#    define LILKA_BATTERY_ADC            3
#    define LILKA_BATTERY_ADC_FUNC(name) adc1_##name
#    define LILKA_BATTERY_ADC_CHANNEL    ADC1_CHANNEL_2
// Buzzer
#    define LILKA_BUZZER                 11
// I2S
#    define LILKA_I2S_BCLK               42
#    define LILKA_I2S_DOUT               2
#    define LILKA_I2S_LRCK               1
// Роз'єм розширення
#    define LILKA_P3                     48
#    define LILKA_P4                     47
#    define LILKA_P5                     21
#    define LILKA_P6                     14 // ADC2, CH3
#    define LILKA_P7                     13 // ADC2, CH2
#    define LILKA_P8                     12 // ADC2, CH1
const uint8_t LILKA_EXT_PINS[] = {LILKA_P3, LILKA_P4, LILKA_P5, LILKA_P6, LILKA_P7, LILKA_P8};

#else
#    error "LILKA_VERSION is not defined - did you forget to set board to lilka_v2 in your platformio.ini?"
#endif

#endif // LILKA_CONFIG_H
