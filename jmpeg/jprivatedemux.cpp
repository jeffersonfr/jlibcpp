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
#include "jmpeg/jprivatedemux.h"
#include "jmpeg/jdemuxmanager.h"
#include "jmpeg/jmpeglib.h"
#include "jmath/jcrc.h"

#include <algorithm>

namespace jmpeg {

PrivateDemux::PrivateDemux():
	jmpeg::Demux(JDT_PRIVATE)
{
	jcommon::Object::SetClassName("jmpeg::PrivateDemux");

  _tid = -1;
	_is_crc_enabled = true;
}
		
PrivateDemux::~PrivateDemux()
{
}

void PrivateDemux::SetTID(int tid)
{
	_tid = tid;
}

int PrivateDemux::GetTID()
{
	return _tid;
}

void PrivateDemux::SetCRCCheckEnabled(bool b)
{
	_is_crc_enabled = b;
}

bool PrivateDemux::IsCRCCheckEnabled()
{
	return _is_crc_enabled;
}

jdemux_status_t PrivateDemux::Append(const char *data, int data_length)
{
	int table_id = TS_G8(data);

	if (_tid >= 0 && _tid != table_id) {
		return JDS_FAILED;
	}

  int section_length = TS_PSI_G_SECTION_LENGTH(data) + 3;

  if (section_length > data_length) {
    return JDS_INCOMPLETE;
  }

	int section_syntax_indicator = TS_GM8(data+1, 1, 1);

  if (section_syntax_indicator != 0) {
    if (_is_crc_enabled == true) {
      uint32_t 
        // crc = *(uint32_t *)(data+(data_length-4)),
        sum = jmath::CRC::Calculate32((const uint8_t *)data, data_length);

      if (sum != 0xffffffff) {
        _last_index = -1;

        return JDS_FAILED;
      }
    }
  }

	return JDS_COMPLETE;
}

}
