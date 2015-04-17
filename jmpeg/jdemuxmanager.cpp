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

	while (_is_running) {
		jthread::AutoLock lock(&_demux_sync_mutex);

		char packet[188];
		int length = 188;

		if (_source->Read(packet, length) == length) {
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

				bool complete = demux->Append(packet, 188);

				if (complete) {
					_demux_status[demux].found = true;
				}

				t = _demux_status[demux];

				if (t.found == false && demux->GetTimeout() < (int)(current_time-t.start_time)) {
					_demux_status[demux].start_time = current_time;

					demux->DispatchDemuxEvent(new DemuxEvent(demux, JDET_DATA_NOT_FOUND, NULL, 0, demux->GetPID(), demux->GetTID()));
				}
			}
		}

		_demux_mutex.Lock();

		_demuxes = _sync_demuxes;
	
		_demux_mutex.Unlock();
	}
}

}
