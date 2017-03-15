#pragma once
#include <Time.h>
#include <TimeLib.h>


/*class TimerElements : private TimeElements{
	bool operator==(const TimerElements& rhs) const;
	bool operator>(const TimerElements& rhs) const;
	bool operator<(const TimerElements& rhs) const;
	bool operator>=(const TimerElements& rhs) const;
	bool operator<=(const TimerElements& rhs) const;
	bool operator>(const time_t& rhs) const;
	bool operator<(const time_t& rhs) const;
	bool operator==(const time_t& rhs) const;
	bool operator!=(const time_t& rhs) const;
};*/


class Timer {

public:
	Timer();
	
protected:
	time_t _start;
	time_t _end;
	bool _unset = true;
	bool _changed = false;

public:
	
	time_t get_start();
	time_t get_end();
	void set_start(time_t);
	void set_end(time_t);
	bool set(uint8_t _hour, uint8_t _min, uint8_t _sec, uint32_t _long);
	bool set(uint8_t h1, uint8_t m1, uint8_t s1, uint8_t h2, uint8_t m2, uint8_t s2);
	bool set(time_t start, time_t end);
	Timer& operator+=(uint32_t _long);
	//bool operator>=(const time_t& rhs) const;
	//bool operator<=(const time_t& rhs) const;
	//bool operator>(const time_t& rhs) const;
	//bool operator<(const time_t& rhs) const;
	//bool operator==(const Timer& rhs) const;
	//bool operator>(const Timer& rhs) const;
	//bool operator<(const Timer& rhs) const;
	//bool operator==(const time_t& rhs) const;
	//bool operator!=(const time_t& rhs) const;
	bool is_set();
	bool is_changed();
};

