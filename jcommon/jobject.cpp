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
#include "jcommon/jobject.h"
#include "jexception/jnullpointerexception.h"

#include <sstream>

namespace jcommon {

Object::Object()
{
	AddParent("jcommon::Object");
	
	_namespace = "null";
	_classname = "null";
}

Object::~Object()
{
}
	
void Object::AddParent(std::string parent)
{
	bool b = false;
	
	for (std::vector<std::string>::iterator i = _parents.begin(); i!=_parents.end(); i++) {
		if ((*i) == parent) {
			b = true;
		}
	}
	
	if (b == false) {
		_parents.push_back(parent);
	}
}

std::vector<std::string> & Object::GetParents()
{
	return _parents;
}

std::string Object::GetNameSpace()
{
	return _namespace;
}

std::string Object::GetClassName()
{
	return _classname;
}

std::string Object::GetFullClassName()
{
	if (_namespace != "") {
		return _namespace + "::" + _classname;
	}

	return _classname;
}

void Object::SetClassName(std::string name)
{
	std::string::size_type r = name.rfind("::");

	if (r != std::string::npos) {
		_namespace = name.substr(0, r);
		_classname = name.substr(r + 2, name.size());
	} else {
		_namespace = "";
		_classname = name;
	}

	AddParent(name);
}

void Object::SetNameSpace(std::string name)
{
	std::string::size_type r = name.rfind("::");

	if (r != std::string::npos) {
		_namespace = name.substr(0, r);
	} else {
		_namespace = name;
	}
}

bool Object::InstanceOf(std::string parent)
{
	for (std::vector<std::string>::iterator i = _parents.begin(); i!=_parents.end(); i++) {
		if ((*i) == parent) {
			return true;
		}
	}
	
	return false;
}

bool Object::InstanceOf(Object *o)
{
	if ((void *)o == NULL) {
    return false;
	}

	return InstanceOf(o->GetFullClassName());
}

bool Object::Equals(Object *o)
{
	if ((void *)o == NULL) {
		return false;
	}

	return Hash() == o->Hash();
}

void Object::Copy(Object *o)
{
	(*this) = (*o);
}

int Object::Compare(Object *o)
{
	return 0;
}

Object * Object::Clone()
{
	return new Object(*this);
}

uint64_t Object::Hash()
{
	return (uint64_t)((uintptr_t)this);
}

std::string Object::what()
{
	std::ostringstream o;

	if (_namespace != "") {
		o << _namespace << "::" << _classname << "@" << std::hex << (uintptr_t)this;
	} else {
		o << _classname << "@" << (uintptr_t)this;
	}
	
	return o.str();
}

}
