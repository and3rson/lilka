#include <esp_heap_caps.h>
#include <lilka.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>

#include "driver.h"
#define OSD_OK          0
#define OSD_INIT_FAILED -1

extern "C" {
#include <event.h>
#include <gui.h>
#include <log.h>
#include <nes/nes.h>
#include <nes/nes_pal.h>
#include <nes/nesinput.h>
#include <nofconfig.h>
#include <osd.h>
#include <noftypes.h>
#include <nofrendo.h>
}

// No need to add `extern "C"` to functions below, because it's already declared in `osd.h`

int osd_init_sound();

void* mem_alloc(int size, bool prefer_fast_memory) {
    // return malloc(size);
    if (prefer_fast_memory) {
        return heap_caps_malloc(size, MALLOC_CAP_8BIT);
    } else {
        return heap_caps_malloc_prefer(size, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
    }
}

const int eventIndices[10] = {
    event_joypad1_up,
    event_joypad1_down,
    event_joypad1_left,
    event_joypad1_right,
    event_joypad1_select,
    event_joypad1_start,
    event_joypad1_a,
    event_joypad1_b,
    event_state_save,
    event_state_load,
};

const lilka::Button buttonIndices[10] = {
    lilka::Button::UP,
    lilka::Button::DOWN,
    lilka::Button::LEFT,
    lilka::Button::RIGHT,
    lilka::Button::SELECT,
    lilka::Button::START,
    lilka::Button::A,
    lilka::Button::B,
    lilka::Button::C,
    lilka::Button::D,
};

void osd_getinput(void) {
    lilka::State state = lilka::controller.getState();
    const lilka::_StateButtons& buttons = *reinterpret_cast<lilka::_StateButtons*>(&state);

    for (int i = 0; i < sizeof(eventIndices) / sizeof(eventIndices[0]); i++) {
        int eventIndex = eventIndices[i];
        int buttonIndex = buttonIndices[i];
        event_t eventHandler = event_get(eventIndex);
        if (!eventHandler) {
            continue;
        }
        if (buttons[buttonIndex].justPressed) {
            eventHandler(INP_STATE_MAKE);
        }
        if (buttons[buttonIndex].justReleased) {
            eventHandler(INP_STATE_BREAK);
        }
    }
}

int logprint(const char* string) {
    return printf("%s", string);
}

int osd_init() {
    nofrendo_log_chain_logfunc(logprint);
    osd_init_sound();
    return 0;
}

void osd_shutdown() {
}

char configfilename[] = "na";
int osd_main(int argc, char* argv[]) {
    config.filename = configfilename;
    return main_loop(argv[0], system_autodetect);
}

TimerHandle_t timer;

int osd_installtimer(int frequency, void* func, int funcsize, void* counter, int countersize) {
    nofrendo_log_printf("Timer install, configTICK_RATE_HZ=%d, freq=%d\n", configTICK_RATE_HZ, frequency);
    timer = xTimerCreate("nes", configTICK_RATE_HZ / frequency, pdTRUE, NULL, (TimerCallbackFunction_t)func);
    xTimerStart(timer, 0);
    return 0;
}

/* filename manipulation */
void osd_fullname(char* fullname, const char* shortname) {
    strncpy(fullname, shortname, PATH_MAX);
}

/* This gives filenames for storage of saves */
// cppcheck-suppress constParameterPointer
extern char* osd_newextension(char* string, char* ext) {
    // dirty: assume both extensions is 3 characters
    size_t l = strlen(string);
    string[l - 3] = ext[1];
    string[l - 2] = ext[2];
    string[l - 1] = ext[3];

    return string;
}

/* This gives filenames for storage of PCX snapshots */
int osd_makesnapname(char* filename, int len) {
    return -1;
}

void osd_getmouse(int* x, int* y, int* button) {
}

#define DEFAULT_FRAGSIZE    64
#define HW_AUDIO_SAMPLERATE 22050
#define HW_AUDIO_BPS        16
static void (*audio_callback)(void* buffer, int length) = NULL;
int16_t* audio_frame;
QueueHandle_t queue;
int osd_init_sound() {
#if LILKA_VERSION == 1
    lilka::serial_err("This part of code should never be called. Audio not supported for this version of lilka");
    return OSD_INIT_FAILED;
#elif LILKA_VERSION == 2
    audio_frame = static_cast<int16_t*>(malloc(DEFAULT_FRAGSIZE * 4));
    if (!audio_frame) {
        lilka::serial_err("Failed to allocate audio_frame\n");
        //
        return OSD_INIT_FAILED;
    }

    esp_i2s::i2s_config_t cfg = {
        .mode = (esp_i2s::i2s_mode_t)(esp_i2s::I2S_MODE_MASTER | esp_i2s::I2S_MODE_TX),
        .sample_rate = HW_AUDIO_SAMPLERATE,
        .bits_per_sample = esp_i2s::I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = esp_i2s::I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format =
            (esp_i2s::i2s_comm_format_t)(esp_i2s::I2S_COMM_FORMAT_I2S | esp_i2s::I2S_COMM_FORMAT_I2S_MSB),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 7,
        .dma_buf_len = 256,
        .use_apll = false,
    };
    i2s_driver_install(esp_i2s::I2S_NUM_0, &cfg, 2, &queue);
    esp_i2s::i2s_pin_config_t pins = {
        .bck_io_num = LILKA_I2S_BCLK,
        .ws_io_num = LILKA_I2S_LRCK,
        .data_out_num = LILKA_I2S_DOUT,
        .data_in_num = -1,
    };
    i2s_set_pin(esp_i2s::I2S_NUM_0, &pins);
    i2s_zero_dma_buffer(esp_i2s::I2S_NUM_0);
    audio_callback = 0;
    return OSD_OK;
#endif

    return OSD_INIT_FAILED;
}

void osd_stopsound() {
    audio_callback = 0;
}

void do_audio_frame() {
    // Reference: https://github.com/moononournation/arduino-nofrendo/blob/c5ba6d39c1fff0ceed314b76f6da5145d0d99bcc/examples/esp32-nofrendo/sound.c#L72
    int left = HW_AUDIO_SAMPLERATE / NES_REFRESH_RATE;
    while (left) {
        int n = DEFAULT_FRAGSIZE;
        if (n > left) n = left;
        audio_callback(audio_frame, n); //get more data

        //16 bit mono -> 32-bit (16 bit r+l)
        const int16_t* mono_ptr = audio_frame + n;
        int16_t* stereo_ptr = audio_frame + n + n;
        int i = n;
        while (i--) {
            // int16_t a = (*(--mono_ptr) >> 2);
            // Too silent, so I changed it to:
            int16_t a = *(--mono_ptr);
            *(--stereo_ptr) = a;
            *(--stereo_ptr) = a;
        }

        size_t i2s_bytes_write;
        i2s_write(esp_i2s::I2S_NUM_0, static_cast<int16_t*>(audio_frame), 4 * n, &i2s_bytes_write, portMAX_DELAY);
        left -= i2s_bytes_write / 4;
    }
}

void osd_setsound(void (*playfunc)(void* buffer, int length)) {
#if LILKA_VERSION == 1
#elif LILKA_VERSION == 2
    audio_callback = playfunc;
    xTaskCreatePinnedToCore(
        [](void* arg) {
            const TickType_t xFrequency = pdMS_TO_TICKS(1000 / NES_REFRESH_RATE);
            TickType_t xLastWakeTime = xTaskGetTickCount();
            while (1) {
                // Call do_audio_frame 60 times per second.
                do_audio_frame();
                vTaskDelayUntil(&xLastWakeTime, xFrequency);
            }
        },
        "nes_audio",
        8192,
        NULL,
        1,
        NULL,
        1
    );
#endif
}

void osd_getsoundinfo(sndinfo_t* info) {
    info->sample_rate = HW_AUDIO_SAMPLERATE;
    info->bps = HW_AUDIO_BPS;
}

void osd_getvideoinfo(vidinfo_t* info) {
    info->default_width = NES_SCREEN_WIDTH;
    info->default_height = NES_SCREEN_HEIGHT;
    info->driver = &Driver::driver;
}
