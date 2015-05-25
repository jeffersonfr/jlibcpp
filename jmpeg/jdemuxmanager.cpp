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
#include "jdemuxmanager.h"
#include "jdemux.h"
#include "jmpeglib.h"
#include "jautolock.h"
#include "jdate.h"

#include <algorithm>

namespace jmpeg {

DemuxManager * DemuxManager::_instance = NULL;

DemuxManager::DemuxManager():
	jthread::Thread(),
	_demux_mutex(jthread::JMT_RECURSIVE)
{
	jcommon::Object::SetClassName("jmpeg::DemuxManager");

	_source = NULL;
	_is_running = false;
}
		
DemuxManager::~DemuxManager()
{
}

DemuxManager * DemuxManager::GetInstance()
{
	if (_instance == NULL) {
		_instance = new DemuxManager();
	}

	return _instance;
}

void DemuxManager::AddDemux(Demux *demux)
{
	if (demux == NULL) {
		return;
	}

	_demux_mutex.Lock();

	if (std::find(_sync_demuxes.begin(), _sync_demuxes.end(), demux) == _sync_demuxes.end()) {
		_sync_demuxes.push_back(demux);
	}
	
	_demux_mutex.Unlock();
}

void DemuxManager::RemoveDemux(Demux *demux)
{
	if (demux == NULL) {
		return;
	}

	_demux_mutex.Lock();

	std::vector<Demux *>::iterator i = std::find(_sync_demuxes.begin(), _sync_demuxes.end(), demux);
	
	if (i != _sync_demuxes.end()) {
		_sync_demuxes.erase(i);
	}
	
	std::map<Demux *, struct jdemux_status_t>::iterator j=_demux_status.find(demux);

	if (j != _demux_status.end()) {
		_demux_status.erase(j);
	}

	_demux_mutex.Unlock();
}

void DemuxManager::SetInputStream(jio::InputStream *is)
{
	_demux_mutex.Lock();

	_source = is;

	_demux_mutex.Unlock();
}

void DemuxManager::Start()
{
	if (IsRunning()) {
		return;
	}

	_is_running = true;
	
	Thread::Start();
}

void DemuxManager::Stop()
{
	if (IsRunning()) {
		_is_running = false;

		WaitThread();
	}
}

void DemuxManager::WaitSync()
{
	jthread::AutoLock lock(&_demux_sync_mutex);
}

void DemuxManager::Run()
{
	if (_source == NULL) {
		return;
	}

	std::map<int, std::string> timeline;

	while (_is_running) {
		jthread::AutoLock lock(&_demux_sync_mutex);

		char packet[TS_PACKET_LENGTH];
		int length = TS_PACKET_LENGTH;

		if (_source->Read(packet, length) != length) {
			break;
		}

		// INFO:: processing transport stream
		const char *data = packet;

		int sync_byte = TS_G8(data);

		if (sync_byte != TS_SYNC_BYTE) {
			continue;
		}

		int transport_error_indicator = TS_GM8(data+1, 0, 1);
		int payload_unit_start_indicator = TS_GM8(data+1, 1, 1);
		// int transport_priority = TS_GM8(data+1, 2, 1);
		int pid = TS_GM16(data+1, 3, 13);
		// int scrambling_control = TS_GM8(data+3, 0, 2);
		int adaptation_field_exist = TS_GM8(data+3, 2, 1);
		int contains_payload = TS_GM8(data+3, 3, 1);
		// int continuity_counter = TS_GM8(data+3, 4, 4);

		if (transport_error_indicator == 1) {
			continue;
		}

		if (contains_payload == 0) {
			continue;
		}

		const char *ptr = data+TS_HEADER_LENGTH;
		const char *end = data+TS_PACKET_LENGTH;

		// INFO:: discards adaptation field
		if (adaptation_field_exist == 1) {
			int adaptation_field_length = TS_G8(ptr);

			ptr = ptr + adaptation_field_length + 1;
		}

		std::string current;
		std::string previous;
		int section_length = -1;

		if (payload_unit_start_indicator == 1) {
			int pointer_field = TS_G8(ptr);

			if (pointer_field > 0) {
				previous = timeline[pid];

				if (previous.size() > 0) {
					previous.append(ptr+1, pointer_field);
				}
			}

			ptr = ptr + pointer_field + 1;

			if (ptr < end) {
				section_length = TS_PSI_G_SECTION_LENGTH(ptr) + 3;

				if (section_length > 3) {
					int length = end - ptr;
					int chunk = section_length;

					if (chunk > length) {
						chunk = length;
					}

					current = std::string(ptr, chunk);
				}
			}
		} else {
			current = timeline[pid];

			if (current.size() == 0) {
				continue;
			}

			section_length = TS_PSI_G_SECTION_LENGTH(current.data()) + 3;

			int length = end - ptr;

			if (length < 0) {
				continue;
			}

			int chunk = section_length - current.size();

			if (chunk > length) {
				chunk = length;
			}

			current.append(ptr, chunk);
		}

		timeline[pid] = current;

		for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
			Demux *demux = (*i);

			std::map<Demux *, struct jdemux_status_t>::iterator j=_demux_status.find(demux);
			uint64_t current_time = jcommon::Date::CurrentTimeMillis();
			struct jdemux_status_t t;

			if (j == _demux_status.end()) {
				t.start_time = current_time;
				t.found = false;

				_demux_status[demux] = t;
			}

			if (demux->GetPID() < 0 || demux->GetPID() == pid) {
				if (demux->GetType() == JMDT_RAW) {
					if (demux->Append(packet, TS_PACKET_LENGTH) == true) {
						_demux_status[demux].found = true;

						demux->DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_ARRIVED, packet, TS_PACKET_LENGTH, pid, -1));
					}
				} else if (demux->GetType() == JMDT_PSI) {
					if (section_length == (int)current.size()) {
						if (demux->Append(current.c_str(), current.size()) == true) {
							int tid = TS_G8(current.data());

							_demux_status[demux].found = true;

							demux->DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_ARRIVED, current.data(), current.size(), pid, tid));
						}
					}
					
					// INFO:: previous section
					if (previous.size() > 0) {
						int section_length = TS_PSI_G_SECTION_LENGTH(previous.data()) + 3;

						if (section_length == (int)previous.size()) {
							if (demux->Append(previous.c_str(), previous.size()) == true) {
								int tid = TS_G8(previous.data());

								_demux_status[demux].found = true;

								demux->DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_ARRIVED, previous.data(), previous.size(), pid, tid));
							}
						}
					}
				}
			}

			t = _demux_status[demux];

			if (t.found == false && demux->GetTimeout() < (int)(current_time-t.start_time)) {
				_demux_status[demux].start_time = current_time;

				demux->DispatchDemuxEvent(new DemuxEvent(demux, JDET_DATA_NOT_FOUND, NULL, 0, demux->GetPID(), demux->GetTID()));
			}
		}

		_demux_mutex.Lock();

		_demuxes = _sync_demuxes;
	
		_demux_mutex.Unlock();
	}

	_is_running = false;
}

}
