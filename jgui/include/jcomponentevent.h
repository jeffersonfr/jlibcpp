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
#ifndef J_COMPONENTEVENT_H
#define J_COMPONENTEVENT_H

#include "jeventobject.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jcomponent_event_t {
	COMPONENT_HIDDEN_EVENT,
	COMPONENT_SHOWN_EVENT,
	COMPONENT_MOVED_EVENT,
	COMPONENT_PAINTED_EVENT
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ComponentEvent : public jcommon::EventObject{

	private:
		jcomponent_event_t _type;

	public:
		/**
		 * \brief
		 *
		 */
		ComponentEvent(void *source, jcomponent_event_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComponentEvent();

		/**
		 * \brief
		 *
		 */
		jcomponent_event_t GetType();

};

}

#endif

