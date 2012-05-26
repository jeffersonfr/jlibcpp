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
#include "Stdafx.h"
#include "jresourceserver.h"

namespace jresource {

ResourceServer::ResourceServer()
{
}

ResourceServer::~ResourceServer()
{
}

void ResourceServer::RegisterResourceStatusListener(ResourceStatusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_status_listeners.begin(), _status_listeners.end(), listener) == _status_listeners.end()) {
		_status_listeners.push_back(listener);
	}
}

void ResourceServer::RemoveResourceStatusListener(ResourceStatusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ResourceStatusListener *>::iterator i = std::find(_status_listeners.begin(), _status_listeners.end(), listener);
	
	if (i != _status_listeners.end()) {
		_status_listeners.erase(i);
	}
}

void ResourceServer::DispatchResourceStatusEvent(ResourceStatusEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_status_listeners.size();

	while (k++ < (int)_status_listeners.size() && event->IsConsumed() == false) {
		ResourceStatusListener *listener = _status_listeners[k-1];

		listener->StatusChanged(event);

		if (size != (int)_status_listeners.size()) {
			size = (int)_status_listeners.size();

			k--;
		}
	}

	delete event;
}

}

