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

#define STRING_EMPTY_CHARS " \n\r\t"

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

std::string StringUtils::LeftTrim(std::string str)
{
	return str.erase(0, str.find_first_not_of(STRING_EMPTY_CHARS));
}

std::string StringUtils::RightTrim(std::string str)
{
	return str.erase(str.find_last_not_of(STRING_EMPTY_CHARS)+1);
}

std::string StringUtils::Trim(std::string str)
{
	return str.erase(0, str.find_first_not_of(STRING_EMPTY_CHARS)).erase(str.find_last_not_of(STRING_EMPTY_CHARS)+1);
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
		s.erase(x, old.size());
		s.insert(x, brand);

		x = s.find(old, x+1);
	}

	return s;
}

}

