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
#include "jproperties.h"
#include "jstringutils.h"
#include "jautolock.h"
#include "jruntimeexception.h"
#include "jillegalargumentexception.h"
#include "jfile.h"

#include <iostream>
#include <fstream>

namespace jcommon {

Properties::Properties():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::Properties");
}

Properties::~Properties()
{
}

void Properties::Load(std::string filename_, std::string escape_)
{
	jthread::AutoLock lock(&_mutex);

	_filename = filename_;

	std::ifstream reader(_filename.c_str());
	std::string line;

	while (std::getline(reader, line)) {
		if (line.find("#") == 0) {
				struct jproperty_t prop;
	
				prop.value = line;
				prop.comment = true;
					
				_properties.push_back(prop);
		} else {
			std::string::size_type r1 = line.find(escape_);
			std::string::size_type r2 = line.find("#");

			if (line.size() > 0 && r1 != std::string::npos && (r2 == std::string::npos || (r2 != std::string::npos && r1 < r2))) {
				struct jproperty_t prop;

				prop.key = jcommon::StringUtils::Trim(line.substr(0, r1));
				prop.value = jcommon::StringUtils::Trim(line.substr(r1+1));
				prop.comment = false;

				_properties.push_back(prop);
			} else {
				struct jproperty_t prop;
	
				prop.value = line;
				prop.comment = true;
					
				_properties.push_back(prop);
			}
		}
	}

	reader.close();
}

void Properties::Save(std::string escape_)
{
	jthread::AutoLock lock(&_mutex);

	try {
		jio::File f(_filename, jio::JFF_WRITE_ONLY | jio::JFF_LARGEFILE | jio::JFF_TRUNCATE);
	
		for (std::vector<struct jproperty_t>::iterator i=_properties.begin(); i != _properties.end(); i++) {
			std::ostringstream o;
			
			jproperty_t p = *i;
			
			if (p.comment == false) {
				o << p.key << " " << escape_ << " " << p.value << std::endl;
			} else {
				o << p.value << std::endl;
			}

			f.Write(o.str().c_str(), (long)o.str().size());
		}

		f.Flush();
		f.Close();
	} catch (...) {
		throw RuntimeException("File not found");
	}
}

void Properties::SetPropertyByName(std::string key, std::string value)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct jproperty_t>::iterator i=_properties.begin(); i != _properties.end(); i++) {
		if ((*i).comment == false && (*i).key == key) {
			(*i).value = value;

			return;
		}
	}

	struct jproperty_t p;

	p.key = key;
	p.value = value;
	p.comment = false;

	_properties.push_back(p);
}

void Properties::SetPropertyByIndex(int index, std::string value)
{
	if ((int)_properties.size() == 0 || index < 0 || index > (int)_properties.size()-1) {
		throw IllegalArgumentException("Index out of bounds");
	}

	int k = 0;

	for (std::vector<struct jproperty_t>::iterator i=_properties.begin(); i != _properties.end(); i++, k++) {
		if (k == index) {
			jproperty_t p = *i;

			p.value = value;

			break;
		}
	}
}

std::string Properties::GetPropertyByName(std::string key, std::string reserv)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct jproperty_t>::iterator i=_properties.begin(); i != _properties.end(); i++) {
		jproperty_t p = *i;
		
		if (p.comment == false && p.key == key) {
			return p.value;
		}
	}
	
	return reserv;
}

std::string Properties::GetPropertyByIndex(int index, std::string reserv)
{
	jthread::AutoLock lock(&_mutex);

	if ((int)_properties.size() == 0 || index < 0 || index > (int)_properties.size()-1) {
		return reserv;
	}

	return _properties[index].value;
}

void Properties::RemovePropertyByName(std::string key)
{
	jthread::AutoLock lock(&_mutex);

	for (std::vector<struct jproperty_t>::iterator i=_properties.begin(); i != _properties.end(); i++) {
		if ((*i).key == key) {
			_properties.erase(i);

			break;
		}
	}
}

void Properties::RemovePropertyByIndex(int index)
{
	jthread::AutoLock lock(&_mutex);

	if ((int)_properties.size() == 0 || index < 0 || index > (int)_properties.size()-1) {
		throw IllegalArgumentException("Index out of bounds");
	}

	_properties.erase(_properties.begin()+index);
}

std::vector<std::string> Properties::GetProperties()
{
	jthread::AutoLock lock(&_mutex);

	std::vector<std::string> properties;

	for (std::vector<struct jproperty_t>::iterator i=_properties.begin(); i != _properties.end(); i++) {
		if ((*i).comment == false) {
			properties.push_back((*i).key);	
		}
	}
	
	return properties;
}

}

