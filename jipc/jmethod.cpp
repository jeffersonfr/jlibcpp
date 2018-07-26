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
#include "jipc/jmethod.h"
#include "jipc/jipchelper.h"
#include "jcommon/jstringtokenizer.h"
#include "jmath/jbase64.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jipcexception.h"

#include <sstream>

namespace jipc {

Method::Method(std::string name)
{
	_name = name;
}

Method::~Method()
{
}

void Method::Initialize(uint8_t *buffer, int size)
{
	if (size <= 0) {
		return;
	}

	std::string str = (const char *)buffer;

	// INFO:: <method>:[<id=value>[;<id=param>]*
	jcommon::StringTokenizer tokens(str, ";", jcommon::JTT_STRING);

	if (tokens.GetSize() > 0) {
		_name = tokens.GetToken(0);
	}

	jmath::Base64 base64;

	for (int i=1; i<tokens.GetSize(); i++) {
		jcommon::StringTokenizer param(tokens.GetToken(i), ":", jcommon::JTT_STRING);

		if (param.GetSize() > 1) {
			char *value = base64.Decode((uint8_t *)param.GetToken(1).c_str(), param.GetToken(1).size());

			if (value == NULL) {
				value = (char *)"";
			}

			SetTextParam(param.GetToken(0), value);
		}
	}
}

std::string Method::GetName()
{
	return _name;
}

void Method::SetName(std::string name)
{
	_name = name;
}

std::string Method::Encode()
{
	jmath::Base64 base64;
	std::ostringstream o;

	o << _name << ";";

	for (std::map<std::string, std::string>::iterator i=GetParameters().begin(); i!=GetParameters().end(); i++) {
		char *value = base64.Encode((uint8_t *)i->second.c_str(), i->second.size());

		if (value == NULL) {
			value = (char *)"";
		}

		o << i->first << ":" << value << ";";
	}

	return o.str();
}

std::string Method::what()
{
	std::ostringstream o;

	o << _name << ";";

	for (std::map<std::string, std::string>::iterator i=GetParameters().begin(); i!=GetParameters().end(); i++) {
		o << i->first << ":" << i->second << ";";
	}

	return o.str();
}

}
