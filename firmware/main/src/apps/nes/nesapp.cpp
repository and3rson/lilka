#include "nesapp.h"
#include "driver.h"

NesApp::NesApp(String path) : App("NES") {
    argv[0] = (char *)path.c_str();
}

void NesApp::run() {
    // Load the ROM
    Driver::setNesApp(this);
    nofrendo_main(1, argv);
    // TODO: Figure out how to terminate the emulator without crashing
}
