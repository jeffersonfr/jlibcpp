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
#include "jmethod.h"
#include "jipchelper.h"
#include "jstringtokenizer.h"
#include "joutofboundsexception.h"

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
	jcommon::StringTokenizer tokens(IPCHelper::Decode(str), ":", jcommon::JTT_STRING);

	if (tokens.GetSize() == 1) {
		_name = str;
	} else {
		jcommon::StringTokenizer params(tokens.GetToken(1), ";", jcommon::JTT_STRING);

		_name = tokens.GetToken(0);

		for (int i=0; i<params.GetSize(); i++) {
			jcommon::StringTokenizer param(params.GetToken(i), "=", jcommon::JTT_STRING);

			if (param.GetSize() > 1) {
				SetTextParam(param.GetToken(0), param.GetToken(1));
			}
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

std::string Method::what()
{
	std::ostringstream o;

	o << _name << ":";

	for (std::map<std::string, std::string>::iterator i=GetParameters().begin(); i!=GetParameters().end(); i++) {
		o << i->first << "=" << i->second << ";";
	}

	return o.str();
}

}
