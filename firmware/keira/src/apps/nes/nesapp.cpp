#include "nesapp.h"
#include "driver.h"

NesApp::NesApp(String path) : App("NES", 0, 0, lilka::display.width(), lilka::display.height()) {
    argv[0] = new char[path.length() + 1];
    strcpy(argv[0], path.c_str());
#ifdef NESAPP_INTERLACED
    setFlags(static_cast<AppFlags>(AppFlags::APP_FLAG_FULLSCREEN | AppFlags::APP_FLAG_INTERLACED));
#else
    setFlags(static_cast<AppFlags>(AppFlags::APP_FLAG_FULLSCREEN));
#endif
}

NesApp::~NesApp() {
    delete[] argv[0];
}

void NesApp::run() {
    // Load the ROM
    Driver::setNesApp(this);
    nofrendo_main(1, argv);
    // TODO: Figure out how to terminate the emulator without crashing
}
