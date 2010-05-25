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
#include "jcommonlib.h"

namespace jcommon {

URL::URL(std::string url_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::URL");

	_url = url_;

	jcommon::StringTokenizer proto(_url, "://", SPLIT_FLAG, false);

	if (proto.GetSize() == 1) {
		jcommon::StringTokenizer proto(_url, ":/", SPLIT_FLAG, false);

		if (proto.GetSize() == 1) {
			_protocol = "file";
			_path = _url;
		} else {
			_protocol = proto.GetToken(0);

			_path = "/";

			for (int i=1; i<proto.GetSize(); i++) {
				_path += proto.GetToken(1);

				if (i < proto.GetSize()-1) {
					_path = _path + "://";
				}
			}
		}
	} else if (proto.GetSize() > 1) {
		_protocol = proto.GetToken(0);

		for (int i=1; i<proto.GetSize(); i++) {
			_path += proto.GetToken(1);

			if (i < proto.GetSize()-1) {
				_path = _path + "://";
			}
		}

		uint32_t index = _path.rfind("/");

		if (index != std::string::npos) {
			_file = _path.substr(index+1);
		}
	}

	jcommon::StringTokenizer host(proto.GetToken(proto.GetSize() - 1), "/", SPLIT_FLAG, false);
	jcommon::StringTokenizer port(host.GetToken(0), ":", SPLIT_FLAG, false);
	
	_port = -1;

	if (port.GetSize() == 1) {
		if (_protocol == "http" || _protocol == "https") {
			_port = 80;
		} else if (_protocol == "ftp") {
			_port = 21;
		}
	} else if (port.GetSize() == 2) {
		_port = atoi(port.GetToken(1).c_str());
	}

	if (_port < 0) {
		/*
		_host = "";
		_query = "";
		_file = "";
		_params = "";
		_reference = "";
		*/

		return;
	}

	_host = port.GetToken(0);

	if (host.GetSize() == 1) {
		_file = _host;
		_query = "/";

	} else {
		for (int i=1; i<host.GetSize(); i++) {
			_query = _query + "/" + host.GetToken(i);
		}

		uint32_t index = _query.rfind("/");

		if (index != std::string::npos) {
			_file = _query.substr(index+1);
		} else {
			_file = "";
		}
		
		jcommon::StringTokenizer params(_file, "?", SPLIT_FLAG, false);

		if (params.GetSize() == 2) {
			_file = params.GetToken(0);
			_params = params.GetToken(1);
		}

		jcommon::StringTokenizer reference(_file, "#", SPLIT_FLAG, false);

		if (reference.GetSize() == 2) {
			_file = reference.GetToken(0);
			_reference = reference.GetToken(1);
		}
	}
}

URL::~URL()
{
}

std::string URL::Encode(std::string s_)
{
	try {
		return Encode(s_, "ISO-8859-1");
	} catch (...) {
	}

	return s_;
}

std::string URL::Decode(std::string s_)
{
	try {
		return Decode(s_, "ISO-8859-1");
	} catch (...) {
	}

	return s_;
}

std::string URL::Encode(std::string s_, std::string standard_)
{
	std::bitset<256> dontNeedEncoding;
	
	int i;
	
	for (i = 'a'; i <= 'z'; i++) {
	    dontNeedEncoding.set(i);
	}
	
	for (i = 'A'; i <= 'Z'; i++) {
	    dontNeedEncoding.set(i);
	}
	
	for (i = '0'; i <= '9'; i++) {
	    dontNeedEncoding.set(i);
	}
	
	dontNeedEncoding.set('&');
	dontNeedEncoding.set('=');
	dontNeedEncoding.set('-');
	dontNeedEncoding.set('_');
	dontNeedEncoding.set('.');
	dontNeedEncoding.set('*');
	dontNeedEncoding.set(':');
	dontNeedEncoding.set('/');
	dontNeedEncoding.set('?');
	
	bool needToChange = false;
	
	if ((int)s_.length() > 512) {
		throw RuntimeException("URL too large");
	}

	std::ostringstream o;

	for (int i=0; i<(int)s_.length(); i++) {
		int c = (int)s_[i];

		if (dontNeedEncoding[c] == true) {
			o << (char)c;
		} else {
			needToChange = true;

			o << "%" << std::hex << (int)c;
		}
	}

	return ((needToChange == true)?o.str():s_);
}

std::string URL::Decode(std::string s_, std::string standard_)
{
	int i = 0,
		numChars = s_.length();
	bool needToChange = false;

	if (numChars > 512) {
		throw RuntimeException("URL too large");
	}

	std::ostringstream o;
	
	if (standard_ == "") {
		standard_ = "iso-8859-1";
	}
	
	char c;

	while (i < numChars) {
		c = s_[i];

		switch (c) {
			case '+':
					needToChange = true;
					o << " ";
					i++;
					break;
			case '%':
					try {
						while (((i+2) < numChars) && (c == '%')) {
							char t[2+1] = {s_[i+1], s_[i+2], '\0'};

							o << (char)strtol(t, NULL, 16);
							
							i += 3;

							if (i < numChars) {
								c = s_[i];
							}
						}

						if ((i < numChars) && (c == '%')) {
							throw RuntimeException("URL decoder imcomplete trailing escape '%' pattern");
						}
					} catch (...) {
							throw RuntimeException("URL decoder illegal hex characters in escape '%' pattern");
					}

					needToChange = true;

					break;
				default:
					o << (char)c;
					i++;
					break;
		}
	}

	return ((needToChange == true)?o.str():s_);
}

std::string URL::GetHost()
{
	return _host;
}

int URL::GetPort()
{
	return _port;
}

std::string URL::GetProtocol()
{
	return _protocol;
}

std::string URL::GetParameters()
{
	return _params;
}

std::string URL::GetReference()
{
	return _reference;
}

std::string URL::GetFile()
{
	return _file;
}

std::string URL::GetQuery()
{
	return _query;
}

std::string URL::GetPath()
{
	return _path;
}

void URL::SetHost(std::string s)
{
	_host = s;
}

void URL::SetPort(int p)
{
	_port = p;
}

void URL::SetProtocol(std::string s)
{
	_protocol = s;
}

void URL::SetReference(std::string s)
{
	_reference = s;
}

void URL::SetParameters(std::string s)
{
	_params = s;
}

void URL::SetFile(std::string s)
{
	_file = s;
}

void URL::SetQuery(std::string s)
{
	_query = s;
}

void URL::SetPath(std::string s)
{
	_path = s;
}

std::string URL::what()
{
	std::ostringstream o;

	o << _protocol << "://" << _path << std::flush;

	return o.str();
}

}
