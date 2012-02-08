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
#include "jtypes.h"
#include "joutofboundsexception.h"

namespace jcommon {

/**
 * C++ version 0.4 char* style "itoa":
 * Written by Lukás Chmela
 * Released under GPLv3.
 */
char * IntToStr(int64_t value, char *result, int radix) 
{
	if (radix < 2 || radix > 36) { 
		*result = '\0'; 
		
		return result; 
	}

	int64_t tmp_value;
	char *ptr = result, 
		*ptr1 = result, 
		tmp_char;

	do {
		tmp_value = value;
		value /= radix;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * radix)];
	} while ( value );

	if (tmp_value < 0) {
		*ptr++ = '-';
	}

	*ptr-- = '\0';

	while(ptr1 < ptr) {
		tmp_char = *ptr;
		*ptr--= *ptr1;
		*ptr1++ = tmp_char;
	}

	return result;
}

Types::Types():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Types");
}

Types::~Types()
{
}

int Types::StringToInteger(std::string s, int radix)
{
	if (radix < 2 || radix > 32) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
	return (int)strtol(s.c_str(), NULL, radix);
}

std::string Types::IntegerToString(int i, int radix)
{
	if (radix < 2 || radix > 32) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
	char tmp[32];

	return std::string(IntToStr((int64_t)i, tmp, radix));
}

int64_t Types::StringToLong(std::string s, int radix)
{
	if (radix < 2 || radix > 32) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
#ifdef _WIN32
	return (int64_t)strtol(s.c_str(), NULL, radix);
#else 
	return (int64_t)strtoll(s.c_str(), NULL, radix);
#endif
}

std::string Types::LongToString(int64_t i, int radix)
{
	if (radix < 2 || radix > 32) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
	char tmp[32];

	return std::string(IntToStr((int64_t)i, tmp, radix));
}

float Types::StringToFloat(std::string s)
{
#ifdef _WIN32
	return (float)strtod(s.c_str(), NULL);
#else
	return strtof(s.c_str(), NULL);
#endif
}

std::string Types::FloatToString(float i, int radix)
{
	if (radix != 8 && radix != 10 && radix != 16) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
	std::ostringstream o;

	if (radix == 8) {
		o << std::oct << i << std::flush;
	} else if (radix == 10) {
		o << std::dec << i << std::flush;
	} else if (radix == 16) {
		o << std::hex << i << std::flush;
	}

	return o.str();
}

double Types::StringToDouble(std::string s)
{
	return strtod(s.c_str(), NULL);
}

std::string Types::DoubleToString(double i, int radix)
{
	if (radix != 8 && radix != 10 && radix != 16) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
	std::ostringstream o;

	if (radix == 8) {
		o << std::oct << i << std::flush;
	} else if (radix == 10) {
		o << std::dec << i << std::flush;
	} else if (radix == 16) {
		o << std::hex << i << std::flush;
	}

	return o.str();
}

}

