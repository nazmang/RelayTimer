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


typedef enum  { RELAY_DISABLED = -1, RELAY_OFF, RELAY_ON } RelayState;

typedef void(*on_callback)();
typedef void(*off_callback)();


typedef struct {
	time_t start1;
	time_t end1;
	RelayState state;
	char name[20];
} __attribute__((packed)) RelayConfig;




class RelayTimer : public Timer {
public:

	RelayTimer(uint8_t pin);
	RelayTimer(uint8_t pin, int addr);

	void on();
	void off();
	void on(on_callback);
	void off(off_callback);
	bool enabled();
	RelayState state();
	void enable();
	void disable();
	void begin();
	void rename(const char* new_name);
	void rename(const String s);
	bool state_changed();
	const String name();
	void process();
	bool locked();
	/*bool set(uint8_t _hour, uint8_t _min, uint8_t _sec, uint32_t _long);
	bool set(uint8_t h1, uint8_t m1, uint8_t s1, uint8_t h2, uint8_t m2, uint8_t s2);
	bool set(time_t start, time_t end);
	void set_start(time_t);
	void set_end(time_t);*/


protected:
	bool set_state(RelayState rs);
	String _name;
	RelayState _current;
	RelayState _previous;
	uint8_t pin_num;
	bool _locked;
	size_t _addr;
	void lock();
	void unlock();


private:
	RelayConfig _rc;
	void readConfig();
	void writeConfig();


};





#ifdef DEBUG_ENABLED
void DebugPrint(const char *fmt, ...);
#endif