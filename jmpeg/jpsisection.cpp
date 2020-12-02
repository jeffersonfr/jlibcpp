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
#include "jmpeg/jpsisection.h"
#include "jexception/jinvalidargumentexception.h"

#include <algorithm>

namespace jmpeg {

PSISection::PSISection(std::shared_ptr<std::string> data):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jmpeg::PSISection");

  _data = data;
}

PSISection::~PSISection()
{
}

std::shared_ptr<std::string> PSISection::GetData()
{
  return _data;
}

int PSISection::GetTableIdentifier()
{
  return TS_G8(_data->c_str() + 0);
}

int PSISection::GetSectionSyntaxIndicator()
{
  return TS_GM8(_data->c_str() + 1, 0, 1);
}

int PSISection::GetSectionLength()
{
  return TS_GM16(_data->c_str() + 1, 4, 12);
}

std::shared_ptr<std::string> PSISection::GetPayload()
{
  return std::make_shared<std::string>(_data->c_str() + 3, GetSectionLength() - 4); // CRC32
}

}
