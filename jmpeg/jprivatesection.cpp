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
#include "jmpeg/jprivatesection.h"
#include "jexception/jinvalidargumentexception.h"

#include <algorithm>

namespace jmpeg {

PrivateSection::PrivateSection(std::shared_ptr<std::string> data):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jmpeg::PrivateSection");

  _data = data;
}

PrivateSection::~PrivateSection()
{
}

std::shared_ptr<std::string> PrivateSection::GetData()
{
  return _data;
}

int PrivateSection::GetTableIdentifier()
{
  return TS_G8(_data->c_str() + 0);
}

int PrivateSection::GetSectionSyntaxIndicator()
{
  return TS_GM8(_data->c_str() + 1, 0, 1);
}

int PrivateSection::GetSectionLength()
{
  return TS_GM16(_data->c_str() + 1, 4, 12);
}

int PrivateSection::GetTableIdentifierExtension()
{
  if (GetSectionSyntaxIndicator() == 0) {
    return -1;
  }

  return TS_G16(_data->c_str() + 3);
}

int PrivateSection::GetVersionNumber()
{
  if (GetSectionSyntaxIndicator() == 0) {
    return -1;
  }
  
  return TS_GM8(_data->c_str() + 5, 2, 5);
}

int PrivateSection::GetCurrentNextIndicator()
{
  if (GetSectionSyntaxIndicator() == 0) {
    return -1;
  }
  
  return TS_GM8(_data->c_str() + 5, 7, 1);
}

int PrivateSection::GetSectionNumber()
{
  if (GetSectionSyntaxIndicator() == 0) {
    return -1;
  }
  
  return TS_G8(_data->c_str() + 6);
}

int PrivateSection::GetLastSectionNumber()
{
  if (GetSectionSyntaxIndicator() == 0) {
    return -1;
  }
  
  return TS_G8(_data->c_str() + 7);
}

std::shared_ptr<std::string> PrivateSection::GetPrivateData()
{
  if (GetSectionSyntaxIndicator() == 0) {
    return std::make_shared<std::string>(_data->c_str() + 3, GetSectionLength());
  }

  return std::make_shared<std::string>(_data->c_str() + 8, GetSectionLength() - 9);
}

}
