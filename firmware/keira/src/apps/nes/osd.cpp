#include <esp_heap_caps.h>
#include <lilka.h>

#include <FreeRTOS.h>
#include <freertos/timers.h>

#include "driver.h"

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

void* mem_alloc(int size, bool prefer_fast_memory) {
    return malloc(size);
    // if (prefer_fast_memory) {
    //     return heap_caps_malloc(size, MALLOC_CAP_8BIT);
    // } else {
    //     return heap_caps_malloc_prefer(size, MALLOC_CAP_SPIRAM, MALLOC_CAP_DEFAULT);
    // }
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
    lilka::_StateButtons& buttons = *reinterpret_cast<lilka::_StateButtons*>(&state);

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

int osd_init_sound() {
    return 0;
}

void osd_stopsound() {
}

void do_audio_frame() {
}

void osd_setsound(void (*playfunc)(void* buffer, int length)) {
}

void osd_getsoundinfo(sndinfo_t* info) {
    // dummy value
    info->sample_rate = 22050;
    info->bps = 16;
}

void osd_getvideoinfo(vidinfo_t* info) {
    info->default_width = NES_SCREEN_WIDTH;
    info->default_height = NES_SCREEN_HEIGHT;
    info->driver = &Driver::driver;
}
