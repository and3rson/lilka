extern "C" {
#include "i_sound.h"
}

static snddevice_t music_devices[] = {
    // SNDDEVICE_PAS,
    // SNDDEVICE_GUS,
    // SNDDEVICE_WAVEBLASTER,
    // SNDDEVICE_SOUNDCANVAS,
    // SNDDEVICE_GENMIDI,
    // SNDDEVICE_AWE32,
};

static boolean I_Default_InitMusic(void) {
    return false;
}
static void I_Default_ShutdownMusic(void) {
}
static void I_Default_SetMusicVolume(int volume) {
}
static void I_Default_PlaySong(void* handle, boolean looping) {
}
static void I_Default_PauseSong(void) {
}
static void I_Default_ResumeSong(void) {
}
static void I_Default_StopSong(void) {
}
static void* I_Default_RegisterSong(void* data, int len) {
    return 0;
}
static void I_Default_UnRegisterSong(void* handle) {
}
static boolean I_Default_MusicIsPlaying(void) {
    return false;
}
static void I_Default_PollMusic(void) {
}

music_module_t DG_music_module = {
    music_devices,
    arrlen(music_devices),
    I_Default_InitMusic,
    I_Default_ShutdownMusic,
    I_Default_SetMusicVolume,
    I_Default_PauseSong,
    I_Default_ResumeSong,
    I_Default_RegisterSong,
    I_Default_UnRegisterSong,
    I_Default_PlaySong,
    I_Default_StopSong,
    I_Default_MusicIsPlaying,
    I_Default_PollMusic,
};
