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
#include "jresource/jresource.h"
#include "jexception/jillegalargumentexception.h"
#include "jexception/jtimeoutexception.h"

#include <algorithm>
#include <chrono>
#include <thread>

namespace jresource {

Resource::Resource()
{
	_listener = NULL;
	_is_available = true;
}

Resource::~Resource()
{
}

bool Resource::IsAvailable()
{
	// jthread::AutoLock lock(&_resource_mutex);

	return _is_available;
}

void Resource::Reserve(jevent::ResourceStatusListener *listener, bool force, int64_t timeout)
{
	if (listener == NULL) {
		throw jexception::IllegalArgumentException("Resource listener cannot be null");
	}

	// jthread::AutoLock lock(&_resource_mutex);

	if (_is_available == false) {
		if (force == true) {
      jevent::ResourceStatusEvent *event = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASE_FORCED);

			_listener->ReleaseForced(event);

			DispatchResourceStatusEvent(event);
		} else {
      jevent::ResourceStatusEvent *event = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASE_REQUESTED);

			bool request = _listener->ReleaseRequested(event);

			DispatchResourceStatusEvent(event);

			if (request == false) {
				if (timeout <= 0LL) {
          jevent::ResourceStatusEvent *e;

					while (_listener->ReleaseRequested((e = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASE_REQUESTED))) != true) {
						DispatchResourceStatusEvent(e);

						// _sem.Wait(&_resource_mutex);

						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					}
				} else {
          jevent::ResourceStatusEvent *e;
					int64_t fixed = 100;
					int64_t count = fixed;

					while ((request = _listener->ReleaseRequested((e = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASE_REQUESTED)))) != true) {
						DispatchResourceStatusEvent(e);

						// _resource_mutex.Unlock();

						std::this_thread::sleep_for(std::chrono::milliseconds(fixed));

						// _resource_mutex.Lock();

						count = count + fixed;

						if (count > timeout) {
							request = _listener->ReleaseRequested((e = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASE_REQUESTED)));

							delete e;

							break;
						}
					}
					
					if (request == false) {
						throw jexception::TimeoutException("Resource time is expired");
					}
				}
			}
		}

    jevent::ResourceStatusEvent *e = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASED);

		_listener->Released(e);
					
		DispatchResourceStatusEvent(e);
	}

	_listener = listener;
	_is_available = false;

	DispatchResourceTypeEvent(new jevent::ResourceTypeEvent(this, jevent::JRT_RESERVED));
}

void Resource::Release()
{
	// jthread::AutoLock lock(&_resource_mutex);

	if (IsAvailable() == false) {
    jevent::ResourceTypeEvent *type_event = new jevent::ResourceTypeEvent(this, jevent::JRT_RELEASED);
    jevent::ResourceStatusEvent *status_event = new jevent::ResourceStatusEvent(this, jevent::JRS_RELEASED);

		_is_available = true;

		_listener->Released(status_event);

		DispatchResourceTypeEvent(type_event);
		DispatchResourceStatusEvent(status_event);
	}
		
	_condition.notify_all();
}

void Resource::RegisterResourceTypeListener(jevent::ResourceTypeListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_type_listeners.begin(), _type_listeners.end(), listener) == _type_listeners.end()) {
		_type_listeners.push_back(listener);
	}
}

void Resource::RemoveResourceTypeListener(jevent::ResourceTypeListener *listener)
{
	if (listener == NULL) {
		return;
	}

  _type_listeners.erase(std::remove(_type_listeners.begin(), _type_listeners.end(), listener), _type_listeners.end());
}

void Resource::DispatchResourceTypeEvent(jevent::ResourceTypeEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_type_listeners.size();

	while (k++ < (int)_type_listeners.size() && event->IsConsumed() == false) {
		jevent::ResourceTypeListener *listener = _type_listeners[k-1];


		if (event->GetType() == jevent::JRT_RESERVED) {
			listener->Reserved(event);
		} else if (event->GetType() == jevent::JRT_RELEASED) {
			listener->Released(event);
		}

		if (size != (int)_type_listeners.size()) {
			size = (int)_type_listeners.size();

			k--;
		}
	}

	delete event;
}
void Resource::RegisterResourceStatusListener(jevent::ResourceStatusListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_status_listeners.begin(), _status_listeners.end(), listener) == _status_listeners.end()) {
		_status_listeners.push_back(listener);
	}
}

void Resource::RemoveResourceStatusListener(jevent::ResourceStatusListener *listener)
{
	if (listener == NULL) {
		return;
	}

  _status_listeners.erase(std::remove(_status_listeners.begin(), _status_listeners.end(), listener), _status_listeners.end());
}

void Resource::DispatchResourceStatusEvent(jevent::ResourceStatusEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_status_listeners.size();

	while (k++ < (int)_status_listeners.size() && event->IsConsumed() == false) {
		jevent::ResourceStatusListener *listener = _status_listeners[k-1];

		if (event->GetType() == jevent::JRS_RELEASED) {
			listener->Released(event);
		} else if (event->GetType() == jevent::JRS_RELEASE_FORCED) {
			listener->ReleaseForced(event);
		} else if (event->GetType() == jevent::JRS_RELEASE_REQUESTED) {
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
