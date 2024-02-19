# Doom Generic (Lilka edition)

Ця бібліотека містить модифікований код з <https://github.com/ozkl/doomgeneric>.

## Зміна 1: зменшення розміру кадру

Я зменшив SCREENWIDTH/SCREENHEIGHT і DOOMGENERIC_RESX/DOOMGENERIC_RESY до розміру екрана Лілки.

## Зміна 2: динамічна алокація пам'яті

Оскільки ESP32-S3 має всього 400 КБ SRAM, деякі масиви не поміщаються в RAM, і їх потрібно динамічно ініціалізувати в heap всередині PSRAM (якої може бути аж до 16 МБ).

Тому найважливіша зміна - це заміна деяких стекових алокацій пам'яті на динамічну (malloc/free), наприклад:

```cpp
// До:

#define MAXVISPLANES 128
visplane_t           visplanes[MAXVISPLANES];
#define MAXOPENINGS  SCREENWIDTH*64
short                openings[MAXOPENINGS];

// Після:

#define MAXVISPLANES 128
visplane_t*          visplanes;
#define MAXOPENINGS	 SCREENWIDTH*64
short*               openings;

void R_AllocPlanes (void)
{
    visplanes = malloc (sizeof(visplane_t) * MAXVISPLANES);
    openings = malloc (sizeof(short) * MAXOPENINGS);
}

void R_FreePlanes (void)
{
    free (visplanes);
    free (openings);
}
```

Повний перелік нових алокацій знаходиться в [d_alloc.c](./src/d_alloc.c).

Він викликається в [main.cpp](../../src/main.cpp).
