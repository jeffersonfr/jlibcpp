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
#include "jcommon/jpolicies.h"
#include "jcommon/jstringutils.h"
#include "jexception/jioexception.h"

#include <fstream>

namespace jcommon {

Policies::Policies(std::string filename)
{
  _filename = filename;
}

Policies::~Policies()
{
}

void Policies::Load()
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::ifstream file(_filename.c_str(), std::ofstream::in | std::ofstream::app);

  if (!file) {
    throw jexception::IOException(jcommon::StringUtils::Format("Unable to load polices at '%s'", _filename.c_str()));
  }

  std::string key;
  std::string value;
  int64_t state = 0;
  char c;

  while (file.get(c)) {
    if (state == -1) {
      // remove comment from file
      if (c == '\n') {
        state = 0;
      }
    } else if (state == 0) {
      if (isalnum(c) != 0) {
        key = key + c;
        state = 1;
      } else if (c == '#') {
        state = -1;
      }
    } else if (state == 1) {
      if (c != '{' && c != '\n') {
        key = key + c;
      } else {
        state = 2;
      }
    } else if (state == 2) {
      if (c != '}' ) {
        value = value + c;
      } else {
        _polices[jcommon::StringUtils::Trim(key)] = jcommon::StringUtils::Trim(value);

        state = 0;
        key = "";
        value = "";
      }
    }
  }

  file.close();
}

void Policies::Save()
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::ofstream file(_filename.c_str(), std::ofstream::out | std::ofstream::app);

  if (!file) {
    throw jexception::IOException(jcommon::StringUtils::Format("Unable to save polices at '%s'", _filename.c_str()));
  }

  for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
    file << i->first << " {\n" << i->second << "\n}\n" << std::endl;
  }

  file.flush();
  file.close();
}

void Policies::AddPolice(std::string police)
{
  std::lock_guard<std::mutex> guard(_mutex);

  _polices[police];
}

std::vector<std::string> Policies::GetPolicies()
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::vector<std::string> polices;

  for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++) {
    polices.push_back(i->first);
  }

  return polices;
}

std::string Policies::GetPoliceByName(std::string police)
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::map<std::string, std::string>::iterator i = _polices.find(police);
  
  if (i != _polices.end()) {
    return i->second;
  }

  return "";
}

std::string Policies::GetPoliceByIndex(int index)
{
  std::lock_guard<std::mutex> guard(_mutex);

  if (index >= 0 && index < (int)_polices.size()) {
    int k = 0;

    for (std::map<std::string, std::string>::iterator i=_polices.begin(); i!=_polices.end(); i++, k++) {
      if (k == index) {
        return i->second;
      }
    }
  }

  return "";
}

void Policies::RemovePoliceByName(std::string police)
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::map<std::string, std::string>::iterator i = _polices.find(police);
  
  if (i != _polices.end()) {
    _polices.erase(i);
  }
}

void Policies::RemovePoliceByIndex(int index)
{
  std::lock_guard<std::mutex> guard(_mutex);

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
  std::lock_guard<std::mutex> guard(_mutex);

  std::map<std::string, std::string>::iterator i = _polices.find(police);
  
  if (i != _polices.end()) {
    _polices[police] = value;
  }
}

}

