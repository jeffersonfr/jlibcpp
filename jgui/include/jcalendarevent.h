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
#ifndef CALENDAREVENT_H
#define CALENDAREVENT_H

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
 * \author Jeff Ferr
 */
class CalendarEvent : public jcommon::EventObject{

	private:
		int _day,
			_month,
			_year;

	public:
		/**
		 * \brief
		 *
		 */
		CalendarEvent(void *source, int day, int month, int year);
		
		/**
		 * \brief
		 *
		 */
		virtual ~CalendarEvent();

		/**
		 * \brief
		 *
		 */
		int GetDay();
		
		/**
		 * \brief
		 *
		 */
		int GetMonth();
		
		/**
		 * \brief
		 *
		 */
		int GetYear();

};

}

#endif

