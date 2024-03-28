#pragma once

#define EEPROM_MAGIC_NUMBER 0x12

void initEEPROM();

bool validEEPROM();

void loadStateFromEEPROM(cpu_state_t* cpuState);

void eraseStateFromEEPROM();

void saveStateToEEPROM(cpu_state_t* cpuState);

void loadHardcodedState(cpu_state_t* cpuState);