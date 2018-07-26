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
#include "jcommon/jdate.h"
#include "jcommon/jcalendar.h"
#include "jexception/jruntimeexception.h"

#include <sstream>

#include <sys/time.h>

#include <string.h>

namespace jcommon {

Date::Date():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
	_time = time(NULL);

	memset(&_zone, 0, sizeof(struct tm));

	struct tm *zone = localtime(&_time);
	
	if (zone != NULL) {
		_zone = *zone;
	}
}

Date::Date(time_t time_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
	_time = time_;
	
	memset(&_zone, 0, sizeof(struct tm));

	struct tm *zone = localtime(&_time);
	
	if (zone != NULL) {
		_zone = *zone;
	}
}

Date::Date(int day, int month, int year):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
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

	memset(&_zone, 0, sizeof(struct tm));

	struct tm *zone = localtime(&_time);
	
	if (zone != NULL) {
		_zone = *zone;
	}
}

Date::Date(int day, int month, int year, int hours, int minutes, int seconds):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Date");
	
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

	memset(&_zone, 0, sizeof(struct tm));

	struct tm *zone = localtime(&_time);
	
	if (zone != NULL) {
		_zone = *zone;
	}
}

Date::Date(double julian)
{
	jcommon::Object::SetClassName("jcommon::Date");

	_time = (time_t)((julian-2299160.5)*864000000000.0);
	
	memset(&_zone, 0, sizeof(struct tm));

	struct tm *zone = localtime(&_time);
	
	if (zone != NULL) {
		_zone = *zone;
	}
}

Date::~Date()
{
}

uint32_t Date::ToJulian()
{
	int d, m, y;

	d = _zone.tm_mday;
	m = _zone.tm_mon+1;
	y = 1900 + _zone.tm_year;

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
	return (uint64_t)time(NULL);
}

uint64_t Date::CurrentTimeMillis()
{
	timeval t;

	gettimeofday(&t, NULL);

	return (uint64_t)t.tv_sec*1000LL + (uint64_t)t.tv_usec/1000LL;
}

uint64_t Date::CurrentTimeMicros()
{
	timeval t;

	gettimeofday(&t, NULL);

	return (uint64_t)t.tv_sec*1000000LL + (uint64_t)t.tv_usec;
}

uint64_t Date::GetMonotonicTime()
{
	struct timespec t;

	clock_gettime(CLOCK_MONOTONIC_RAW, &t);

	return (uint64_t)t.tv_sec*1000000000LL + (uint64_t)t.tv_nsec;
}

uint64_t Date::GetThreadTime()
{
	struct timespec t;

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, &t);

	return (uint64_t)t.tv_sec*1000000000LL + (uint64_t)t.tv_nsec;
}

uint64_t Date::GetProcessTime()
{
	struct timespec t;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t);

	return (uint64_t)t.tv_sec*1000000000LL + (uint64_t)t.tv_nsec;
}

uint64_t Date::GetTime()
{
	return _time;
}

int Date::GetDayOfMonth()
{
	return _zone.tm_mday;
}

int Date::GetMonth()
{
	return _zone.tm_mon+1;
}

int Date::GetYear()
{
	return (1900 + _zone.tm_year);
}

int Date::GetSecond()
{
	return _zone.tm_sec;
}

int Date::GetMinute()
{
	return _zone.tm_min;
}

int Date::GetHour()
{
	return _zone.tm_hour;
}

int Date::GetDayOfWeek()
{
	// INFO:: doming, segunda, terca, quarta, quinta, sexta, sabado
	// return _zone.tm_wday;

	// INFO:: segunda, terca, quarta, quinta, sexta, sabado, domingo
	int w = ((GetYear()-1901)*365+(GetYear()-1901)/4+GetDayOfMonth()+(GetMonth()-1)*31-((GetMonth()*4+23)/10)*((GetMonth()+12)/15)+((4-GetYear()%4)/4)*((GetMonth()+12)/15)+1)%7;

	if (w < 0) {
		w = ((w + 7) + 5) % 7;
	}

	return w;
}

int Date::GetDayOfYear()
{
	return _zone.tm_yday;
}

std::string Date::what()
{
	std::ostringstream date;

	std::string month[] = {
		"Jan",
		"Fev",
		"Mar",
		"Abr",
		"Mai",
		"Jun",
		"Jul",
		"Ago",
		"Set",
		"Out",
		"Nov",
		"Dez"
	};
	std::string week[] = {
		"Seg",
		"Ter",
		"Qua",
		"Qui",
		"Sex",
		"Sab",
		"Dom",
	};
		
	date << week[GetDayOfWeek()-1] << " " << month[GetMonth()-1] << " " << GetDayOfMonth() << " " << GetHour() << ":" << GetMinute() << ":" << GetSecond() << " " << GetYear();
	
	return date.str();
}

}

