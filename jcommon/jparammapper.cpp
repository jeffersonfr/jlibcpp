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
#include "jcommon/jparammapper.h"
#include "jcommon/jstringtokenizer.h"
#include "jexception/joutofboundsexception.h"

#include <sstream>
#include <iomanip>

#include <stdlib.h>
#include <string.h>

namespace jcommon {

ParamMapper::ParamMapper()
{
}

ParamMapper::~ParamMapper()
{
}

const std::map<std::string, std::string> & ParamMapper::GetParameters()
{
	return _params;
}

std::string ParamMapper::GetTextParam(std::string key, std::string value)
{
	if (_params.find(key) != _params.end()) {
		return _params[key];
	}

	return value;
}

bool ParamMapper::GetBooleanParam(std::string key, bool value)
{
	if (_params.find(key) != _params.end()) {
		std::string str = _params[key];

		if (str == "1" || str == "on" || str == "yes" || str == "true") {
			return true;
		} else {
			return false;
		}
	}

	return value;
}

int64_t ParamMapper::GetIntegerParam(std::string key, int64_t value)
{
	if (_params.find(key) != _params.end()) {
		std::string str = _params[key];

		if (str.size() == 0 || str.size() > 10) {
			return -1;
		}

		int radix = 10;
		char *tmp = (char *)str.c_str();
		char *err;

		if (tmp[0] == '0') {
			if (strnlen(tmp, 2) > 1 && (tmp[1] == 'x' || tmp[1] == 'X')) {
				radix = 16;
				tmp = tmp+2;
			} else {
				radix = 8;
				tmp = tmp+1;
			}
		}

		return strtoll(tmp, &err, radix);
	}

	return value;
}

double ParamMapper::GetDecimalParam(std::string key, double value)
{
	if (_params.find(key) != _params.end()) {
		return atof(_params[key].c_str());
	}

	return value;
}

std::string ParamMapper::GetArrayParam(std::string key, std::string value)
{
	if (_params.find(key) != _params.end()) {
		return _params[key];
	}

	return value;
}

std::string ParamMapper::GetParamByIndex(int index)
{
	if (index < 0 || index >= (int)_params.size()) {
		throw jexception::OutOfBoundsException("Index out of index");
	}

	std::map<std::string, std::string>::iterator i=_params.begin();

	for (int j=0; j<=index; j++) {
		i++;
	}

	return i->second;
}

void ParamMapper::SetTextParam(std::string key, std::string value)
{
	_params[key] = value;
}

void ParamMapper::SetBooleanParam(std::string key, bool value)
{
	char *str = (char *)"false";

	if (value == true) {
		str = (char *)"true";
	}

	_params[key] = str;
}

void ParamMapper::SetIntegerParam(std::string key, int64_t value)
{
	std::ostringstream o;

	o << value;

	_params[key] = o.str();
}

void ParamMapper::SetDecimalParam(std::string key, double value)
{
	std::ostringstream o;

	o << std::setprecision(2) << value;
	
	_params[key] = o.str();
}

void ParamMapper::SetArrayParam(std::string key, uint8_t *value, uint64_t value_size)
{
	if (value == nullptr || value_size == 0) {
		return;
	}

	_params[key] = std::string((const char *)value, value_size);
}

void ParamMapper::SetParamByIndex(int index, std::string value)
{
	if (_params.size() < 1) {
		throw jexception::OutOfBoundsException("List of parameters is empty");
	}

	if (index < 0 || index >= (int)_params.size()) {
		throw jexception::OutOfBoundsException("Index out of index");
	}

	std::map<std::string, std::string>::iterator i=_params.begin();

	for (int j=0; j<=index; j++) {
		i++;
	}

	i->second = value;
}

void ParamMapper::RemoveParamByID(std::string id)
{
	std::map<std::string, std::string>::iterator i=_params.find(id);

	if (i != _params.end()) {
		_params.erase(i);
	}
}

void ParamMapper::RemoveParamByIndex(int index)
{
	if (_params.size() < 1) {
		throw jexception::OutOfBoundsException("List of parameters is empty");
	}

	if (index < 0 || index >= (int)_params.size()) {
		throw jexception::OutOfBoundsException("Index out of index");
	}

	std::map<std::string, std::string>::iterator i=_params.begin();

	for (int j=0; j<=index; j++) {
		i++;
	}

	if (i != _params.end()) {
		_params.erase(i);
	}
}

Object * ParamMapper::Clone()
{
	std::map<std::string, std::string> params = GetParameters();

	ParamMapper *mapper = new ParamMapper();

	for (std::map<std::string, std::string>::iterator i=params.begin(); i!=params.end(); i++) {
		mapper->SetTextParam(i->first, i->second);
	}

	return mapper;
}

bool ParamMapper::Equals(Object *o)
{
	ParamMapper *mapper = dynamic_cast<ParamMapper *>(o);

	if (mapper == nullptr) {
		return false;
	}

	std::map<std::string, std::string> params = mapper->GetParameters();
	std::map<std::string, std::string> local_params = GetParameters();


	for (std::map<std::string, std::string>::iterator i=params.begin(); i!=params.end(); i++) {
		if (i->second != GetTextParam(i->first)) {
			return false;
		}
	}

	return true;
}

void ParamMapper::Copy(Object *o)
{
	ParamMapper *mapper = dynamic_cast<ParamMapper *>(o);

	if (mapper == nullptr) {
		return;
	}

	std::map<std::string, std::string> params = mapper->GetParameters();

	for (std::map<std::string, std::string>::iterator i=params.begin(); i!=params.end(); i++) {
		SetTextParam(i->first, i->second);
	}
}

}
