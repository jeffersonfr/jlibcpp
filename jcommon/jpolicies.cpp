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
#include "jpolicies.h"
#include "jstringutils.h"
#include "jautolock.h"
#include "jfile.h"
#include "jioexception.h"

namespace jcommon {

Policies::Policies()
{
}

Policies::~Policies()
{
}

void Policies::Load(std::string filename, std::string escape)
{
	jthread::AutoLock lock(&_mutex);

	_filename = filename;

	jio::File file(_filename);

	std::string id,
		content;
	int64_t r,
		state = 0;
	char c;

	while ((r = file.Read((char *)&c, 1)) != EOF) {
		if (state == -1) {
			// remove comment from file
			if (c == '\n') {
				state = 0;
			}
		} else if (state == 0) {
			if (isalnum(c) != 0) {
				id = id + c;
				state = 1;
			} else if (c == '#') {
				state = -1;
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
				id = "";
				content = "";
			}
		}
	}
}

void Policies::Save(std::string escape)
{
	jthread::AutoLock lock(&_mutex);

	try {
		jio::File f(_filename, jio::JFF_WRITE_ONLY | jio::JFF_LARGEFILE | jio::JFF_TRUNCATE);

		for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
			std::ostringstream o;

			o << i->first << " {\n" << i->second << "\n}\n" << std::endl;

			f.Write(o.str().c_str(), (uint64_t)o.str().size());
		}

		f.Flush();
	} catch (...) {
		throw jcommon::RuntimeException("File not found");
	}
}

void Policies::AddPolice(std::string police)
{
	jthread::AutoLock lock(&_mutex);

	_polices[police];
}

std::vector<std::string> Policies::GetPolicies()
{
	jthread::AutoLock lock(&_mutex);

	std::vector<std::string> polices;

	for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
		polices.push_back(i->first);
	}

	return polices;
}

std::string Policies::GetPoliceByName(std::string police)
{
	jthread::AutoLock lock(&_mutex);

	std::map<std::string, std::string>::iterator i = _polices.find(police);
	
	if (i != _polices.end()) {
		return i->second;
	}

	throw jcommon::RuntimeException("Index out of bounds exception");
}

std::string Policies::GetPoliceByIndex(int index)
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

void Policies::RemovePoliceByName(std::string police)
{
	jthread::AutoLock lock(&_mutex);

	std::map<std::string, std::string>::iterator i = _polices.find(police);
	
	if (i == _polices.end()) {
		throw jcommon::RuntimeException("Cannot find police");
	}

	_polices.erase(i);
}

void Policies::RemovePoliceByIndex(int index)
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

void Policies::SetPoliceContent(std::string police, std::string value)
{
	jthread::AutoLock lock(&_mutex);

	std::map<std::string, std::string>::iterator i = _polices.find(police);
	
	if (i == _polices.end()) {
		throw jcommon::RuntimeException("Cannot find police");
	}
		
	_polices[police] = value;
}

}

