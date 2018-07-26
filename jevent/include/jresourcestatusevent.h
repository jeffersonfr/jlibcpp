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
#ifndef J_RESOURCESTATUSEVENT_H
#define J_RESOURCESTATUSEVENT_H

#include "jevent/jeventobject.h"

namespace jevent {

enum jresourceevent_status_t {
	JRS_RELEASED,
	JRS_RELEASE_FORCED,
	JRS_RELEASE_REQUESTED
};

/**
 * \brief
 * 
 * \author Jeff Ferr
 */
class ResourceStatusEvent : public jevent::EventObject {

	private:
		/** \brief */
		jresourceevent_status_t _type;
		
	public:
		/**
		 * \brief
		 * 
		 */
		ResourceStatusEvent(void *source, jresourceevent_status_t type);
		
		/**
		 * \brief
		 * 
		 */
		virtual ~ResourceStatusEvent();

		/**
		 * \brief
		 * 
		 */
		virtual jresourceevent_status_t  GetType();
    
};

}

#endif
