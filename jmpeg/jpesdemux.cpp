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
#include "jmpeg/jpesdemux.h"
#include "jmpeg/jdemuxmanager.h"
#include "jmpeg/jmpeglib.h"
#include "jmath/jcrc.h"

#include <algorithm>

namespace jmpeg {

PESDemux::PESDemux():
	jmpeg::Demux(JDT_PES)
{
	jcommon::Object::SetClassName("jmpeg::PESDemux");
}
		
PESDemux::~PESDemux()
{
}

bool PESDemux::Append(const char *data, int data_length)
{
  if (
      TS_G8(data+0) != 0x00 ||
      TS_G8(data+1) != 0x00 ||
      TS_G8(data+2) != 0x01) {
    return false;
  }

  return true;
}

}
