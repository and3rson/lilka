#include "i_sound.h"

static snddevice_t sound_devices[] = {};

static boolean I_NoSound_InitSound(bool _use_sfx_prefix) {
    return false;
}

static void I_NoSound_ShutdownSound(void) {
}

sound_module_t sound_module_NoSound = {
    sound_devices,
    0,
    I_NoSound_InitSound,
    I_NoSound_ShutdownSound,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};
