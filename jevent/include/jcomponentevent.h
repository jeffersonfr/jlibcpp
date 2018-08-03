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

#include "jevent/jeventobject.h"

namespace jevent {

/**
 * \brief
 *
 */
enum jcomponentevent_type_t {
	JCET_ONHIDE,
	JCET_ONSHOW,
	JCET_ONMOVE,
	JCET_ONSIZE,
	JCET_ONPAINT,
	JCET_ONENTER,
	JCET_ONLEAVE
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ComponentEvent : public jevent::EventObject {

	private:
		/** \brief */
		jcomponentevent_type_t _type;

	public:
		/**
		 * \brief
		 *
		 */
		ComponentEvent(void *source, jcomponentevent_type_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComponentEvent();

		/**
		 * \brief
		 *
		 */
		virtual jcomponentevent_type_t GetType();

};

}

#endif

