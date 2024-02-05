#ifndef LILKA_CONFIG_H
#define LILKA_CONFIG_H

#if LILKA_VERSION == -1

// Lilka on a breadboard

#    define LILKA_GPIO_UP 4
#    define LILKA_GPIO_DOWN 7
#    define LILKA_GPIO_LEFT 5
#    define LILKA_GPIO_RIGHT 6
#    define LILKA_GPIO_SELECT 9
#    define LILKA_GPIO_START 10
#    define LILKA_GPIO_A 20
#    define LILKA_GPIO_B -1

#    define LILKA_DISPLAY_DC 3
#    define LILKA_DISPLAY_CS 21
#    define LILKA_DISPLAY_SCK 0
#    define LILKA_DISPLAY_MOSI 1
#    define LILKA_DISPLAY_RST 2
#    define LILKA_DISPLAY_ROTATION 0
#    define LILKA_DISPLAY_WIDTH 240
#    define LILKA_DISPLAY_HEIGHT 280

#elif LILKA_VERSION == 1

// Lilka v1 (first prototype from pcb24.com.ua)

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

#else
#    error "LILKA_VERSION is not defined"
#endif

#endif // LILKA_CONFIG_H
