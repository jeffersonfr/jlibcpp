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
#ifndef J_SELECTEVENT_H
#define J_SELECTEVENT_H

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

class Item;

/**
 * \brief
 *
 */
enum jselect_type_t {
	LEFT_ITEM,
	RIGHT_ITEM,
	UP_ITEM,
	DOWN_ITEM,
	ACTION_ITEM
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class SelectEvent : public jcommon::EventObject{

	private:
		jgui::Item *_item;
		jselect_type_t _type;
		int _index;

	public:
		/**
		 * \brief
		 *
		 */
		SelectEvent(void *source, jgui::Item *item, int index, jselect_type_t type);
		
		/**
		 * \brief
		 *
		 */
		virtual ~SelectEvent();

		/**
		 * \brief
		 *
		 */
		jgui::Item * GetItem();
		
		/**
		 * \brief
		 *
		 */
		int GetIndex();
		
		/**
		 * \brief
		 *
		 */
		jselect_type_t GetType();

};

}

#endif

