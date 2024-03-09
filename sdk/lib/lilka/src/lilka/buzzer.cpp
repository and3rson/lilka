#include <Arduino.h>

#include "buzzer.h"
#include "config.h"
#include "serial.h"

namespace lilka {

void Buzzer::begin() {
    // TODO: Use ledc?
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#endif
    pinMode(LILKA_BUZZER, OUTPUT);
}

void Buzzer::play(uint16_t frequency) {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#endif
    tone(LILKA_BUZZER, frequency);
}

void Buzzer::play(uint16_t frequency, uint32_t duration) {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#endif
    tone(LILKA_BUZZER, frequency, duration);
}

void Buzzer::stop() {
#if LILKA_VERSION < 2
    serial_err("Buzzer is not supported on this board");
    return;
#endif
    noTone(LILKA_BUZZER);
}

void Buzzer::playMelody(const Tone *melody, uint32_t length, uint32_t tempo) {
    // TODO: Make this a FreeRTOS task
    for (uint32_t i = 0; i < length; i++) {
        Tone tone = melody[i];
        if (tone.size == 0) {
            continue;
        }
        uint32_t duration = (60000 / tempo) / abs(tone.size);
        if (tone.size < 0) {
            duration += duration / 2;
        }

        if (melody[i].frequency == 0) {
            delay(duration);
        } else {
            play(melody[i].frequency, duration);
            delay(duration);
        }
    }
}

void Buzzer::playDoom() {
    const Tone doom_e1m1[] = {
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, -2},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_FS3, -16}, {NOTE_D3, -16},  {NOTE_B2, -16}, {NOTE_A3, -16},  {NOTE_FS3, -16}, {NOTE_B2, -16},
        {NOTE_D3, -16},  {NOTE_FS3, -16}, {NOTE_A3, -16}, {NOTE_FS3, -16}, {NOTE_D3, -16},  {NOTE_B2, -16},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B3, -16},  {NOTE_G3, -16},
        {NOTE_E3, -16},  {NOTE_G3, -16},  {NOTE_B3, -16}, {NOTE_E4, -16},  {NOTE_G3, -16},  {NOTE_B3, -16},
        {NOTE_E4, -16},  {NOTE_B3, -16},  {NOTE_G4, -16}, {NOTE_B4, -16},  {NOTE_A2, 8},    {NOTE_A2, 8},
        {NOTE_A3, 8},    {NOTE_A2, 8},    {NOTE_A2, 8},   {NOTE_G3, 8},    {NOTE_A2, 8},    {NOTE_A2, 8},
        {NOTE_F3, 8},    {NOTE_A2, 8},    {NOTE_A2, 8},   {NOTE_DS3, 8},   {NOTE_A2, 8},    {NOTE_A2, 8},
        {NOTE_E3, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_A3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_G3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_DS3, -2},  {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_A3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_G3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_DS3, 8},   {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_E3, 8},    {NOTE_F3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, 8},   {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_G3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, -16}, {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_C4, -16}, {NOTE_A3, -16},  {NOTE_F3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_CS3, 8},   {NOTE_CS3, 8},
        {NOTE_CS4, 8},   {NOTE_CS3, 8},   {NOTE_CS3, 8},  {NOTE_B3, 8},    {NOTE_CS3, 8},   {NOTE_CS3, 8},
        {NOTE_A3, 8},    {NOTE_CS3, 8},   {NOTE_CS3, 8},  {NOTE_G3, 8},    {NOTE_CS3, 8},   {NOTE_CS3, 8},
        {NOTE_GS3, 8},   {NOTE_A3, 8},    {NOTE_B2, 8},   {NOTE_B2, 8},    {NOTE_B3, 8},    {NOTE_B2, 8},
        {NOTE_B2, 8},    {NOTE_A3, 8},    {NOTE_B2, 8},   {NOTE_B2, 8},    {NOTE_G3, 8},    {NOTE_B2, 8},
        {NOTE_B2, 8},    {NOTE_F3, -2},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B3, -16}, {NOTE_G3, -16},  {NOTE_E3, -16},  {NOTE_G3, -16},
        {NOTE_B3, -16},  {NOTE_E4, -16},  {NOTE_G3, -16}, {NOTE_B3, -16},  {NOTE_E4, -16},  {NOTE_B3, -16},
        {NOTE_G4, -16},  {NOTE_B4, -16},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_FS3, -16}, {NOTE_DS3, -16}, {NOTE_B2, -16}, {NOTE_FS3, -16}, {NOTE_DS3, -16}, {NOTE_B2, -16},
        {NOTE_G3, -16},  {NOTE_D3, -16},  {NOTE_B2, -16}, {NOTE_DS4, -16}, {NOTE_DS3, -16}, {NOTE_B2, -16},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E4, -16},  {NOTE_B3, -16},
        {NOTE_G3, -16},  {NOTE_G4, -16},  {NOTE_E4, -16}, {NOTE_G3, -16},  {NOTE_B3, -16},  {NOTE_D4, -16},
        {NOTE_E4, -16},  {NOTE_G4, -16},  {NOTE_E4, -16}, {NOTE_G3, -16},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, -2},  {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_A3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_G3, 8},    {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_F3, 8},    {NOTE_A2, 8},
        {NOTE_A2, 8},    {NOTE_DS3, 8},   {NOTE_A2, 8},   {NOTE_A2, 8},    {NOTE_E3, 8},    {NOTE_F3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, 8},   {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_G3, 8},
        {NOTE_A2, 8},    {NOTE_A2, 8},    {NOTE_A3, -16}, {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_C4, -16}, {NOTE_A3, -16},  {NOTE_F3, -16},  {NOTE_A3, -16},
        {NOTE_F3, -16},  {NOTE_D3, -16},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, -2},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_C3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_AS2, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B2, 8},   {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, -2},  {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_E3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_D3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_C3, 8},    {NOTE_E2, 8},    {NOTE_E2, 8},   {NOTE_AS2, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},
        {NOTE_B2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, -2},  {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_E3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_D3, 8},    {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_C3, 8},    {NOTE_E2, 8},
        {NOTE_E2, 8},    {NOTE_AS2, 8},   {NOTE_E2, 8},   {NOTE_E2, 8},    {NOTE_B2, 8},    {NOTE_C3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_E3, 8},   {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_D3, 8},
        {NOTE_E2, 8},    {NOTE_E2, 8},    {NOTE_B3, -16}, {NOTE_G3, -16},  {NOTE_E3, -16},  {NOTE_B2, -16},
        {NOTE_E3, -16},  {NOTE_G3, -16},  {NOTE_C4, -16}, {NOTE_B3, -16},  {NOTE_G3, -16},  {NOTE_B3, -16},
        {NOTE_G3, -16},  {NOTE_E3, -16},
    };
    playMelody(
        doom_e1m1, sizeof(doom_e1m1) / sizeof(doom_e1m1[0]), 60
    ); // TODO - Should be 160... But note durations seem to be off
}

Buzzer buzzer;

} // namespace lilka
