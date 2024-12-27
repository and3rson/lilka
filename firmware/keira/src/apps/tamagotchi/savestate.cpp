#include <Preferences.h>

#include "savestate.h"
#include "cpu.h"

bool loadState() {
    bool loaded = false;
    Preferences preferences;
    preferences.begin("keira", true);
    if (preferences.isKey("tama_cpu") && preferences.isKey("tama_cpu_mem")) {
        cpu_state_t cpuState;
        cpu_get_state(&cpuState);
        preferences.getBytes("tama_cpu", &cpuState, sizeof(cpu_state_t));
        preferences.getBytes("tama_cpu_mem", cpuState.memory, MEMORY_SIZE);
        cpu_set_state(&cpuState);
        loaded = true;
    }
    preferences.end();
    return loaded;
}

void saveState() {
    Preferences preferences;
    cpu_state_t cpuState;
    cpu_get_state(&cpuState);
    preferences.begin("keira", false);
    preferences.putBytes("tama_cpu", &cpuState, sizeof(cpu_state_t));
    preferences.putBytes("tama_cpu_mem", cpuState.memory, MEMORY_SIZE);
    preferences.end();
}

void resetState() {
    Preferences preferences;
    preferences.begin("keira", false);
    preferences.remove("tama_cpu");
    preferences.remove("tama_cpu_mem");
    preferences.end();
}
