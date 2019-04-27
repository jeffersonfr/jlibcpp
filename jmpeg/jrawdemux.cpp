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
#include "jmpeg/jrawdemux.h"

namespace jmpeg {

RawDemux::RawDemux():
  Demux(JDT_RAW)
{
  jcommon::Object::SetClassName("jmpeg::RawDemux");

  _packet_size = 188; // INFO:: DVB standard
}
    
RawDemux::~RawDemux()
{
}

void RawDemux::SetPacketSize(int packet_size)
{
  _packet_size = packet_size;
}

int RawDemux::GetPacketSize()
{
  return _packet_size;
}

bool RawDemux::Append(const char *data, int data_length)
{
  if (_packet_size != data_length) {
    return false;
  }

  return true;
}

}
