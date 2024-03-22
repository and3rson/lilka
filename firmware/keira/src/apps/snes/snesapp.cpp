#include "snesapp.h"

extern "C" {
#include "snes9x.h"
#include "soundux.h"
#include "memmap.h"
#include "apu.h"
#include "display.h"
#include "gfx.h"
#include "cpuexec.h"
#include "srtc.h"
#include "save.h"
}

SNesApp::SNesApp(class String path) : App("SuperNES", 0, 0, lilka::display.width(), lilka::display.height()) {
    romPath = path;
    setFlags(AppFlags::APP_FLAG_FULLSCREEN);
}

void SNesApp::run() {
    // Init graphics
    GFX.Pitch = canvas->width() * 2;
    GFX.ZPitch = canvas->width();
    GFX.Screen = (uint8_t*)canvas->getFramebuffer();
    GFX.SubScreen = (uint8_t*)malloc(GFX.Pitch * canvas->height());
    GFX.ZBuffer = (uint8_t*)malloc(GFX.ZPitch * canvas->height());
    GFX.SubZBuffer = (uint8_t*)malloc(GFX.ZPitch * canvas->height());

    if (!S9xInitMemory()) {
        alert("Error", "S9xInitMemory failed");
        return;
    }

    if (!S9xInitAPU()) {
        alert("Error", "S9xInitAPU failed");
        return;
    }

    if (!S9xInitSound(0, 0)) {
        alert("Error", "S9xInitSound failed");
        return;
    }

    if (!S9xInitGFX()) {
        alert("Error", "S9xGfxInit failed");
        return;
    }

    if (!LoadROM(romPath.c_str())) {
        alert("Error", "Failed to load ROM");
        return;
    }

    while (1) {
        lilka::serial_log("Running S9xMainLoop");
        S9xMainLoop();
    }
}

void SNesApp::alert(const char* title, const char* message) {
    lilka::Alert box(title, message);
    box.draw(canvas);
    queueDraw();
    while (!box.isFinished()) {
        box.update();
    }
}
