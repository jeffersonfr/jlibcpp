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
#include "jtypes.h"
#include "joutofboundsexception.h"

#include <sstream>

#include <sys/time.h>
#include <unistd.h>

namespace jcommon {

Types::Types():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Types");
}

Types::~Types()
{
}

int StringToInteger(std::string s, int radix)
{
	if (radix < 2 || radix > 32) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
	return (int)strtol(s.c_str(), NULL, radix);
}

std::string IntegerToString(int i, int radix)
{
	return "";
}

long long StringToLong(std::string s, int radix)
{
	if (radix < 2 || radix > 32) {
		throw OutOfBoundsException("Bounds of radix exception");
	}
	
#ifndef __CYGWIN32__
	return (long long)strtoll(s.c_str(), NULL, radix);
#else 
	// TODO:: cygwin
	return (long long)strtol(s.c_str(), NULL, radix);
#endif
}

std::string LongToString(long long i, int radix)
{
	return "";
}

float StringToFloat(std::string s)
{
	return strtof(s.c_str(), NULL);
}

std::string FloatToString(float i, int radix)
{
	return "";
}

double StringToDouble(std::string s)
{
	return strtod(s.c_str(), NULL);
}

std::string DoubleToString(double i, int radix)
{
	return "";
}

std::string LongToRadix(long long i, int radix)
{
	return "";
}

}

