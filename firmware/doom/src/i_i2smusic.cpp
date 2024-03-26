extern "C" {
#include "i_sound.h"
}

static snddevice_t music_sdl_devices[] = {
    // SNDDEVICE_PAS,
    // SNDDEVICE_GUS,
    // SNDDEVICE_WAVEBLASTER,
    // SNDDEVICE_SOUNDCANVAS,
    // SNDDEVICE_GENMIDI,
    // SNDDEVICE_AWE32,
};

static boolean I_SDL_InitMusic(void) {
    return false;
}
static void I_SDL_ShutdownMusic(void) {
}
static void I_SDL_SetMusicVolume(int volume) {
}
static void I_SDL_PlaySong(void* handle, boolean looping) {
}
static void I_SDL_PauseSong(void) {
}
static void I_SDL_ResumeSong(void) {
}
static void I_SDL_StopSong(void) {
}
static void* I_SDL_RegisterSong(void* data, int len) {
    return 0;
}
static void I_SDL_UnRegisterSong(void* handle) {
}
static boolean I_SDL_MusicIsPlaying(void) {
    return false;
}
static void I_SDL_PollMusic(void) {
}

music_module_t DG_music_module = {
    music_sdl_devices,
    arrlen(music_sdl_devices),
    I_SDL_InitMusic,
    I_SDL_ShutdownMusic,
    I_SDL_SetMusicVolume,
    I_SDL_PauseSong,
    I_SDL_ResumeSong,
    I_SDL_RegisterSong,
    I_SDL_UnRegisterSong,
    I_SDL_PlaySong,
    I_SDL_StopSong,
    I_SDL_MusicIsPlaying,
    I_SDL_PollMusic,
};
