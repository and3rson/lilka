#include <lilka.h>
#include "driver/i2s.h"

extern "C" {
#include "config.h"
#include "i_sound.h"
#include "deh_str.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"
}

static boolean use_sfx_prefix;

void soundTask(void* param);
void queueSound(uint8_t* data, uint32_t length, uint32_t sample_rate, uint8_t vol);

static snddevice_t sound_devices[] = {
    SNDDEVICE_SB,
    SNDDEVICE_PAS,
    SNDDEVICE_GUS,
    SNDDEVICE_WAVEBLASTER,
    SNDDEVICE_SOUNDCANVAS,
    SNDDEVICE_AWE32,
};

const float doomNoteToFreq[] = {
    // Notes start with 175 Hz (F3)
    // Each octave has 24 notes (not 12), thus 2 notes per semitone
    // Table taken from https://www.doomworld.com/idgames/sounds/pcspkr10
    0, //   Silence
    175.00, //     F-3
    180.02,
    185.01, //     F#3
    190.02,
    196.02, //     G-3
    202.02,
    208.01, //     G#3
    214.02,
    220.02, //     A-3
    226.02,
    233.04, //     A#3
    240.02,
    247.03, //     B-3
    254.03,
    262.00, //     C-4
    269.03,
    277.03, //     C#4
    285.04,
    294.03, //     D-4
    302.07,
    311.04, //     D#4
    320.05,
    330.06, //     E-4
    339.06,
    349.08, //     F-4
    359.06,
    370.09, //     F#4
    381.08,
    392.10, //     G-4
    403.10,
    415.01, //     G#4
    427.05,
    440.12, //     A-4
    453.16,
    466.08, //     A#4
    480.15,
    494.07, //     B-4
    508.16,
    523.09, //     C-5
    539.16,
    554.19, //     C#5
    571.17,
    587.19, //     D-5
    604.14,
    622.09, //     D#5
    640.11,
    659.21, //     E-5
    679.10,
    698.17, //     F-5
    719.21,
    740.18, //     F#5
    762.41,
    784.47, //     G-5
    807.29,
    831.48, //     G#5
    855.32,
    880.57, //     A-5
    906.67,
    932.17, //     A#5
    960.69,
    988.55, //     B-5
    1017.20,
    1046.64, //     C-6
    1077.85,
    1109.93, //     C#6
    1141.79,
    1175.54, //     D-6
    1210.12,
    1244.19, //     D#6
    1281.61,
    1318.43, //     E-6
    1357.42,
    1397.16, //     F-6
    1439.30,
    1480.37, //     F#6
    1523.85,
    1569.97, //     G-6
    1614.58,
    1661.81, //     G#6
    1711.87,
    1762.45, //     A-6
    1813.34,
    1864.34, //     A#6
    1921.38,
    1975.46, //     B-6
    2036.14,
    2093.29, //     C-7
    2157.64,
    2217.80, //     C#7
    2285.78,
    2353.41, //     D-7
    2420.24,
    2490.98, //     D#7
    2565.97,
    2639.77, //     E-7

};

static boolean I_Buzzer_InitSound(bool _use_sfx_prefix) {
    lilka::serial_log("I_Buzzer_InitSound");
    use_sfx_prefix = _use_sfx_prefix;
    return true;
}

static void I_Buzzer_ShutdownSound(void) {
    lilka::serial_log("I_Buzzer_ShutdownSound");
}

static void GetSfxLumpName(sfxinfo_t* sfx, char* buf, size_t buf_len) {
    // Linked sfx lumps? Get the lump number for the sound linked to.

    if (sfx->link != NULL) {
        sfx = sfx->link;
    }

    // DP prefix means PC speaker sound lump.

    if (use_sfx_prefix) {
        M_snprintf(buf, buf_len, "dp%s", DEH_String(sfx->name));
    } else {
        M_StringCopy(buf, DEH_String(sfx->name), buf_len);
    }
}

static int I_Buzzer_GetSfxLumpNum(sfxinfo_t* sfx) {
    char namebuf[9];

    GetSfxLumpName(sfx, namebuf, sizeof(namebuf));

    return W_GetNumForName(namebuf);
}

static void I_Buzzer_UpdateSound(void) {
    // Nothing to do here.
}

static void I_Buzzer_UpdateSoundParams(int handle, int vol, int sep) {
    // Nothing to do here.
}

static int I_Buzzer_StartSound(sfxinfo_t* sfxinfo, int channel, int vol, int sep) {
    // lilka::serial_log("I_Buzzer_StartSound: %s, channel: %d", DEH_String(sfxinfo->name), channel);
    // Get sound lump
    int lump = I_Buzzer_GetSfxLumpNum(sfxinfo);
    if (lump == -1) {
        lilka::serial_log("I_Buzzer_StartSound: I_Buzzer_GetSfxLumpNum failed");
        return -1;
    }

    uint32_t lumplen = W_LumpLength(lump);
    // lilka::serial_log("I_Buzzer_StartSound: lump %d, length %d", lump, lumplen);

    // Load sound lump
    byte* data = static_cast<byte*>(W_CacheLumpNum(lump, PU_CACHE)); // TODO - free
    if (data == NULL) {
        lilka::serial_log("I_Buzzer_StartSound: W_CacheLumpNum failed");
        return -1;
    }

    if (lumplen < 4) {
        // Invalid sound
        lilka::serial_log("I_Buzzer_StartSound: Invalid sound");
        return false;
    }

    uint32_t length = data[2] | (data[3] << 8);

    // Skip header
    data += 4;
    length -= 4;

    lilka::Tone* tone = new lilka::Tone[length];
    for (int i = 0; i < length; i++) {
        tone[i].size = 1;
        tone[i].frequency = doomNoteToFreq[data[i]];
    }
    // Each sample is played for 1/140 s, so BPM is 8400
    lilka::buzzer.playMelody(const_cast<lilka::Tone*>(tone), length, 8400);
    delete[] tone;

    return 0; // Return channel number?
}

static void I_Buzzer_StopSound(int handle) {
    // Nothing to do here.
}

static boolean I_Buzzer_SoundIsPlaying(int handle) {
    return false; // TODO
}

static void I_Buzzer_PrecacheSounds(sfxinfo_t* sounds, int num_sounds) {
    // Nothing to do here.
}

sound_module_t sound_module_Buzzer = {
    sound_devices,
    arrlen(sound_devices),
    I_Buzzer_InitSound,
    I_Buzzer_ShutdownSound,
    I_Buzzer_GetSfxLumpNum,
    I_Buzzer_UpdateSound,
    I_Buzzer_UpdateSoundParams,
    I_Buzzer_StartSound,
    I_Buzzer_StopSound,
    I_Buzzer_SoundIsPlaying,
    I_Buzzer_PrecacheSounds,
};
