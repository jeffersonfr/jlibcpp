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
#ifndef J_MOUSEEVENT_H
#define J_MOUSEEVENT_H

#include "jeventobject.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jmouse_event_t {
	JME_UNKNOWN,
	JME_PRESSED,
	JME_RELEASED,
	JME_MOVED,
	JME_WHEEL
};

/**
 * \brief
 *
 */
enum jmouse_button_t {
	JMB_UNKNOWN = 0x0,
	JMB_BUTTON1 = 0x1,
	JMB_BUTTON2 = 0x2,
	JMB_BUTTON3 = 0x4,
	JMB_WHEEL		= 0x08
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class MouseEvent : public jcommon::EventObject{

	private:
		int _x,
				_y,
				_click_count;
		jmouse_event_t _type;
		jmouse_button_t _button;

	public:
		/**
		 * \brief
		 *
		 */
		MouseEvent(void *source, jmouse_event_t type, jmouse_button_t button, int click_count, int x, int y);
		
		/**
		 * \brief
		 *
		 */
		virtual ~MouseEvent();

		/**
		 * \brief
		 *
		 */
		virtual jmouse_event_t GetType();

		/**
		 * \brief
		 *
		 */
		virtual int GetClickCount();
		
		/**
		 * \brief
		 *
		 */
		virtual jmouse_button_t GetButton();

		/**
		 * \brief
		 *
		 */
		virtual int GetX();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetY();

		/**
		 * \brief
		 *
		 */
		virtual void SetX(int x);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetY(int y);

};

}

#endif

