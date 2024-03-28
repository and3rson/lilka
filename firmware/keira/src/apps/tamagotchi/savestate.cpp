#include <Arduino.h>
#include <EEPROM.h>
#include "cpu.h"
#include "savestate.h"
#include "hardcoded_state.h"

void initEEPROM()
{
#if defined(ESP8266) || defined(ESP32)
    EEPROM.begin(1 + sizeof(cpu_state_t) + MEMORY_SIZE);
#endif
}

bool validEEPROM()
{
    return EEPROM.read(0) == EEPROM_MAGIC_NUMBER;
}

void loadStateFromEEPROM(cpu_state_t* cpuState)
{
    cpu_get_state(cpuState);
    u4_t *memTemp = cpuState->memory;
    EEPROM.get(1, *cpuState);
    cpu_set_state(cpuState);
    uint32_t i = 0;
    for (i = 0; i < MEMORY_SIZE; i++)
    {
        memTemp[i] = EEPROM.read(1 + sizeof(cpu_state_t) + i);
    }
#ifdef ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START    
    Serial.print(F("Loaded "));
    Serial.print(1 + sizeof(cpu_state_t) + MEMORY_SIZE);
    Serial.println(F(" bytes"));
#endif
}

void eraseStateFromEEPROM() {
    for (uint32_t i = 0; i < EEPROM.length(); i++)
        EEPROM.write(i, 0);
#if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif
}

void saveStateToEEPROM(cpu_state_t* cpuState)
{
    uint32_t i = 0;
    if (EEPROM.read(0) != EEPROM_MAGIC_NUMBER)
        eraseStateFromEEPROM();

#if defined(ESP8266) || defined(ESP32)
    EEPROM.write(0, EEPROM_MAGIC_NUMBER);
#else
    EEPROM.update(0, EEPROM_MAGIC_NUMBER);
#endif

    cpu_get_state(cpuState);
    EEPROM.put(1, *cpuState);
    for (i = 0; i < MEMORY_SIZE; i++)
    {
#if defined(ESP8266) || defined(ESP32)
        EEPROM.write(1 + sizeof(cpu_state_t) + i, cpuState->memory[i]);
#else
        EEPROM.update(1 + sizeof(cpu_state_t) + i, cpuState->memory[i]);
#endif
    }

#if defined(ESP8266) || defined(ESP32)
    EEPROM.commit();
#endif

#ifdef ENABLE_DUMP_STATE_TO_SERIAL_WHEN_START    
    Serial.print(F("Saved "));
    Serial.print(1 + sizeof(cpu_state_t) + MEMORY_SIZE);
    Serial.println(F(" bytes"));
#endif
}

void loadHardcodedState(cpu_state_t* cpuState)
{
  cpu_get_state(cpuState);
  u4_t *memTemp = cpuState->memory;
  uint16_t i;
  uint8_t *cpuS = (uint8_t *)cpuState;
  for (i = 0; i < sizeof(cpu_state_t); i++)
  {
    cpuS[i] = pgm_read_byte_near(hardcodedState + i);
  }
  for (i = 0; i < MEMORY_SIZE; i++)
  {
    memTemp[i] = pgm_read_byte_near(hardcodedState + sizeof(cpu_state_t) + i);
  }
  cpuState->memory = memTemp;
  cpu_set_state(cpuState);
  Serial.println("Hardcoded");
}
