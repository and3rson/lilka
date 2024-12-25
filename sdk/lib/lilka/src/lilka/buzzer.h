#ifndef LILKA_BUZZER_H
#define LILKA_BUZZER_H

#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

namespace lilka {

// Ідею взято з https://github.com/robsoncouto/arduino-songs/blob/master/doom/doom.ino

// clang-format off
typedef enum {
    NOTE_B0 = 31,
    NOTE_C1 = 33,   NOTE_CS1 = 35,   NOTE_D1 = 37,   NOTE_DS1 = 39,   NOTE_E1 = 41,   NOTE_F1 = 44,   NOTE_FS1 = 46,   NOTE_G1 = 49,   NOTE_GS1 = 52,   NOTE_A1 = 55,   NOTE_AS1 = 58,   NOTE_B1 = 62,
    NOTE_C2 = 65,   NOTE_CS2 = 69,   NOTE_D2 = 73,   NOTE_DS2 = 78,   NOTE_E2 = 82,   NOTE_F2 = 87,   NOTE_FS2 = 93,   NOTE_G2 = 98,   NOTE_GS2 = 104,  NOTE_A2 = 110,  NOTE_AS2 = 117,  NOTE_B2 = 123,
    NOTE_C3 = 131,  NOTE_CS3 = 139,  NOTE_D3 = 147,  NOTE_DS3 = 156,  NOTE_E3 = 165,  NOTE_F3 = 175,  NOTE_FS3 = 185,  NOTE_G3 = 196,  NOTE_GS3 = 208,  NOTE_A3 = 220,  NOTE_AS3 = 233,  NOTE_B3 = 247,
    NOTE_C4 = 262,  NOTE_CS4 = 277,  NOTE_D4 = 294,  NOTE_DS4 = 311,  NOTE_E4 = 330,  NOTE_F4 = 349,  NOTE_FS4 = 370,  NOTE_G4 = 392,  NOTE_GS4 = 415,  NOTE_A4 = 440,  NOTE_AS4 = 466,  NOTE_B4 = 494,
    NOTE_C5 = 523,  NOTE_CS5 = 554,  NOTE_D5 = 587,  NOTE_DS5 = 622,  NOTE_E5 = 659,  NOTE_F5 = 698,  NOTE_FS5 = 740,  NOTE_G5 = 784,  NOTE_GS5 = 831,  NOTE_A5 = 880,  NOTE_AS5 = 932,  NOTE_B5 = 988,
    NOTE_C6 = 1047, NOTE_CS6 = 1109, NOTE_D6 = 1175, NOTE_DS6 = 1245, NOTE_E6 = 1319, NOTE_F6 = 1397, NOTE_FS6 = 1480, NOTE_G6 = 1568, NOTE_GS6 = 1661, NOTE_A6 = 1760, NOTE_AS6 = 1865, NOTE_B6 = 1976,
    NOTE_C7 = 2093, NOTE_CS7 = 2217, NOTE_D7 = 2349, NOTE_DS7 = 2489, NOTE_E7 = 2637, NOTE_F7 = 2794, NOTE_FS7 = 2960, NOTE_G7 = 3136, NOTE_GS7 = 3322, NOTE_A7 = 3520, NOTE_AS7 = 3729, NOTE_B7 = 3951,
    NOTE_C8 = 4186, NOTE_CS8 = 4435, NOTE_D8 = 4699, NOTE_DS8 = 4978, NOTE_E8 = 5274, NOTE_F8 = 5588, NOTE_FS8 = 5920, NOTE_G8 = 6272, NOTE_GS8 = 6645, NOTE_A8 = 7040, NOTE_AS8 = 7459, NOTE_B8 = 7902,
    REST    = 0
} Note;
// clang-format on

typedef struct {
    /// Частота ноти (може бути значенням з Note)
    uint16_t frequency;

    /// Розмір ноти:
    ///
    /// - 1 - ціла нота
    /// - 2 - половина
    /// - 4 - чверть
    /// - 8 - одна восьма
    ///
    /// і т.д.
    ///
    /// Від'ємне значення - це ноти з крапкою:
    /// - -1 - ціла нота з крапкою (1 + 1/2)
    /// - -2 - половина з крапкою (1/2 + 1/4)
    /// - -4 - чверть з крапкою (1/4 + 1/8)
    /// - -8 - одна восьма з крапкою (1/8 + 1/16)
    ///
    /// і т.д.
    int8_t size;
} Tone;

/// Клас для роботи з п'єзо-динаміком.
/// Використовується для відтворення монотонних звуків.
///
/// Всі методи цього класу є неблокуючими, тобто вони не чекають завершення відтворення звуку і не блокують виконання коду, що йде після них.
///
/// Щоб зупинити відтворення звуку, використовуйте метод `stop()`.
///
/// Приклад використання:
///
/// @code
/// #include <lilka.h>
///
/// void setup() {
///     lilka::begin();
/// }
///
/// void loop() {
///     lilka::buzzer.play(lilka::NOTE_A4); // Грати ноту "Ля"
///     delay(500);
///     lilka::buzzer.stop(); // Зупинити відтворення
///     delay(1500);
/// }
/// @endcode
class Buzzer {
public:
    Buzzer();
    /// Почати роботу з п'єзо-динаміком.
    /// \warning Цей метод викликається автоматично при виклику `lilka::begin()`.
    void begin();
    /// Відтворити ноту з певною частотою.
    void play(uint16_t frequency);
    /// Відтворити ноту з певною частотою впродовж певного часу.
    void play(uint16_t frequency, uint32_t duration);
    /// Відтворити мелодію.
    void playMelody(const Tone* melody, uint32_t length, uint32_t tempo = 120);
    /// Зупинити відтворення всіх звуків.
    void stop();
    /// Відтворити мелодію з DOOM - E1M1, At Doom's Gate (Bobby Prince).
    void playDoom();
    /// Перевірити чи увімкнено звук вітання
    bool getStartupBuzzerEnabled();
    /// Увімкнути чи вимкнути звук вітання
    void setStartupBuzzerEnabled(bool enable);

    static void melodyTask(void* arg);

private:
    // cppcheck-suppress unusedPrivateFunction
    void _stop();
    void saveSettings();

    bool startupBuzzer = true;

    SemaphoreHandle_t buzzerMutex;
    TaskHandle_t melodyTaskHandle;
    Tone* currentMelody;
    uint32_t currentMelodyLength;
    uint32_t currentMelodyTempo;
};

/// Екземпляр класу `Buzzer`, який можна використовувати для відтворення монотонних звуків.
/// Вам не потрібно інстанціювати `Buzzer` вручну.
extern Buzzer buzzer;

} // namespace lilka

#endif
