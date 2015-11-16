#include <EEPROM.h>

void formatStorage() {
  for (int i = 0; i < 512; i++) EEPROM.write(i, 0);
  saveDefaultStorageValues(); 
}

void saveDefaultStorageValues() {
  saveBoolean(EEPROM_LOCKED, false);
  saveBoolean(EEPROM_DRIVING_LIGHTS, true);
  saveBoolean(EEPROM_STANDBY_LIGHTS, true);
  saveByte(EEPROM_FRONT_SENSOR, 20);
  saveByte(EEPROM_BACK_SENSOR, 20);
}

void saveInt(int address, int value) {
  EEPROM.write(address, highByte(value));
  EEPROM.write(address + 1, lowByte(value));
}

int loadInt(int address) {
  byte high = EEPROM.read(address);
  byte low = EEPROM.read(address + 1);
  return int(word(high, low));
}

void saveLong(int address, unsigned long value) {
  byte four = (value & 0xFF);
  byte three = ((value >> 8) & 0xFF);
  byte two = ((value >> 16) & 0xFF);
  byte one = ((value >> 24) & 0xFF);
  
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}

long loadLong(int address) {
  unsigned long four = EEPROM.read(address);
  unsigned long three = EEPROM.read(address + 1);
  unsigned long two = EEPROM.read(address + 2);
  unsigned long one = EEPROM.read(address + 3);
  
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void saveByte(int address, byte value) {
  EEPROM.write(address, value);
}

byte loadByte(int address) {
  EEPROM.read(address);
}

void saveBoolean(int address, boolean value) {
  EEPROM.write(address, value);
}

boolean loadBoolean(int address) {
  return EEPROM.read(address);
}
