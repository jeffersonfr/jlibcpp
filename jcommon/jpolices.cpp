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
#include "jpolices.h"
#include "jruntimeexception.h"
#include "jstringutils.h"
#include "jfile.h"
#include "jautolock.h"

#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

namespace jcommon {

Polices::Polices()
{
}

Polices::~Polices()
{
}

void Polices::Load(std::string filename, std::string escape)
{
	jthread::AutoLock lock(&_mutex);

	_filename = filename;

	jio::File file(_filename);

	int r,
			state = 0;
	char c;

	std::string id,
		content;

	while ((r = file.Read((char *)&c, 1)) != EOF) {
		if (state == 0) {
			id = "";
			content = "";

			if (isalnum(c) == true) {
				id = id + c;
				state = 1;
			}
		} else if (state == 1) {
			if (c != '{' && c != '\n') {
				id = id + c;
			} else {
				state = 2;
			}
		} else if (state == 2) {
			if (c != '}' ) {
				content = content + c;
			} else {
				_polices[jcommon::StringUtils::Trim(id)] = jcommon::StringUtils::Trim(content);

				state = 0;
			}
		}
	}
}

void Polices::Save(std::string escape)
{
	jthread::AutoLock lock(&_mutex);

	try {
		jio::File f(_filename, jio::F_WRITE_ONLY | jio::F_LARGEFILE | jio::F_TRUNCATE);

		for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
			std::ostringstream o;

			o << i->first << " {\n" << i->second << "\n}\n" << std::endl;

			f.Write(o.str().c_str(), (uint64_t)o.str().size());
		}

		f.Flush();
	} catch (...) {
		throw jcommon::RuntimeException("File not found !");
	}
}

void Polices::AddPolice(std::string police)
{
	jthread::AutoLock lock(&_mutex);

	_polices[police];
}

std::vector<std::string> * Polices::GetPolices()
{
	jthread::AutoLock lock(&_mutex);

	std::vector<std::string> *polices = new std::vector<std::string>();

	for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
		polices->push_back(i->first);
	}

	return polices;
}

std::string Polices::GetPoliceByName(std::string police)
{
	jthread::AutoLock lock(&_mutex);

	std::map<std::string, std::string>::iterator i = _polices.find(police);
	
	if (i != _polices.end()) {
		return i->second;
	}

	throw jcommon::RuntimeException("Index out of bounds exception");
}

std::string Polices::GetPoliceByIndex(int index)
{
	jthread::AutoLock lock(&_mutex);

	if (index >= 0 && index < (int)_polices.size()) {
		int k = 0;

		for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++, k++) {
			if (k == index) {
				return i->second;
			}
		}
	}

	throw jcommon::RuntimeException("Index out of bounds exception");
}

void Polices::RemovePoliceByName(std::string police)
{
	jthread::AutoLock lock(&_mutex);

	std::map<std::string, std::string>::iterator i = _polices.find(police);
	
	if (i == _polices.end()) {
		throw jcommon::RuntimeException("Cannot find police");
	}

	_polices.erase(i);
}

void Polices::RemovePoliceByIndex(int index)
{
	jthread::AutoLock lock(&_mutex);

	if (index >= 0 && index < (int)_polices.size()) {
		int k = 0;

		for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++, k++) {
			if (k == index) {
				_polices.erase(i);

				break;
			}
		}
	}
}

void Polices::SetPoliceContent(std::string police, std::string value)
{
	jthread::AutoLock lock(&_mutex);

	std::map<std::string, std::string>::iterator i = _polices.find(police);
	
	if (i == _polices.end()) {
		throw jcommon::RuntimeException("Cannot find police");
	}
		
	_polices[police] = value;
}

}

