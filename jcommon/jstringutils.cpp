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
#include "jstringutils.h"
#include "jruntimeexception.h"

#include <sstream>
#include <iostream>
#include <vector>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

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
	return str.erase(0, str.find_first_not_of(" "));

	/*
	if ((*str) == NULL) {
		return 0;
	}
	
	char *p;
	int count;

	p = (*str);
	
	while (*p == ' ') {
		p++;
	}

	if (p == (*str)) {
		return length;
	}

	count = p - (*str);

	memcpy((*str), p, length - count + 1);

	(*str)[length - count + 1] = '\0';

	return (length - count);
	*/
}

std::string StringUtils::RightTrim(std::string str)
{
	return str.erase(str.find_last_not_of(" ")+1, str.size());

	/*
	if ((*str) == NULL) {
		return 0;
	}
	
	char *q;

	q = (*str) + length - 1;

	while (*q == ' ' && q != (*str)) {
		q--;
	}

	if (q == ((*str) + length - 1)) {
		return length;
	}

	*(++q) = '\0';

	return ((*str) - q - 1);
	*/
}

std::string StringUtils::Trim(std::string str)
{
	std::string tmp;

	tmp = str.erase(0, str.find_first_not_of(" "));
	
	return tmp.erase(tmp.find_last_not_of(" ")+1, tmp.size());

	/*
	int length;

	if (limit > 10*1024*1024) { // 10 MB
		return -1;
	}

#ifdef _WIN32
	length = strlen(*str);
#else
	length = strnlen(*str, limit);
#endif
	length = LeftTrim(str, length);

	if (length == -1) {
		return -1;
	}
	
	length = RightTrim(str, length);
	
	return length;
	*/
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
	unsigned long x = s.find(old);

	while (x != std::string::npos) {
		s.erase(x, old.size());
		s.insert(x, brand);

		x = s.find(old, x+1);
	}

	return s;
}

}

