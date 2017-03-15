#include "Timer.h"

/*bool Timer::operator>=(const time_t& rhs) const{
	TimeElements tm_min, tm_max;
	breakTime(rhs, tm_min);
	breakTime(this->_end, tm_max);

	return(tm_max.Hour > tm_min.Hour || tm_max.Minute > tm_min.Minute);	
}

bool Timer::operator<=(const time_t& rhs) const{
	TimeElements tm_min, tm_max;
	breakTime(rhs, tm_max);
	breakTime(this->_start, tm_min);
	if (tm_max.Hour >= tm_min.Hour){
		if (tm_max.Minute >= tm_min.Minute) return true;
	}
	return false;
}*/

/*bool Timer::operator>=(const time_t& rhs) const{
	TimeElements tm_min, tm_max;
	breakTime(rhs, tm_min);
	breakTime(this->_start, tm_max);
	return(tm_max.Hour >= tm_min.Hour || tm_max.Minute >= tm_min.Minute || tm_max.Second >= tm_min.Second);
}

bool Timer::operator<=(const time_t& rhs) const{
	TimeElements tm_min, tm_max;
	breakTime(rhs, tm_max);
	breakTime(this->_end, tm_min);
	return(tm_max.Hour >= tm_min.Hour && tm_max.Minute >= tm_min.Minute);
}*/

//Timer& Timer::operator+=(uint32_t _long)
//{
//	_end = _start + _long;
//	return *this;
//}

/*bool Timer::operator==(const time_t& rhs) const{
	TimeElements tm_min, tm_max;
	breakTime(rhs, tm_min);
	breakTime(this->_start, tm_max);
	return(tm_max.Hour == tm_min.Hour && tm_max.Minute == tm_min.Minute && tm_max.Second == tm_min.Second);
}*/

/*bool TimerElements::operator!=(const time_t& rhs) const{
	TimerElements tm;
	breakTime(rhs, tm);
	return !(*this == tm);
}*/

Timer::Timer() :_start(0), _end(0)
{
	_unset = true;
}

time_t Timer::get_start()
{
	return _start;
}

time_t Timer::get_end()
{
	return _end;
}

void Timer::set_start(time_t st)
{
	_start = st;
}

void Timer::set_end(time_t en)
{
	_end = en;
}

bool Timer::set(uint8_t _hour, uint8_t _min, uint8_t _sec, uint32_t _long)
{
	TimeElements tm;
	tm.Hour = _hour;
	tm.Minute = _min;
	tm.Second = _sec;
	time_t _temp = makeTime(tm);

	if (_temp != _start && (_start + _long) != _end) {

		if (_start > _end) {
			_changed = false;
			return false;
		}
		_start = makeTime(tm);
		_end = _start + _long;
		_changed = true;
		_unset = false;
		return true;
	}
	_changed = false;
	_unset = true;
	return false;
}

bool Timer::set(uint8_t h1, uint8_t m1, uint8_t s1, uint8_t h2, uint8_t m2, uint8_t s2)
{
	TimeElements tm1, tm2;
	tm1.Hour = h1;
	tm1.Minute = m1;
	tm1.Second = s1;
	tm2.Hour = h2;
	tm2.Minute = m2;
	tm2.Second = s2;
	time_t _temp1 = makeTime(tm1);
	time_t _temp2 = makeTime(tm2);

	if (_temp1 != _start || _temp2 != _end)
	{
		if (_start > _end) {
			_changed = false;
			return false;
		}
		_start = makeTime(tm1);
		_end = makeTime(tm2);
		_unset = false;
		_changed = true;
		return true;
	}
	_changed = false;
	_unset = true;
	return false;
}

bool Timer::set(time_t start, time_t end)
{
	
	//if (start != 0 && end != 0) {
		if (start > end) {
			_changed = false;
			return false;
		}
		if (_start != start || _end != end){
			_start = start;
			_end = end;
			_unset = false;
			_changed = true;
			return true;			
		}
	//}
	_changed = false;
	_unset = true;
	return false;
}

bool Timer::is_set()
{
	return _unset;
}


bool Timer::is_changed()
{
	
	if (this->is_set()) return _changed;
	return false;
}
