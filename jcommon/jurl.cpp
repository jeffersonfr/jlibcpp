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
#include "jurl.h"
#include "jruntimeexception.h"
#include "jstringtokenizer.h"

#include <sstream>

namespace jcommon {

URL::URL(std::string url_):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jcommon::URL");

	_url = url_;

	jcommon::StringTokenizer proto(_url, "://", SPLIT_FLAG, false);
	int index;

	if (proto.GetSize() == 1) {
		if (_url[0] == '.' || _url[0] == '/') {
			_protocol = "file";
		} else {
		 	_protocol = "http";
		}
	} else if (proto.GetSize() == 2) {
		_protocol = proto.GetToken(0);
	} else {
		throw jcommon::RuntimeException("URL malformed exception");
	}
	
	jcommon::StringTokenizer host(proto.GetToken(proto.GetSize() - 1), "/", SPLIT_FLAG, false);
	jcommon::StringTokenizer port(host.GetToken(0), ":", SPLIT_FLAG, false);
	
	if (port.GetSize() == 1) {
		if (_protocol == "http" || _protocol == "https") {
			_port = 80;
		} else if (_protocol == "ftp") {
			_port = 21;
		} else {
			// file, playlist, plist
			_port = 0;
		}
	} else if (port.GetSize() == 2) {
		_port = atoi(port.GetToken(1).c_str());
	} else {
		throw jcommon::RuntimeException("URL malformed exception");
	}

	_host = port.GetToken(0);

	if (host.GetSize() == 1) {
		_params = "";
		_resource = "/";
		_file = "";
	
		return;
	}
	
	index = proto.GetToken(proto.GetSize() - 1).find("/");
	
	if (_protocol == "file" || _protocol == "plist" || _protocol == "playlist") {
		_resource = _host;
	}

	_resource = _resource + proto.GetToken(proto.GetSize() - 1).substr(index);
	
	if (_protocol == "file" || _protocol == "plist" || _protocol == "playlist") {
		uint32_t index = _resource.rfind("/");

		if (index != std::string::npos) {
			_file = _resource.substr(index+1);
			_resource = _resource.substr(0, index);
		} else {
			_file = _resource;
			_resource = "";
		}
	}
}

URL::~URL()
{
}

std::string URL::Encode(std::string s_)
{
	std::string str;

	try {
	    str = Encode(s_, "ISO-8859-1");
	} catch (...) {
		return s_;
	}

	return str;
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
	
	dontNeedEncoding.set(' ');
	dontNeedEncoding.set('-');
	dontNeedEncoding.set('_');
	dontNeedEncoding.set('.');
	dontNeedEncoding.set('*');
	
	bool needToChange = false;
	bool wroteUnencodedChar = false;
	int maxBytesPerChar = 10,
		length = (int)s_.length();
	char *out = NULL;
	char *bytes = NULL;
	
	if (length > 500) {
		throw RuntimeException("URL too large");
	}
	
	out = new char[length * 3];
	bytes = new char[maxBytesPerChar];

	out[0] = '\0';
	bytes[0] = '\0';
	
	for (int i=0; i<length; i++) {
		int c = (int)s_[i];

		if (dontNeedEncoding[c] == true) {
			if (c == ' ') {
				c = '+';
				needToChange = true;
			}

			char t[] = {c, '\0'};
			
			strcat(out, t);
			wroteUnencodedChar = true;
		} else {
			try {
				if (wroteUnencodedChar == true) {
					if ((void *)bytes != NULL) {
						delete [] bytes;
					}
					
					bytes = new char[maxBytesPerChar];

					wroteUnencodedChar = false;
				}

				char t[] = {c, '\0'};

				strcat(bytes, t);
			} catch (...) {
				bytes[0] = '\0';

				continue;
			}

			int ba_length = strlen(bytes);
			char t[4],
				 p[] = {'%', '\0'},
				 *ba = bytes;
			
			for (int j=0; j<ba_length; j++) {
				strcat(out, p);
				sprintf(t, "%x", ba[j] & 0xff);
				strcat(out, t);
			}

			bytes[0] = '\0';

			needToChange = true;
		}
	}

	return ((needToChange == true)?std::string(out):s_);
}

std::string URL::Decode(std::string s_, std::string standard_)
{
	bool needToChange = false;
	int numChars = s_.length();
	int i = 0;
	char *sb;

	if (numChars > 500) {
		throw RuntimeException("URL too large");
	}
	
	sb = new char[s_.length()+1];
	sb[0] = '\0';
	
	if (standard_ == "") {
		standard_ = "iso-8859-1";
	}
	
	char c,
		 *bytes = NULL;

	while (i < numChars) {
		c = s_[i];

		switch (c) {
			case '+':
					strcat(sb, " ");
					i++;
					needToChange = true;
					break;
			case '%':
					try {
						if (bytes == NULL) {
							bytes = new char[(numChars-i)/3];
						}

						int pos = 0;

						while (((i+2) < numChars) && (c == '%')) {
							char t[2+1] = {s_[i+1], s_[i+2], '\0'};
							int r;

							r = bytes[pos++] = (char)strtol(t, NULL, 16);
							
							i += 3;

							if (i < numChars) {
								c = s_[i];
							}
						}

						bytes[pos] = '\0';

						if ((i < numChars) && (c == '%')) {
							throw RuntimeException("URL decoder imcomplete trailing escape '%' pattern");
						}

						strcat(sb, bytes);
					} catch (...) {
							throw RuntimeException("URL decoder illegal hex characters in escape '%' pattern");
					}

					needToChange = true;
					break;
				default:
					char t[2];

					t[0] = (char)c;
					t[1] = '\0';
		
					strcat(sb, t);
					i++;

					break;
		}
	}

	return ((needToChange == true)?sb:s_);
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

std::string URL::GetFile()
{
	return _file;
}

std::string URL::GetResource()
{
	return _resource;
}

std::string URL::GetPath()
{
	return	_resource + "/" + _file;
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

void URL::SetParameters(std::string s)
{
	_params = s;
}

void URL::SetFile(std::string s)
{
	_file = s;
}

void URL::SetResource(std::string s)
{
	_resource = s;
}

std::string URL::what()
{
	std::ostringstream o;

	if (_protocol == "file" || _protocol == "plist" || _protocol == "playlist") {
		o << _protocol << "://" << _resource << "/" << _file << std::flush;
	} else {
		o << _protocol << "://" << _host << ":" << _port << _resource << std::flush;
	}

	return o.str();
}

}
