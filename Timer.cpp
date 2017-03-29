#include "Timer.h"


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

time_t actualTime(){
	
}

Timer::Timer() /*:_start(0), _end(0)*/
{
	breakTime(0, tm_start);
	breakTime(0, tm_end);
}

time_t Timer::get_start()
{
	return makeTime(tm_start);
}

time_t Timer::get_end()
{
	return makeTime(tm_end);
}

bool Timer::set_start(time_t st)
{
	TimeElements tm;
	breakTime(st, tm);
	if (tm.Hour >= tm_end.Hour){
		if (tm.Minute >= tm_end.Minute){
			_changed = false;
			return false;
		}
	}
	if (tm_start.Hour != tm.Hour || tm_start.Minute != tm.Minute || tm_start.Second != tm.Second)
	{
		breakTime(st, tm_start);
		return _changed = true;
	}
	return false;
}

bool Timer::set_end(time_t en)
{
	TimeElements tm;
	breakTime(en, tm);
	if (tm.Hour <= tm_start.Hour){
		if (tm.Minute <= tm_start.Minute){
			_changed = false;
			return false;
		}
	}
	if (tm_end.Hour != tm.Hour || tm_end.Minute != tm.Minute || tm_end.Second != tm.Second)
	{
		breakTime(en, tm_end);
		return _changed = true;
	}
	return false;
}

bool Timer::set(uint8_t _hour, uint8_t _min, uint8_t _sec, uint32_t _long)
{
	TimeElements tms;
	tms.Hour = _hour;
	tms.Minute = _min;
	tms.Second = _sec;
	if (set_start(makeTime(tms)) || set_end(makeTime(tms) + _long)){
		return _changed = true;
	}
	return _changed = false;
}

bool Timer::set(uint8_t h1, uint8_t m1, uint8_t s1, uint8_t h2, uint8_t m2, uint8_t s2)
{
	TimeElements tms, tme;
	tms.Hour = h1;
	tms.Minute = m1;
	tms.Second = s1;
	tme.Hour = h2;
	tme.Minute = m2;
	tme.Second = s2;

	if (set_start(makeTime(tms)) || set_end(makeTime(tme))){
		return _changed = true;
	}
	
	return _changed = false;
}

bool Timer::set(time_t start, time_t end)
{
	if (start > end) {
		_changed = false;
		return false;
	}
	if (set_start(start) || set_end(end)){
		return _changed = true;

	}

	return _changed = false;
}

bool Timer::is_set()
{
	return makeTime(tm_start) > 0 && makeTime(tm_end) > 0;
}


bool Timer::is_changed()
{
	if (this->is_set()) return _changed;
	return false;
}



//bool Timer::operator>=(const time_t& rhs) const{
//	TimeElements tm_min, tm_max;
//	breakTime(rhs, tm_min);
//	breakTime(this->_end, tm_max);
//
//	return(tm_max.Hour > tm_min.Hour || tm_max.Minute > tm_min.Minute);	
//}
//
//bool Timer::operator<=(const time_t& rhs) const{
//	TimeElements tm_min, tm_max;
//	breakTime(rhs, tm_max);
//	breakTime(this->_start, tm_min);
//	if (tm_max.Hour >= tm_min.Hour){
//		if (tm_max.Minute >= tm_min.Minute) return true;
//	}
//	return false;
//}


//Timer& Timer::operator+=(uint32_t _long)
//{
//	_end = _start + _long;
//	return *this;
//}


//bool Timer::operator==(const time_t& rhs) const{
//	TimeElements tm_min, tm_max;
//	breakTime(rhs, tm_min);
//	breakTime(this->_start, tm_max);
//	return(tm_max.Hour == tm_min.Hour && tm_max.Minute == tm_min.Minute && tm_max.Second == tm_min.Second);
//}
//
//bool Timer::operator!=(const time_t& rhs) const{
//	TimeElements tm;
//	breakTime(rhs, tm);
//	return !(*this == tm);
//}