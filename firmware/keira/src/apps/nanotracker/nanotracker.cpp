#include <lilka.h>
#include "I2S.h"

#include "nanotracker.h"

constexpr int32_t bpm = 400;
constexpr int32_t scoreTop = 0;
const int32_t scoreBottom = lilka::display.height();
constexpr int32_t channelCount = 3;
constexpr int32_t channelSize = 32;

typedef enum { WAVEFORM_SQUARE, WAVEFORM_SAWTOOTH, WAVEFORM_TRIANGLE, WAVEFORM_SINE, WAVEFORM_NOISE } waveform_t;

typedef struct {
    int32_t pitch;
    // float velocity;
} event_t;

typedef struct {
    waveform_t waveform;
    float volume;
    float panning;
    float pitch;
    event_t events[channelSize];
} channel_t;

// Typedef for function with signature float(float time, float frequency, float amplitude, float phase)
typedef float (*waveform_fn_t)(float time, float frequency, float amplitude, float phase);

// Triangle wave function
float triangle(float time, float frequency, float amplitude, float phase) {
    return 2.0f * amplitude * fabsf(2.0f * fmodf(time * frequency + phase, 1.0f) - 1.0f) - amplitude;
}

// Sine wave function
float sine(float time, float frequency, float amplitude, float phase) {
    return amplitude * sinf(2.0f * M_PI * frequency * time + phase);
}

// Square wave function
float square(float time, float frequency, float amplitude, float phase) {
    return amplitude * (sinf(2.0f * M_PI * frequency * time + phase) > 0.0f ? 1.0f : -1.0f);
}

// Sawtooth wave function
float sawtooth(float time, float frequency, float amplitude, float phase) {
    return 2.0f * amplitude * (fmodf(time * frequency + phase, 1.0f) - 0.5f);
}

const channel_t channels[channelCount] = {
    {
        .waveform = WAVEFORM_SQUARE,
        .volume = 1.0f,
        .panning = 0.5f,
        .pitch = 1.0f,
        .events =
            {
                {lilka::NOTE_E4},
                {0},
                {lilka::NOTE_E4},
                {0},
                {0},
                {0},
                {0},
                {lilka::NOTE_E4},
                {0},
                {lilka::NOTE_E4},
                {0},
                {lilka::NOTE_D4},
                {lilka::NOTE_E4},
                {0},
                {lilka::NOTE_D4},
                {0},
                {lilka::NOTE_C4},
                {0},
                {lilka::NOTE_C4},
                {0},
                {0},
                {0},
                {0},
                {lilka::NOTE_A3},
                {0},
                {lilka::NOTE_B3},
                {0},
                {lilka::NOTE_A3},
                {lilka::NOTE_C4},
                {0},
                {lilka::NOTE_B3},
                {0},
            },
    },
    // {
    //     .waveform = WAVEFORM_SAWTOOTH,
    //     .volume = 1.0f,
    //     .panning = 0.5f,
    //     .pitch = 1.0f,
    //     .events = {},
    // },
    {
        .waveform = WAVEFORM_SAWTOOTH,
        .volume = 1.0f,
        .panning = 0.5f,
        .pitch = 1.0f,
        .events =
            {
                {lilka::NOTE_E2},
                {lilka::NOTE_G2},
                {lilka::NOTE_A2},
                {lilka::NOTE_B2},
                //
                {0}, // {lilka::NOTE_E2},
                {lilka::NOTE_G2},
                {lilka::NOTE_A2},
                {lilka::NOTE_B2},
                //
                {0}, // {lilka::NOTE_E2},
                {lilka::NOTE_G2},
                {lilka::NOTE_A2},
                {0}, // {lilka::NOTE_B2},
                //
                {lilka::NOTE_E2},
                {lilka::NOTE_G2},
                {0}, // {lilka::NOTE_A2},
                {lilka::NOTE_B2},
                //
                {lilka::NOTE_C2},
                {lilka::NOTE_E2},
                {lilka::NOTE_F2},
                {lilka::NOTE_G2},
                //
                {0}, // {lilka::NOTE_C2},
                {lilka::NOTE_E2},
                {lilka::NOTE_F2},
                {lilka::NOTE_G2},
                //
                {0}, // {lilka::NOTE_C2},
                {lilka::NOTE_E2},
                {lilka::NOTE_F2},
                {0}, // {lilka::NOTE_G2},
                //
                {lilka::NOTE_C2},
                {lilka::NOTE_E2},
                {0}, // {lilka::NOTE_F2},
                {lilka::NOTE_G2},
            },
    },
    {
        .waveform = WAVEFORM_TRIANGLE,
        .volume = 1.0f,
        .panning = 0.5f,
        .pitch = 1.0f,
        .events =
            {
                {lilka::NOTE_E6},
                {0},
                {0},
                {0},
                {lilka::NOTE_E6},
                {0},
                {lilka::NOTE_E6},
                {0},
                {0},
                {lilka::NOTE_E6},
                {0},
                {0},
                {lilka::NOTE_E6},
                {0},
                {0},
                {0},
                {lilka::NOTE_C6},
                {0},
                {0},
                {0},
                {lilka::NOTE_C6},
                {0},
                {lilka::NOTE_C6},
                {0},
                {0},
                {lilka::NOTE_C6},
                {0},
                {0},
                {lilka::NOTE_C6},
                {0},
                {0},
                {0},
            },
    },
};

NanoTrackerApp::NanoTrackerApp() : App("NanoTracker", 0, 0, lilka::display.width(), lilka::display.height()) {
    this->setFlags(APP_FLAG_FULLSCREEN);
}

int32_t globalCurrentEventIndex = 0;
xSemaphoreHandle xMutex;

void sequencerTask(void* pvParameters) {
    constexpr uint32_t sampleRate = 11025;
    // Calculate the number of samples per beat
    int32_t samplesPerBeat = sampleRate * 60 / bpm;
    I2S.begin(I2S_PHILIPS_MODE, sampleRate, 16);
    uint64_t time = 0;
    while (true) {
        // Iterate over events
        for (int32_t eventIndex = 0; eventIndex < channelSize; eventIndex++) {
            xSemaphoreTake(xMutex, portMAX_DELAY);
            globalCurrentEventIndex = eventIndex;
            xSemaphoreGive(xMutex);
            // Process one bar
            for (int i = 0; i < samplesPerBeat; i++) {
                time += 1;
                int32_t mix = 0;
                // Iterate over channels
                for (int32_t channelIndex = 0; channelIndex < channelCount; channelIndex++) {
                    const channel_t* channel = &channels[channelIndex];
                    event_t event = channel->events[eventIndex];
                    waveform_fn_t waveform_fn = nullptr;
                    switch (channel->waveform) {
                        case WAVEFORM_SQUARE:
                            waveform_fn = square;
                            break;
                        case WAVEFORM_SAWTOOTH:
                            waveform_fn = sawtooth;
                            break;
                        case WAVEFORM_TRIANGLE:
                            waveform_fn = triangle;
                            break;
                        case WAVEFORM_SINE:
                            waveform_fn = sine;
                            break;
                        case WAVEFORM_NOISE:
                            break;
                    }
                    int16_t value = 0;
                    if (event.pitch != 0) {
                        float frequency = static_cast<float>(event.pitch);
                        float amplitude = 1.0;
                        float phase = 0.0;
                        value = waveform_fn(((float)time) / sampleRate, frequency, amplitude, phase) * 32767;
                    }
                    mix += value;
                }
                mix = mix / channelCount / 2; // Divide by 2 to reduce volume
                I2S.write(static_cast<int16_t>(mix));
                I2S.write(static_cast<int16_t>(mix));
            }
        }
        taskYIELD();
    }
}

// TODO: Move this to a separate file?
const double A4_FREQUENCY = 440.0;
const double SEMITONE_RATIO = pow(2.0, 1.0 / 12.0);
// Function to convert frequency to the nearest musical note
char* frequency_to_note(double frequency) {
    // Create a string to store the result
    static char result[4]; // Assuming maximum length of note name is 3 characters

    // Calculate the number of semitones away from A4
    if (frequency > 0) {
        double semitones_from_a4 = 12.0 * log2(frequency / A4_FREQUENCY);

        // Round to the nearest integer number of semitones
        int rounded_semitones = (int)round(semitones_from_a4);

        // Define the musical note names
        char* note_names[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};

        // Calculate the octave
        int octave = 4 + rounded_semitones / 12;

        // Calculate the index of the note within the octave
        int note_index = (rounded_semitones % 12 + 12) % 12;

        sprintf(result, "%s%d", note_names[note_index], octave);
    } else {
        sprintf(result, "---");
    }

    return result;
}

void NanoTrackerApp::run() {
    xMutex = xSemaphoreCreateMutex();
    xTaskCreatePinnedToCore(sequencerTask, "sequencerTask", 8192, nullptr, 1, nullptr, 0);

    while (1) {
        canvas->fillScreen(lilka::colors::Black);
        canvas->setFont(FONT_10x20);
        canvas->setTextBound(0, 0, canvas->width(), canvas->height());
        canvas->setTextColor(lilka::colors::White);
        constexpr int16_t itemHeight = 20;
        const int16_t centerY = canvas->height() / 2;
        xSemaphoreTake(xMutex, portMAX_DELAY);
        int currentEventIndex = globalCurrentEventIndex;
        xSemaphoreGive(xMutex);
        for (int eventIndex = 0; eventIndex < channelSize; eventIndex++) {
            // Draw score, with current event in the middle
            int16_t y = centerY + (eventIndex - currentEventIndex) * itemHeight;
            if (y < scoreTop || y > scoreBottom) {
                continue;
            }
            if (eventIndex == currentEventIndex) {
                canvas->fillRect(0, y, canvas->width(), itemHeight, lilka::colors::Blue);
            }
            canvas->setCursor(24, y + 18);
            canvas->printf("| %2d | ", eventIndex + 1);
            canvas->printf("%-3s | ", frequency_to_note(channels[0].events[eventIndex].pitch));
            canvas->printf("%-3s | ", frequency_to_note(channels[1].events[eventIndex].pitch));
            canvas->printf("%-3s | ", frequency_to_note(channels[2].events[eventIndex].pitch));
        }
        queueDraw();
    }
}
