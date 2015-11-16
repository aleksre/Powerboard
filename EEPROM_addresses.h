// Boolean: 1 address -- Byte: 1 address -- Int: 2 addresses -- Long: 4 addresses 

#define EEPROM_LOCKED 0          // boolean
#define EEPROM_DRIVING_LIGHTS 1  // boolean
#define EEPROM_STANDBY_LIGHTS 2  // boolean
#define EEPROM_SPEED_CAP 3       // byte (can be 0-2)
#define EEPROM_CURRENT_TRIP 4    // long: 4-5-6-7
#define EEPROM_TOTAL_TRIP 8      // long: 8-9-10-11
#define EEPROM_FRONT_SENSOR 12   // byte
#define EEPROM_BACK_SENSOR 13    // byte
