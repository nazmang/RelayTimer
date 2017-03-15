#ifndef SYNCPROVIDER_RTC
#define SYNCPROVIDER_NTP
#else
#undef SYNCPROVIDER_NTP
#define SYNCPROVIDER_RTC
#endif

#if defined(ARDUINO_ARCH_ESP8266)

#else

#endif

// NTP Servers:
#define NTP_IP_ADDRESS 193,34,155,3 //pool.ntp.org

#define TIME_ZONE  2

#define EEPROM_TYPE_INTERNAL

#define I2C_EEP_ADDRESS

#define DEBUG_ENABLED

#define DBG_OUTPUT_PORT Serial

#define PIN_LEVEL_MODE 1 // 0 - Normal, 1 - Revert  
