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
#ifndef J_GROUP_H
#define J_GROUP_H

#include "jresourcestatuslistener.h"
#include "jresourcestatusevent.h"

#include <string>

namespace jresource {

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class ResourceServer{

	private:

		
	public:
		/**
		 * \brief
		 * 
		 */
		ResourceServer();
		
		/**
		 * \brief
		 * 
		 */
		virtual ~ResourceServer();

		/**
		 * \brief
		 * 
		 */
		void AddResourceStatusListener(ResourceStatusListener *listener);

		/**
		 * \brief
		 * 
		 */
		void RemoveResourceStatusListener(ResourceStatusListener *listener);
    
		/**
		 * \brief
		 * 
		 */
		void DispatchResourceStatusEvent(ResourceStatusEvent *event);
    
};

}

#endif
