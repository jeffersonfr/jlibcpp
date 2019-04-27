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
#include "jmpeg/jprogramserviceinformation.h"
#include "jexception/jinvalidargumentexception.h"

#include <algorithm>

namespace jmpeg {

ProgramServiceInformation::ProgramServiceInformation(std::shared_ptr<std::string> data):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jmpeg::ProgramServiceInformation");

  _data = data;
}

ProgramServiceInformation::~ProgramServiceInformation()
{
}

std::shared_ptr<std::string> ProgramServiceInformation::GetData()
{
  return _data;
}

int ProgramServiceInformation::GetTableIdentifier()
{
  return TS_G8(_data->c_str() + 0);
}

int ProgramServiceInformation::GetSectionSyntaxIndicator()
{
  return TS_GM8(_data->c_str() + 1, 0, 1);
}

int ProgramServiceInformation::GetSectionLength()
{
  return TS_GM16(_data->c_str() + 1, 4, 12);
}

std::shared_ptr<std::string> ProgramServiceInformation::GetPayload()
{
  return std::make_shared<std::string>(_data->c_str() + 3, GetSectionLength() - 4); // CRC32
}

}
