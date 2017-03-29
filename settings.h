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

#define TIME_ZONE 3

#define USE_DAYLIGHT_SAVING 

#define EEPROM_TYPE_INTERNAL

#define I2C_EEP_ADDRESS

#define DEBUG_ENABLED

#define DBG_OUTPUT_PORT Serial

#define PIN_LEVEL_MODE 1 // 0 - Normal, 1 - Revert  


#define DHTPIN 5     // GPIO 2 pin of ESP8266
#define DHTTYPE DHT22   // DHT 22  (AM2302)