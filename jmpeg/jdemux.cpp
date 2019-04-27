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
#include "jmpeg/jdemux.h"
#include "jmpeg/jdemuxmanager.h"
#include "jmpeg/jmpeglib.h"
#include "jmath/jcrc.h"

#include <algorithm>

namespace jmpeg {

Demux::Demux(jdemux_type_t type):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jmpeg::Demux");

  _type = type;
  _timeout = std::chrono::milliseconds(2000);

  UpdateTimePoint();
}
    
Demux::~Demux()
{
}

void Demux::SetID(std::string id)
{
  _id = id;
}

void Demux::Start()
{
  DemuxManager::GetInstance()->AddDemux(this);
}

void Demux::Stop()
{
  DemuxManager::GetInstance()->RemoveDemux(this);
}

std::string Demux::GetID()
{
  return _id;
}

jdemux_type_t Demux::GetType()
{
  return _type;
}

void Demux::SetTimeout(std::chrono::milliseconds ms)
{
  _timeout = ms;
}

std::chrono::milliseconds Demux::GetTimeout()
{
  return _timeout;
}

std::chrono::steady_clock::time_point Demux::GetTimePoint()
{
  return _timepoint;
}

void Demux::UpdateTimePoint()
{
  _timepoint = std::chrono::steady_clock::now();
}

void Demux::SetPID(int pid)
{
  _pid = pid;
}

int Demux::GetPID()
{
  return _pid;
}

bool Demux::Append(const char *data, int data_length)
{
  return false;
}

void Demux::RegisterDemuxListener(jevent::DemuxListener *listener)
{
  if (listener == nullptr) {
    return;
  }

  if (std::find(_demux_listeners.begin(), _demux_listeners.end(), listener) == _demux_listeners.end()) {
    _demux_listeners.push_back(listener);
  }
}

void Demux::RemoveDemuxListener(jevent::DemuxListener *listener)
{
  if (listener == nullptr) {
    return;
  }

  _demux_listeners.erase(std::remove(_demux_listeners.begin(), _demux_listeners.end(), listener), _demux_listeners.end());
}

void Demux::DispatchDemuxEvent(jevent::DemuxEvent *event)
{
  if (event == nullptr) {
    return;
  }

  int k = 0,
      size = (int)_demux_listeners.size();

  while (k++ < (int)_demux_listeners.size() && event->IsConsumed() == false) {
    jevent::DemuxListener *listener = _demux_listeners[k-1];

    if (event->GetType() == jevent::JDET_DATA_ARRIVED) {
      listener->DataArrived(event);
    } else if (event->GetType() == jevent::JDET_DATA_NOT_FOUND) {
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
