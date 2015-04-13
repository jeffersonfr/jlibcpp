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

#include <algorithm>

namespace jmpeg {

DemuxManager * DemuxManager::_instance = NULL;

DemuxManager::DemuxManager():
	jthread::Thread()
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

	if (std::find(_demuxes.begin(), _demuxes.end(), demux) == _demuxes.end()) {
		_demuxes.push_back(demux);
	}
	
	_demux_mutex.Unlock();
}

void DemuxManager::RemoveDemux(Demux *demux)
{
	if (demux == NULL) {
		return;
	}

	_demux_mutex.Lock();

	std::vector<Demux *>::iterator i = std::find(_demuxes.begin(), _demuxes.end(), demux);
	
	if (i != _demuxes.end()) {
		_demuxes.erase(i);
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

void DemuxManager::Run()
{
	if (_source == NULL) {
		return;
	}

	while (_is_running) {
		_demux_mutex.Lock();

		char packet[188];
		int length = 188;

		if (_source->Read(packet, length) == length) {
			for (std::vector<Demux *>::iterator i=_demuxes.begin(); i!=_demuxes.end(); i++) {
				Demux *demux = (*i);

				demux->Append(packet, 188);
			}
		}

		_demux_mutex.Unlock();
	}
}

}
