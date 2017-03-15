#include <inttypes.h>
//#include <avr/pgmspace.h>
#ifdef __cplusplus
#include <Arduino.h>
/*#include <string.h>
#include <stdint.h>*/
#include <stdarg.h>
#endif
#include <EEPROM.h>
#include "Timer.h"
#include "settings.h"


typedef enum  { RELAY_DISABLED =-1, RELAY_OFF, RELAY_ON } RelayState;



typedef struct {
	time_t start1;
	time_t end1;
	time_t start2;
	time_t end2;
	RelayState state;
	char name[20];
} __attribute__((packed)) RelayConfig;

class RelayTimer {
public:

	RelayTimer(uint8_t pin);
	RelayTimer(uint8_t pin, int addr);
	//RelayTimer& operator=(RelayTimer&  rhs);
	void on();
	void off();
	void on(void(*)());
	void off(void(*)());
	bool enabled();
	RelayState state();
	void enable();
	void disable();
	void begin();
	void rename(const char* new_name);
	void rename(const String s);
	bool set_state(RelayState rs); // Will be protected!
	//const char* name();
	const String name();
	void process();

	Timer t1;
	Timer t2;

protected:
	String _name;
	RelayState _current;
	RelayState _previous;
	uint8_t pin_num;
	//
	
	uint16_t read_data();
	void save_data();
	size_t _addr;	
	
};


void writeConfig(RelayTimer &rt, int addr); // Will be moved to class body
void readConfig(RelayTimer &rt, int addr); // Will be moved to class body
time_t convertTime(char const *str);
String convertTime(const time_t t);

#ifdef DEBUG_ENABLED
void DebugPrint(const char *fmt, ...);
#endif