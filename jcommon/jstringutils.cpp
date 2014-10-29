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
#include "jstringutils.h"

namespace jcommon {

struct property_t {
	std::string params[2];
	int count;
};

StringUtils::StringUtils():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::StringUtils");
}

StringUtils::~StringUtils()
{
}

std::string StringUtils::Format(const char *format, ...)
{
	std::string str;
	va_list ap, cp;
	int format_length;
	char *dst = NULL;

	va_start(ap, format);
	va_copy(cp, ap);

	format_length = vsnprintf(NULL, 0, format, cp);

	if ((dst = (char *)malloc((format_length + 1) * sizeof(char))) != NULL) {
		vsprintf(dst, format, ap);
		
		str = std::string(dst);

		free(dst);
	}

	va_end(ap);
	va_end(cp);

	return str;
}

std::string StringUtils::LeftTrim(std::string str, std::string chars)
{
	if (str.size() > 0) {
		std::string::size_type idx = str.find_first_not_of(chars);

		if (idx != std::string::npos) {
			str = str.substr(idx);
		}
	}

	return str;
	
	// return str.erase(0, str.find_first_not_of(STRING_EMPTY_CHARS));
}

std::string StringUtils::RightTrim(std::string str, std::string chars)
{
	if (str.size() > 0) {
		std::string::size_type idx = str.find_last_not_of(chars);

		if (idx != std::string::npos) {
			str = str.substr(0, idx+1);
		}
	}

	return str;
	
	// return str.erase(str.find_last_not_of(STRING_EMPTY_CHARS)+1);
}

std::string StringUtils::Trim(std::string str, std::string chars)
{
	if (str.size() > 0) {
		std::string::size_type idx = str.find_first_not_of(chars);

		if (idx != std::string::npos) {
			str = str.substr(idx);
		}

		idx = str.find_last_not_of(chars);

		if (idx != std::string::npos) {
			str = str.substr(0, idx+1);
		}
	}

	return str;
	
	// return LeftTrim(RightTrim(str, chars), chars);
}

std::string StringUtils::ToLower(std::string str)
{
	for (int i=0; i<(int)str.size(); i++) {
		str[i] = tolower(str[i]);
	}

	return str;
}

std::string StringUtils::ToUpper(std::string str)
{
	for (int i=0; i<(int)str.size(); i++) {
		str[i] = toupper(str[i]);
	}

	return str;
}

std::string StringUtils::ReplaceString(std::string s, std::string old, std::string brand)
{
	std::string::size_type x = s.find(old);

	while (x != std::string::npos) {
		s = s.substr(0, x) + brand + s.substr(x+1);

		// s.erase(x, old.size());
		// s.insert(x, brand);

		x = s.find(old, x+brand.size()+1);
	}

	return s;
}

}

