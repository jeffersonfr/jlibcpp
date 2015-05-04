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
	std::vector<int> last_pids;

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

		// int transport_error_indicator = TS_GM8(data+1, 0, 1);
		int payload_unit_start_indicator = TS_GM8(data+1, 1, 1);
		// int transport_priority = TS_GM8(data+1, 2, 1);
		int pid = TS_GM16(data+1, 3, 13);
		// int scrambling_control = TS_GM8(data+3, 0, 2);
		int adaptation_field_exist = TS_GM8(data+3, 2, 1);
		int contains_payload = TS_GM8(data+3, 3, 1);
		// int continuity_counter = TS_GM8(data+3, 4, 4);

		if (contains_payload == 0) {
			continue;
		}

		const char *ptr = data+TS_HEADER_LENGTH;
		int offset = TS_HEADER_LENGTH;

		// INFO:: discards adaptation field
		if (adaptation_field_exist == 1) {
			int adaptation_field_length = TS_G8(data+4);

			ptr = ptr + adaptation_field_length;
			offset = offset + adaptation_field_length;
		}

		std::string current;
		std::string previous;
		int previous_pid = -1;
		int tid = -1;
		int section_length = -1;

		if (payload_unit_start_indicator == 1) {
			int pointer_field = TS_G8(ptr);

			if (pointer_field > 0) {
				if (last_pids.size() > 0) {
					previous = timeline[last_pids.back()];

					last_pids.pop_back();

					if (previous.size() > 0) {
						previous.append(ptr+1, pointer_field);

						previous_pid = pid;
					}
				}
			}

			ptr = ptr + pointer_field + 1;
			offset = offset + pointer_field + 1;
			tid = TS_G8(ptr);
			section_length = TS_PSI_G_SECTION_LENGTH(ptr) + 3;

			int length = TS_PACKET_LENGTH - offset;
			int chunk_length = section_length;

			if (section_length > length) {
				chunk_length = length;
			}

			if (offset >= 184 || section_length == 3) {
				continue;
			}

			current = std::string(ptr, chunk_length);
		} else {
			current = timeline[pid];

			if (current.size() == 0) {
				continue;
			}

			tid = TS_G8(current.data());
			section_length = TS_PSI_G_SECTION_LENGTH(current.data()) + 3;

			int length = TS_PACKET_LENGTH - offset;
			int chunk_length = section_length - current.size();

			if (chunk_length > length) {
				chunk_length = length;
			}

			current.append(ptr, chunk_length);
		}

		if (section_length == (int)current.size()) {
			timeline[pid].clear();
			
			std::vector<int>::iterator i = std::find(last_pids.begin(), last_pids.end(), pid);
			
			if (i != last_pids.end()) {
				last_pids.erase(i);
			}
		} else {
			timeline[pid] = current;
		
			if (std::find(last_pids.begin(), last_pids.end(), pid) == last_pids.end()) {
				last_pids.push_back(pid);
			}
		}

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
							_demux_status[demux].found = true;

							demux->DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_ARRIVED, current.data(), current.size(), pid, tid));
						}
					}
				}
			}

			if (demux->GetPID() < 0 || demux->GetPID() == previous_pid) {
				if (demux->GetType() == JMDT_PSI) {
					int section_length = TS_PSI_G_SECTION_LENGTH(previous.data()) + 3;

					if (section_length == (int)previous.size()) {
						if (demux->Append(previous.c_str(), previous.size()) == true) {
							_demux_status[demux].found = true;

							demux->DispatchDemuxEvent(new DemuxEvent(this, JDET_DATA_ARRIVED, previous.data(), previous.size(), pid, tid));
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
