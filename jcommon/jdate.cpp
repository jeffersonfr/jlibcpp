/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "Stdafx.h"
#include "jdate.h"
#include "jruntimeexception.h"
#include "jcalendar.h"

namespace jcommon {

#ifdef _WIN32

// Number of 100 nanosecond units from 1/1/1601 to 1/1/1970
#define EPOCH_BIAS 116444736000000000i64

typedef union {
	unsigned __int64 ft_scalar;
	FILETIME ft_struct;
} FT;

uint64_t SystemTime2LongTime(SYSTEMTIME *stm) 
{
	FT nt_time;

	SystemTimeToFileTime(stm, &nt_time.ft_struct);

	return (uint64_t)((__time64_t)((nt_time.ft_scalar - EPOCH_BIAS) / 10000000i64));
}

#endif

Date::Date():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
#ifdef _WIN32
	GetLocalTime(&_zone);

	_time = (time_t)SystemTime2LongTime(&_zone);
#else
	_time = time(NULL);
	_zone = localtime(&_time);
	
	if (_zone == NULL) {
		try {
			_zone = new struct tm;
		} catch (std::bad_alloc &e) {
			jcommon::RuntimeException("Cannot allocate memory");
		}

		memset(_zone, 0, sizeof(struct tm));
	}
#endif
}

Date::Date(time_t time_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
	_time = time_;
	
#ifdef _WIN32
	int Y2, 
			M2, 
			Y, 
			M, 
			D, 
			K,
			hora,
			horas, 
			minutos, 
			segundos,
			MJD;

	MJD = (int)(_time >> 24);
	hora = (int)(_time & 0xffffff);

	Y2 = (int) ((MJD - 15078.2) / 365.25);
	M2 = (int) ((MJD - 14956.1 - (int) (Y2 * 365.25)) / 30.6001);
	D  = MJD - 14956 - (int) (Y2 * 365.25) - (int) (M2 * 30.6001);
	K  = M2 == 14 || M2 == 15 ? 1 : 0;
	Y  = Y2 + K;
	M  = M2 - 1 - K * 12;

	horas    = ((hora >> 20) & 0x0f) * 10 + ((hora >> 16) & 0x0f);
	minutos  = ((hora >> 12) & 0x0f) * 10 + ((hora >>  8) & 0x0f);
	segundos = ((hora >>  4) & 0x0f) * 10 + ((hora >>  0) & 0x0f);

	_zone.wDay = D; // 1-31
	_zone.wMonth = M; // 1-12
	_zone.wYear = Y + 1900; // 1601-30827
	_zone.wHour = horas; // 0-23
	_zone.wMinute = minutos; // 0-59
	_zone.wSecond = segundos; // 0-59
#else
	_zone = localtime(&_time);
	
	if (_zone == NULL) {
		try {
			_zone = new struct tm;
		} catch (std::bad_alloc &e) {
			jcommon::RuntimeException("Cannot allocate memory");
		}

		memset(_zone, 0, sizeof(struct tm));
	}
#endif
}

Date::Date(int day, int month, int year):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
#ifdef _WIN32
	GetLocalTime(&_zone);
	
	_zone.wDay = day; // 1-31
	_zone.wMonth = month; // 1-12
	_zone.wYear = year; // 1601-30827
	// _zone.wSecond = seconds; // 0-59
	// _zone.wMinute = minutes; // 0-59
	// _zone.wHour = hour; // 0-23

	_time = (time_t)SystemTime2LongTime(&_zone);
#else
	_time = time(NULL);

	struct tm t,
						*l = localtime(&_time);

	t.tm_sec = l->tm_sec;   // seconds (0 - 60) 
	t.tm_min = l->tm_min;   // minutes (0 - 59) 
	t.tm_hour = l->tm_hour; // hours (0 - 23) 
	t.tm_mday = day;    		// day of month (1 - 31) 
	t.tm_mon = month-1; 		// month of year (0 - 11) 
	t.tm_year = year-1900;	// year - 1900 
	t.tm_wday = 0;					// day of week (Sunday = 0) 
	t.tm_yday = 0;    			// day of year (0 - 365) 
	t.tm_isdst = 0;   			// is summer time in effect? 
	t.tm_zone = NULL;  			// abbreviation of timezone name 
	// long tm_gmtoff; 			// offset from UTC in seconds 

	_time = mktime(&t);
	_zone = localtime(&_time);
	
	if (_zone == NULL) {
		try {
			_zone = new struct tm;
		} catch (std::bad_alloc &e) {
			jcommon::RuntimeException("Cannot allocate memory");
		}

		memset(_zone, 0, sizeof(struct tm));
	}
#endif
}

Date::Date(int day, int month, int year, int hours, int minutes, int seconds):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
#ifdef _WIN32
	GetLocalTime(&_zone);
	
	_zone.wDay = day; // 1-31
	_zone.wMonth = month; // 1-12
	_zone.wYear = year; // 1601-30827
	_zone.wHour = hours; // 0-23
	_zone.wMinute = minutes; // 0-59
	_zone.wSecond = seconds; // 0-59

	_time = (time_t)SystemTime2LongTime(&_zone);
#else
	_time = time(NULL);

	struct tm t;

	t.tm_sec = seconds;   	// seconds (0 - 60) 
	t.tm_min = minutes;   	// minutes (0 - 59) 
	t.tm_hour = hours; 			// hours (0 - 23) 
	t.tm_mday = day;    		// day of month (1 - 31) 
	t.tm_mon = month-1; 		// month of year (0 - 11) 
	t.tm_year = year-1900;	// year - 1900 
	t.tm_wday = 0;					// day of week (Sunday = 0) 
	t.tm_yday = 0;    			// day of year (0 - 365) 
	t.tm_isdst = 0;   			// is summer time in effect? 
	t.tm_zone = NULL;  			// abbreviation of timezone name 
	// long tm_gmtoff; 			// offset from UTC in seconds 

	_time = mktime(&t);
	_zone = localtime(&_time);
	
	if (_zone == NULL) {
		try {
			_zone = new struct tm;
		} catch (std::bad_alloc &e) {
			jcommon::RuntimeException("Cannot allocate memory");
		}

		memset(_zone, 0, sizeof(struct tm));
	}
#endif
}

Date::Date(double julian)
{
	jcommon::Object::SetClassName("jcommon::Date");

	_time = (time_t)((julian-2299160.5)*864000000000.0);
	
#ifdef _WIN32
	int Y2, 
			M2, 
			Y, 
			M, 
			D, 
			K,
			hora,
			horas, 
			minutos, 
			segundos,
			MJD;

	MJD = (int)(_time >> 24);
	hora = (int)(_time & 0xffffff);

	Y2 = (int) ((MJD - 15078.2) / 365.25);
	M2 = (int) ((MJD - 14956.1 - (int) (Y2 * 365.25)) / 30.6001);
	D  = MJD - 14956 - (int) (Y2 * 365.25) - (int) (M2 * 30.6001);
	K  = M2 == 14 || M2 == 15 ? 1 : 0;
	Y  = Y2 + K;
	M  = M2 - 1 - K * 12;

	horas    = ((hora >> 20) & 0x0f) * 10 + ((hora >> 16) & 0x0f);
	minutos  = ((hora >> 12) & 0x0f) * 10 + ((hora >>  8) & 0x0f);
	segundos = ((hora >>  4) & 0x0f) * 10 + ((hora >>  0) & 0x0f);

	_zone.wDay = D; // 1-31
	_zone.wMonth = M; // 1-12
	_zone.wYear = Y + 1900; // 1601-30827
	_zone.wHour = horas; // 0-23
	_zone.wMinute = minutos; // 0-59
	_zone.wSecond = segundos; // 0-59
#else
	_zone = localtime(&_time);
	
	if (_zone == NULL) {
		try {
			_zone = new struct tm;
		} catch (std::bad_alloc &e) {
			jcommon::RuntimeException("Cannot allocate memory");
		}

		memset(_zone, 0, sizeof(struct tm));
	}
#endif
}

Date::~Date()
{
#ifdef _WIN32
#else
	if (_zone != NULL) {
		// free(_zone);
	}
#endif
}

uint32_t Date::ToJulian()
{
	int d, m, y;

#ifdef _WIN32
	d = _zone.wDay;
	m = _zone.wMonth;
	y = _zone.wYear;
#else
	d = _zone->tm_mday;
	m = _zone->tm_mon+1;
	y = 1900 + _zone->tm_year;
#endif

	int a1 = (14 - m) / 12;
	int y1 = y + 4800 - a1;
	int m1 = m + 12 * a1 - 3;

	return d + (153*m1 + 2) / 5 + y1*365 + y1/4 - y1/100 + y1/400 - 32045; // 1, 1, 1970
}

double Date::ToGregorian()
{
	return double(_time)/864000000000.0+2299160.5; // first day of Gregorian reform (Oct 15 1582)
}

uint64_t Date::CurrentTimeSeconds()
{
#ifdef _WIN32
	return (uint64_t)(GetTickCount64()/1000);
#else
	return (uint64_t)time(NULL);
#endif
}

uint64_t Date::CurrentTimeMillis()
{
#ifdef _WIN32
	return (uint64_t)GetTickCount64();
#else
	timeval t;

	gettimeofday(&t, NULL);

	return (uint64_t)t.tv_sec*1000LL + (uint64_t)t.tv_usec/1000LL;
#endif
}

uint64_t Date::CurrentTimeMicros()
{
#ifdef _WIN32
	FT nt_time;

	GetSystemTimeAsFileTime(&nt_time.ft_struct);

	return (uint64_t)((__time64_t)((nt_time.ft_scalar - EPOCH_BIAS) / 10000000i64));
#else
	timeval t;

	gettimeofday(&t, NULL);

	return (uint64_t)t.tv_sec*1000000LL + (uint64_t)t.tv_usec;
#endif
}

uint64_t Date::GetMonotonicTime()
{
#ifdef _WIN32
	FT nt_time;

	GetSystemTimeAsFileTime(&nt_time.ft_struct);

	return (uint64_t)((__time64_t)((nt_time.ft_scalar - EPOCH_BIAS) / 10000000i64));
#else
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t);

	return (uint64_t)t.tv_sec*1000000000LL + (uint64_t)t.tv_nsec;
#endif
}

uint64_t Date::GetThreadTime()
{
#ifdef _WIN32
	return 0LL;
#else
	struct timespec t;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);

	return (uint64_t)t.tv_sec*1000000000LL + (uint64_t)t.tv_nsec;
#endif
}

uint64_t Date::GetProcessTime()
{
#ifdef _WIN32
	return 0LL;
#else
	struct timespec t;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);

	return (uint64_t)t.tv_sec*1000000000LL + (uint64_t)t.tv_nsec;
#endif
}

uint64_t Date::GetTime()
{
	return _time;
}

int Date::GetDayOfMonth()
{
#ifdef _WIN32
	return _zone.wDay;
#else
	return _zone->tm_mday;
#endif
}

int Date::GetMonth()
{
#ifdef _WIN32
	return (_zone.wMonth);
#else
	return _zone->tm_mon+1;
#endif
}

int Date::GetYear()
{
#ifdef _WIN32
	return _zone.wYear;
#else
	return (1900 + _zone->tm_year);
#endif
}

int Date::GetSecond()
{
#ifdef _WIN32
	return _zone.wSecond;
#else
	return _zone->tm_sec;
#endif
}

int Date::GetMinute()
{
#ifdef _WIN32
	return _zone.wMinute;
#else
	return _zone->tm_min;
#endif
}

int Date::GetHour()
{
#ifdef _WIN32
	return _zone.wHour;
#else
	return _zone->tm_hour;
#endif
}

int Date::GetDayOfWeek()
{
#ifdef _WIN32
	return _zone.wDayOfWeek;
#else
	// INFO:: segunda, terca, quarta, quinta, sexta, sabado, domingo
	return ((GetYear()-1901)*365+(GetYear()-1901)/4+GetDayOfMonth()+(GetMonth()-1)*31-((GetMonth()*4+23)/10)*((GetMonth()+12)/15)+((4-GetYear()%4)/4)*((GetMonth()+12)/15)+1)%7;
	
	// return _zone->tm_wday;
#endif
}

int Date::GetDayOfYear()
{
#ifdef _WIN32
	Calendar c1(1, 1, _zone.wYear),
		 c2(_zone.wDay, _zone.wMonth, _zone.wYear);

	return c1.CountDays(&c2);
#else
	return _zone->tm_yday;
#endif
}

std::string Date::what()
{
	std::ostringstream date;
	std::string month;

	switch (GetMonth()) {
		case 1:  month = "Jan"; break;
		case 2:  month = "Feb"; break;
		case 3:  month = "Mar"; break;
		case 4:  month = "Apr"; break;
		case 5:  month = "May"; break;
		case 6:  month = "Jun"; break;
		case 7:  month = "Jul"; break;
		case 8:  month = "Ago"; break;
		case 9:  month = "Sep"; break;
		case 10:  month = "Oct"; break;
		case 11:  month = "Nov"; break;
		case 12:  month = "Dez"; break;
	}
		
	date << month << " " << GetDayOfMonth() << " " << GetHour() << ":" << GetMinute() << ":" << GetSecond() << " " << GetYear();
	
	return date.str();
}

}

