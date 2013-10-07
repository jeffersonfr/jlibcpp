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
#include "jresource.h"
#include "jillegalargumentexception.h"
#include "jtimeoutexception.h"
#include "jthread.h"

namespace jresource {

Resource::Resource()
{
	_listener = NULL;
	_is_available = false;
}

Resource::~Resource()
{
}

bool Resource::IsAvailable()
{
	return _is_available;
}

void Resource::Reserve(ResourceStatusListener *listener, bool force, int timeout)
{
	if (listener == NULL) {
		throw jcommon::IllegalArgumentException("Resource listener cannot be null");
	}

	if (_is_available == false) {
		if (force == true) {
			ResourceStatusEvent *event = new ResourceStatusEvent(this, JRS_RELEASE_FORCED);

			_listener->ReleaseForced(event);

			DispatchResourceStatusEvent(event);
		} else {
			ResourceStatusEvent *event = new ResourceStatusEvent(this, JRS_RELEASE_REQUESTED);

			bool request = _listener->ReleaseRequested(event);

			DispatchResourceStatusEvent(event);

			if (request == false) {
				if (timeout < 0) {
					ResourceStatusEvent *event;

					while (_listener->ReleaseRequested((event = new ResourceStatusEvent(this, JRS_RELEASE_REQUESTED))) != true) {
						DispatchResourceStatusEvent(event);

						jthread::Thread::MSleep(1000);
					}
				} else {
					ResourceStatusEvent *event = new ResourceStatusEvent(this, JRS_RELEASE_REQUESTED);

					jthread::Thread::MSleep(timeout);

					request = _listener->ReleaseRequested(event);

					DispatchResourceStatusEvent(event);

					if (request == false) {
						throw jthread::TimeoutException("Resource time is expired");
					}
				}
			}
		}

		ResourceStatusEvent *event = new ResourceStatusEvent(this, JRS_RELEASED);

		_listener->Released(event);
					
		DispatchResourceStatusEvent(event);
	}

	_listener = listener;
	_is_available = false;

	DispatchResourceTypeEvent(new ResourceTypeEvent(this, JRT_RESERVED));
}

void Resource::Release()
{
	if (_is_available == false) {
		ResourceTypeEvent *type_event = new ResourceTypeEvent(this, JRT_RELEASED);
		ResourceStatusEvent *status_event = new ResourceStatusEvent(this, JRS_RELEASED);

		_listener->Released(status_event);

		_is_available = true;

		DispatchResourceTypeEvent(type_event);
		DispatchResourceStatusEvent(status_event);
	}
}

void Resource::RegisterResourceTypeListener(ResourceTypeListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_type_listeners.begin(), _type_listeners.end(), listener) == _type_listeners.end()) {
		_type_listeners.push_back(listener);
	}
}

void Resource::RemoveResourceTypeListener(ResourceTypeListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ResourceTypeListener *>::iterator i = std::find(_type_listeners.begin(), _type_listeners.end(), listener);

	if (i != _type_listeners.end()) {
		_type_listeners.erase(i);
	}
}

void Resource::DispatchResourceTypeEvent(ResourceTypeEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_type_listeners.size();

	while (k++ < (int)_type_listeners.size() && event->IsConsumed() == false) {
		ResourceTypeListener *listener = _type_listeners[k-1];


		if (event->GetType() == JRT_RESERVED) {
			listener->Reserved(event);
		} else if (event->GetType() == JRT_RELEASED) {
			listener->Released(event);
		}

		if (size != (int)_type_listeners.size()) {
			size = (int)_type_listeners.size();

			k--;
		}
	}

	delete event;
}
void Resource::RegisterResourceStatusListener(ResourceStatusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_status_listeners.begin(), _status_listeners.end(), listener) == _status_listeners.end()) {
		_status_listeners.push_back(listener);
	}
}

void Resource::RemoveResourceStatusListener(ResourceStatusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ResourceStatusListener *>::iterator i = std::find(_status_listeners.begin(), _status_listeners.end(), listener);

	if (i != _status_listeners.end()) {
		_status_listeners.erase(i);
	}
}

void Resource::DispatchResourceStatusEvent(ResourceStatusEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_status_listeners.size();

	while (k++ < (int)_status_listeners.size() && event->IsConsumed() == false) {
		ResourceStatusListener *listener = _status_listeners[k-1];

		if (event->GetType() == JRS_RELEASED) {
			listener->Released(event);
		} else if (event->GetType() == JRS_RELEASE_FORCED) {
			listener->ReleaseForced(event);
		} else if (event->GetType() == JRS_RELEASE_REQUESTED) {
			listener->ReleaseRequested(event);
		}

		if (size != (int)_status_listeners.size()) {
			size = (int)_status_listeners.size();

			k--;
		}
	}

	delete event;
}

}
