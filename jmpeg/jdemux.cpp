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

#include <algorithm>

namespace jmpeg {

Demux::Demux(jmpeg_data_type_t type):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jmpeg::Demux");

	_type = type;
	_last_crc = 0;
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

void Demux::Append(const char *data, int data_length)
{
	int sync_byte = TS_G8(data);

	if (sync_byte != TS_SYNC_BYTE) {
		return;
	}

	// int transport_error_indicator = TS_GM8(data+1, 0, 1);
	// int payload_unit_start_indicator = TS_GM8(data+1, 1, 1);
	// int transport_priority = TS_GM8(data+1, 2, 1);
	int pid = TS_GM16(data+1, 3, 13);
	// int scrambling_control = TS_GM8(data+3, 0, 2);
	// int adaptation_field_exist = TS_GM8(data+3, 2, 1);
	// int contains_payload = TS_GM8(data+3, 3, 1);
	// int continuity_counter = TS_GM8(data+3, 4, 4);

	if (_pid > 0 && _pid != pid) {
		return;
	}

	if (_type == JMDT_RAW) {
		DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_ARRIVED, data, data_length, _pid, -1));

		return;
	}

	int tid = TS_G8(data+4);

	if (_tid > 0 && _tid != tid) {
		return;
	}

	// TODO:: process section
	
	bool is_complete = false;
	uint32_t crc;

	if (is_complete) {
		_data = _buffer;
		_buffer.clear();

		if (_is_crc_enabled == false || _last_crc != crc) {
			DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_NOT_FOUND, data+TS_HEADER_LENGTH, data_length-TS_HEADER_LENGTH, _pid, _tid));
		}
	}
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

	std::vector<DemuxListener *>::iterator i = std::find(_demux_listeners.begin(), _demux_listeners.end(), listener);
	
	if (i != _demux_listeners.end()) {
		_demux_listeners.erase(i);
	}
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
