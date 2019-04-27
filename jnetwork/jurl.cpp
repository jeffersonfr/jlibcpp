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
#include "jnetwork/jurl.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jstringutils.h"
#include "jexception/jruntimeexception.h"

#include <sstream>
#include <bitset>

namespace jnetwork {

URL::URL(std::string url_):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jcommon::URL");

  _url = jcommon::StringUtils::Trim(url_);

  std::string::size_type proto_index = _url.find("://");
  std::string::size_type file_proto = _url.find(":/");
  std::string::size_type slash_index = _url.find("/");

  if (proto_index != std::string::npos && proto_index == (slash_index-1)) {
    _protocol = _url.substr(0, proto_index);
    _path = _url.substr(proto_index+3, _url.size());
  } else {
    if (file_proto != std::string::npos && file_proto == (slash_index-1)) {
      _protocol = _url.substr(0, file_proto);
      _path = _url.substr(file_proto+1, _url.size());
    } else {
      _protocol = "file";
      _path = _url;
    }
  }

  _protocol = jcommon::StringUtils::ToLower(_protocol);

  slash_index = _path.find("/");

  if (slash_index != std::string::npos) {
    _host = _path.substr(0, slash_index);
    _query = _path.substr(slash_index);
  } else {
    _host = _path;
    _query = "/";
  }

  std::string::size_type port_index = _host.find(":");

  if (port_index != std::string::npos) {
    _host = _host.substr(0, port_index);

    if (slash_index != std::string::npos && port_index < slash_index) {
      _port = atoi(_path.substr(port_index+1, slash_index).c_str());
    } else {
      _port = atoi(_path.substr(port_index+1).c_str());
    }
  } else {
    _port = -1;

    if (_protocol == "http") {
      _port = 80;
    } else if (_protocol == "https") {
      _port = 443;
    } else if (_protocol == "ftp") {
      _port = 21;
    } else if (_protocol == "pop") {
      _port = 110;
    }
  }

  std::string::size_type file_slash = _path.rfind("/");

  if (file_slash != std::string::npos) {
    _file = _path.substr(file_slash+1);
  } else {
    _file = _path;
  }
  
  file_proto = _query.find(":/");

  std::string::size_type param_index = _query.find("?");

  if (param_index != std::string::npos) {
    if (file_proto == std::string::npos || (file_proto != std::string::npos && file_proto > param_index)) {
      _params = _query.substr(param_index+1);
    }
  }

  std::string::size_type ref_index = _file.find("#");

  if (ref_index != std::string::npos) {
    _reference = _file.substr(ref_index+1);
    _file = _file.substr(0, ref_index);
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
    throw jexception::RuntimeException("URL too large");
  }

  std::ostringstream o;

  for (i=0; i<(int)s_.length(); i++) {
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
    throw jexception::RuntimeException("URL too large");
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

              o << (char)strtol(t, nullptr, 16);
              
              i += 3;

              if (i < numChars) {
                c = s_[i];
              }
            }

            if ((i < numChars) && (c == '%')) {
              throw jexception::RuntimeException("URL decoder imcomplete trailing escape '%' pattern");
            }
          } catch (...) {
              throw jexception::RuntimeException("URL decoder illegal hex characters in escape '%' pattern");
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

std::string URL::What()
{
  std::ostringstream o;

  o << _protocol << "://" << _path << std::flush;

  return o.str();
}

}
