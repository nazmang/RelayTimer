#include "RelayTimer.h"
#include "settings.h"

#ifdef DEBUG_ENABLED
void DebugPrint(const char *fmt, ...){
	TimeElements tm;
	char fmtBuffer[300];
	int j = 0;

	breakTime(now(), tm);
	va_list args;
	va_start(args, fmt);
	
	j = snprintf(fmtBuffer, 10, PSTR("%d:%d:%d "), tm.Hour,tm.Minute,tm.Second);
	vsnprintf(fmtBuffer + j, 288, fmt, args);
	va_end(args);
	DBG_OUTPUT_PORT.print(fmtBuffer);
}
#endif

#ifdef EEPROM_TYPE_INTERNAL
static void initConfigBlock(size_t length = 1024 /*ATMega328 has 1024 bytes*/)
{
	static bool initDone = false;
	if (!initDone)
	{
		EEPROM.begin(length);
		initDone = true;
	}
}

void readConfigBlock(void* buf, void* adr, size_t length)
{
	initConfigBlock();
	uint8_t* dst = static_cast<uint8_t*>(buf);
	int offs = reinterpret_cast<int>(adr);
	while (length-- > 0)
	{
		*dst++ = EEPROM.read(offs++);
	}
}

void writeConfigBlock(void* buf, void* adr, size_t length)
{
	initConfigBlock();
	uint8_t* src = static_cast<uint8_t*>(buf);
	int offs = reinterpret_cast<int>(adr);
	while (length-- > 0)
	{
		EEPROM.write(offs++, *src++);
	}
	EEPROM.commit();
}

#elif defined EEPROM_TYPE_EXTERNAL
#include <Wire.h>
void i2c_eeprom_write_byte(unsigned int eeaddress, byte data ) {
	int rdata = data;
	Wire.beginTransmission(I2C_EEP_ADDRESS);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.write(rdata);
	Wire.endTransmission();
}

byte i2c_eeprom_read_byte(unsigned int eeaddress) {
	byte rdata = 0xFF;
	Wire.beginTransmission(I2C_EEP_ADDRESS);
	Wire.write((int)(eeaddress >> 8)); // MSB
	Wire.write((int)(eeaddress & 0xFF)); // LSB
	Wire.endTransmission();
	Wire.requestFrom(I2C_EEP_ADDRESS, 1);
	if (Wire.available()) rdata = Wire.read();
	return rdata;
}

void readConfigBlock(void* buf, void* adr, size_t length)
{
	uint8_t* dst = static_cast<uint8_t*>(buf);
	int offs = reinterpret_cast<int>(adr);
	while (length-- > 0)
	{
		*dst++ = i2c_eeprom_read_byte(offs++);
	}
}

void writeConfigBlock(void* buf, void* adr, size_t length)
{
	uint8_t* src = static_cast<uint8_t*>(buf);
	int offs = reinterpret_cast<int>(adr);
	while (length-- > 0)
	{
		i2c_eeprom_write_byte(offs++, *src++);
	}
}
#endif

void RelayTimer::writeConfig(){
	RelayConfig rc = {0};
	//if (rt.t1.is_set()){
	rc.start1 = get_start();
		rc.end1 = get_end();
	/*}
	if (rt.t1.is_set()){*/
		/*rc.start2 = rt.t2.start();
		rc.end2 = rt.t2.end();*/
	//}
		rc.state = state();
		memset(rc.name, 0, sizeof(rc.name));
		strncpy_P(rc.name, name().c_str(), sizeof(rc.name) - 1);
		//rc.name = rt.name();
	writeConfigBlock((void*)&rc, (void*)_addr, sizeof(RelayConfig));
	DBG_OUTPUT_PORT.printf("Write config to EEPROM for relay %s with state: %d\n", rc.name, rc.state);
}

void RelayTimer::readConfig(){
	RelayConfig rc;
	readConfigBlock((void *)&rc, (void*)(_addr), sizeof(rc));
	
	set(rc.start1, rc.end1);
	//rt.t2.set(rc.start2, rc.end2);
	set_state(rc.state);
	rename(rc.name);

	DBG_OUTPUT_PORT.printf("Config was read from EEPROM for relay %s with state: %d\n", name().c_str(), state());

}

time_t convertTime(char const *str){
	TimeElements tm;
	String s(str);
	time_t t;
	t = now();
	breakTime(t, tm);
	tm.Hour = s.substring(0, 2).toInt();
	tm.Minute = s.substring(3, 5).toInt();
	tm.Second = 0;
	t = makeTime(tm);

	return makeTime(tm);
}

String convertTime(const time_t t){
	TimeElements tm;
	breakTime(t, tm);
	String s = "";
	if (tm.Hour < 10) s = "0" + String(tm.Hour);
	else s = String(tm.Hour);

	s += ":";

	if (tm.Minute < 10) s += "0" + String(tm.Minute);
	else s += String(tm.Minute);

	return s;
}

RelayTimer::RelayTimer(uint8_t pin)
{
	pin_num = pin;
	_current = RELAY_OFF;
	_previous = RELAY_OFF;
	_name = ""; 
	_addr = -1;
#ifdef DEBUG_ENABLED
	DBG_OUTPUT_PORT.printf("\nNew relay init on pin: %d", this->pin_num);
#endif
}

RelayTimer::RelayTimer(uint8_t pin, int addr)
{
	pin_num = pin;
	_addr = addr;
	_current = RELAY_OFF;
	_previous = RELAY_OFF;
	_name = "";
#ifdef DEBUG_ENABLED
	DBG_OUTPUT_PORT.printf("\nNew relay init on pin: %d\n With address: %d", this->pin_num, this->_addr);
#endif
}

void RelayTimer::_on(){
	if (enabled()) {
		//digitalWrite(this->pin_num, LOW);
		if (set_state(RELAY_ON)) writeConfig();
#ifdef DEBUG_ENABLED
		DBG_OUTPUT_PORT.printf("\nGot: %d - try to set ON for relay %s \n", this->state(), this->name().c_str());
#endif
		
	}	
}

void RelayTimer::on(void(*fn)() = on_callback){
	if (enabled()) {
		_on();
		fn();
	}
}

void RelayTimer::off(void(*fn)() = off_callback){
	if (enabled()) {
		_off();
		fn();
	}
}

void RelayTimer::_off(){

	if (enabled()){
		//digitalWrite(this->pin_num, HIGH);
		if (set_state(RELAY_OFF))writeConfig();
#ifdef DEBUG_ENABLED
		DBG_OUTPUT_PORT.printf("\nGot: %d - try to set OFF state for relay %s \n", this->state(), this->name().c_str());
#endif
		
	}	
}

bool RelayTimer::enabled(){
	if (_current != RELAY_DISABLED) return true;
	return false;
}

RelayState RelayTimer::state(){
	
	return _current;
}

void RelayTimer::enable(){
	if (set_state(RELAY_OFF)) writeConfig();
#ifdef DEBUG_ENABLED
	DBG_OUTPUT_PORT.printf("\nGot: %d - try to ENABLE relay %s \n", this->state(), this->name().c_str());
#endif
	
	//digitalWrite(this->pin_num, HIGH);
}

void RelayTimer::disable(){
	if (set_state(RELAY_DISABLED)) writeConfig();
#ifdef DEBUG_ENABLED
	DBG_OUTPUT_PORT.printf("\nGot: %d - try to DISABLE relay %s \n", this->state(), this->name().c_str());
#endif
	
	//digitalWrite(this->pin_num, HIGH);
}

void RelayTimer::begin()
{
	if (timeStatus() != timeSet){
		DBG_OUTPUT_PORT.println("Time is not set. You should set time first!\n");
	}
	else {
#ifdef DEBUG_ENABLED
		DBG_OUTPUT_PORT.printf("Time was set in the system: %d\n", now());
#endif
		pinMode(this->pin_num, OUTPUT);
		digitalWrite(this->pin_num, HIGH);
	}
#ifdef DEBUG_ENABLED
	DBG_OUTPUT_PORT.printf("Begin reading data...\n");
#endif

	readConfig();
	
}

void RelayTimer::rename(const char* new_name)
{
	_name = new_name;
}

bool RelayTimer::set_state(RelayState rs){
	if (_current != rs){
		_previous = _current;
		_current = rs;
		switch (this->state()){
			case RELAY_DISABLED: {digitalWrite(this->pin_num, HIGH); break; }
			case RELAY_ON: {digitalWrite(this->pin_num, LOW); break; }
			case RELAY_OFF: {digitalWrite(this->pin_num, HIGH); break; }
		}
#ifdef DEBUG_ENABLED
		DBG_OUTPUT_PORT.printf("State changed for relay %s, current is: %d\n", _name.c_str(), _current);
#endif
		return true;
	}
#ifdef DEBUG_ENABLED
	else{
		DBG_OUTPUT_PORT.printf("State unchanged for relay %s, current is: %d\n", _name.c_str(), _current);
	}
#endif
	return false;
}

const String RelayTimer::name()
{
	return _name;
}

void RelayTimer::process()
{
	if (millis() % 3000 == 0) {
		DBG_OUTPUT_PORT.printf("Current state in process: %d for relay %s \n", this->state(), this->name().c_str());

		if (this->state() != RELAY_ON){
			if (_start <= now() && _end >= now()) { }

		}
		//else off();
	}
	if (millis() % 3000 == 0) {
		DBG_OUTPUT_PORT.printf("Current state process after: %d for relay %s \n", this->state(), this->name().c_str());
	}
	return; // Attention!!! Achtung!!! Don't comment or disable!!!
}
