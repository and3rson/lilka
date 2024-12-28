#include "config.h"
#include "spi.h"

namespace lilka {

/// Системна шина SPI.
/// Використовується внутрішньо для роботи з дисплеєм та SD-картою.
/// Ми радимо не чіпати цей об'єкт, якщо ви не знаєте, що робите.
SPIClass SPI1(SPI1_NUM); // Системний SPI (Дисплей та SD-карта)
#ifdef SPI2_NUM
/// Користувацька шина SPI.
///
/// Використовується для роботи з будь-якими іншими пристроями.
/// Для її використання потрібно викликати `SPI2.begin()`.
/// Приклад використання SPI2 для роботи з двома SPI-пристроями:
///
/// @code
/// #include "lilka.h"
///
/// // Визначення пінів для SPI2. Можна використовувати будь-які піни,
/// // які виведені на роз'єм розширення:
// #define SPI2_SCK     12
// #define SPI2_MISO    13
// #define SPI2_MOSI    14
// #define SPI2_DEV1_CS 21 // Chip Select для пристрою 1
// #define SPI2_DEV2_CS 47 // Chip Select для пристрою 2
///
/// void setup() {
///     lilka::begin();
///     lilka::SPI2.begin(SPI2_SCK, SPI2_MISO, SPI2_MOSI);
///     pinMode(SPI2_DEV1_CS, OUTPUT);
///     pinMode(SPI2_DEV2_CS, OUTPUT);
///     digitalWrite(SPI2_DEV1_CS, HIGH); // Вимкнути пристрій 1
///     digitalWrite(SPI2_DEV2_CS, HIGH); // Вимкнути пристрій 2
/// }
///
/// void loop() {
///     // Надсилання даних пристроєві, під'єднаному до SPI2 (лінія активації - SPI2_DEV1_CS)
///     lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
///     digitalWrite(SPI2_DEV1_CS, LOW); // Увімкнути пристрій 1
///     lilka::SPI2.transfer(0x55);
///     digitalWrite(SPI2_DEV1_CS, HIGH); // Вимкнути пристрій 1
///     lilka::SPI2.endTransaction();
///
///     delay(500);
///
///     // Надсилання даних пристроєві, під'єднаному до SPI2 (лінія активації - SPI2_DEV2_CS)
///     lilka::SPI2.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
///     digitalWrite(SPI2_DEV2_CS, LOW); // Увімкнути пристрій 2
///     lilka::SPI2.transfer(0xAA);
///     digitalWrite(SPI2_DEV2_CS, HIGH); // Вимкнути пристрій 2
///     lilka::SPI2.endTransaction();
///
///     delay(1000);
/// }
/// @endcode
// clang-format off
#ifdef USE_EXT_SPI_FOR_SD
    #define SPI2_SCK     12
    #define SPI2_MISO    13
    #define SPI2_MOSI    14
    #define SPI2_DEV1_CS 21 // Chip Select для пристрою 1
    //#define SPI2_DEV2_CS 47 // Chip Select для пристрою 2
#endif
// clang-format on

SPIClass SPI2(SPI2_NUM); // Користувацький SPI
#endif

void spi_begin() {
#if LILKA_VERSION == 1
    SPI1.begin(LILKA_SPI_SCK, -1, LILKA_SPI_MOSI);
#else
    SPI1.begin(LILKA_SPI_SCK, LILKA_SPI_MISO, LILKA_SPI_MOSI);

#endif
#ifdef USE_EXT_SPI_FOR_SD
    SPI2.begin(LILKA_SPI_SCK, LILKA_SPI_MISO, LILKA_SPI_MOSI);
#endif
}

} // namespace lilka
