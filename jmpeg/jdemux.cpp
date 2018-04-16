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
#include "jdemux.h"
#include "jdemuxmanager.h"
#include "jmpeglib.h"
#include "jcrc.h"

#include <algorithm>

namespace jmpeg {

Demux::Demux(jmpeg_data_type_t type):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmpeg::Demux");

	_type = type;
	_last_index = -1;
	_timeout = -1;
	_is_crc_enabled = true;
	_is_update_if_modified = true;
}
		
Demux::~Demux()
{
}

void Demux::Start()
{
	DemuxManager::GetInstance()->AddDemux(this);
}

void Demux::Stop()
{
	DemuxManager::GetInstance()->RemoveDemux(this);
}

void Demux::SetType(jmpeg_data_type_t type)
{
	_type = type;
}

jmpeg_data_type_t Demux::GetType()
{
	return _type;
}

void Demux::SetTimeout(int timeout)
{
	_timeout = timeout;
}

int Demux::GetTimeout()
{
	return _timeout;
}

void Demux::SetPID(int pid)
{
	_pid = pid;
}

void Demux::SetTID(int tid)
{
	_tid = tid;
}

int Demux::GetPID()
{
	return _pid;
}

int Demux::GetTID()
{
	return _tid;
}

void Demux::SetCRCCheckEnabled(bool b)
{
	_is_crc_enabled = b;
}

bool Demux::IsCRCCheckEnabled()
{
	return _is_crc_enabled;
}

void Demux::SetUpdateIfModified(bool b)
{
	_is_update_if_modified = b;
}

bool Demux::IsUpdateIfModified()
{
	return _is_update_if_modified;
}

bool Demux::Append(const char *data, int data_length)
{
	if (_type == JMDT_RAW) {
		return true;
	}

	int tid = TS_G8(data);

	if (_tid >= 0 && _tid != tid) {
		return false;
	}

	uint32_t crc = *(uint32_t *)(data+(data_length-4));
	
	if (_is_crc_enabled == true) {
		uint32_t sum = jmath::CRC::Calculate32((const uint8_t *)_buffer.data(), _buffer.size()-4);

		if (sum != 0) {
			_buffer.clear();

			_last_index = -1;

			return false;
		}
	}

	if (_is_update_if_modified == false || _last_crcs.find(crc) == _last_crcs.end()) {
		_last_crcs.insert(crc);

		return true;
	}

	return false;
}

void Demux::RegisterDemuxListener(DemuxListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_demux_listeners.begin(), _demux_listeners.end(), listener) == _demux_listeners.end()) {
		_demux_listeners.push_back(listener);
	}
}

void Demux::RemoveDemuxListener(DemuxListener *listener)
{
	if (listener == NULL) {
		return;
	}

  _demux_listeners.erase(std::remove(_demux_listeners.begin(), _demux_listeners.end(), listener), _demux_listeners.end());
}

void Demux::DispatchDemuxEvent(DemuxEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_demux_listeners.size();

	while (k++ < (int)_demux_listeners.size() && event->IsConsumed() == false) {
		DemuxListener *listener = _demux_listeners[k-1];

		if (event->GetType() == JDET_DATA_ARRIVED) {
			listener->DataArrived(event);
		} else if (event->GetType() == JDET_DATA_NOT_FOUND) {
			listener->DataNotFound(event);
		}

		if (size != (int)_demux_listeners.size()) {
			size = (int)_demux_listeners.size();

			k--;
		}
	}

	delete event;
}

}
