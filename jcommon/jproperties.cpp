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
#include "jcommon/jproperties.h"
#include "jcommon/jstringutils.h"
#include "jexception/jioexception.h"

#include <iostream>
#include <fstream>

namespace jcommon {

Properties::Properties(std::string filename):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jcommon::Properties");

  _filename = filename;
}

Properties::~Properties()
{
}

void Properties::Load()
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::ifstream file(_filename.c_str(), std::ofstream::in);

  if (!file) {
    throw jexception::IOException(jcommon::StringUtils::Format("Unable to load properties at '%s'", _filename.c_str()));
  }

  std::string line;

  while (std::getline(file, line)) {
    line = jcommon::StringUtils::Trim(line);

    if (line.find("#") == 0) {
      // INFO:: comments
    } else {
      std::string::size_type r1 = line.find("=");
      std::string::size_type r2 = line.find("#");

      if (line.size() > 0 && r1 != std::string::npos && (r2 == std::string::npos || (r2 != std::string::npos && r1 < r2))) {
        std::string key = jcommon::StringUtils::Trim(line.substr(0, r1));
        std::string value = jcommon::StringUtils::Trim(line.substr(r1 + 1));

        SetTextParam(key, value);
      } else {
        // INFO:: comments
      }
    }
  }

  file.close();
}

void Properties::Save()
{
  std::lock_guard<std::mutex> guard(_mutex);

  std::ofstream file(_filename.c_str(), std::ofstream::out);

  if (!file) {
    throw jexception::IOException(jcommon::StringUtils::Format("Unable to save properties at '%s'", _filename.c_str()));
  }

  std::map<std::string, std::string> params = GetParameters();

  for (std::map<std::string, std::string>::iterator i=params.begin(); i!=params.end(); i++) {
    file << i->first << " = " << i->second << std::endl;
  }

  file.flush();
  file.close();
}

}

