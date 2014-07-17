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
#ifndef J_RESOURCE_H
#define J_RESOURCE_H

#include "jresourcestatuslistener.h"
#include "jresourcetypelistener.h"

#include "jautolock.h"
#include "jcondition.h"

#include <string>

namespace jresource {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class Resource{

	private:
		/** \brief */
		std::vector<ResourceTypeListener *> _type_listeners;
		/** \brief */
		std::vector<ResourceStatusListener *> _status_listeners;
		/** \brief */
		ResourceStatusListener *_listener;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		jthread::Condition _sem;
		/** \brief */
		bool _is_available;
		
	public:
		/**
		 * \brief
		 * 
		 */
		Resource();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~Resource();

		/**
		 * \brief
		 * 
		 */
		virtual bool IsAvailable();

		/**
		 * \brief
		 * 
		 */
		virtual void Reserve(ResourceStatusListener *listener, bool force = false, int64_t timeout = -1LL);

		/**
		 * \brief
		 * 
		 */
		virtual void Release();
    
		/**
		 * \brief
		 * 
		 */
		virtual void RegisterResourceTypeListener(ResourceTypeListener *listener);

		/**
		 * \brief
		 * 
		 */
		virtual void RemoveResourceTypeListener(ResourceTypeListener *listener);

		/**
		 * \brief
		 * 
		 */
		virtual void DispatchResourceTypeEvent(ResourceTypeEvent *event);

		/**
		 * \brief
		 * 
		 */
		virtual void RegisterResourceStatusListener(ResourceStatusListener *listener);

		/**
		 * \brief
		 * 
		 */
		virtual void RemoveResourceStatusListener(ResourceStatusListener *listener);

		/**
		 * \brief
		 * 
		 */
		virtual void DispatchResourceStatusEvent(ResourceStatusEvent *event);

};

}

#endif

