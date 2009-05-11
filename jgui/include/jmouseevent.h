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
#ifndef MOUSEEVENT_H
#define MOUSEEVENT_H

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

enum jmouse_event_t {
	JMOUSE_UNKNOWN_EVENT,
	JMOUSE_CLICKED_EVENT,
	JMOUSE_PRESSED_EVENT,
	JMOUSE_RELEASED_EVENT,
	JMOUSE_MOVED_EVENT,
	JMOUSE_WHEEL_EVENT
};

enum jmouse_button_t {
	JBUTTON_UNKNOWN = 0x0,
	JBUTTON_BUTTON1 = 0x1,
	JBUTTON_BUTTON2 = 0x2,
	JBUTTON_BUTTON3 = 0x4,
	JBUTTON_WHEEL = 0x08
};

class MouseEvent : public jcommon::EventObject{

	private:
		int _x,
			_y,
			_click_count;
		jmouse_event_t _type;
		jmouse_button_t _button;

	public:
		MouseEvent(void *source, jmouse_event_t type, jmouse_button_t button, int click_count, int x, int y);
		virtual ~MouseEvent();

		jmouse_event_t GetType();

		int GetClickCount();
		jmouse_button_t GetButton();

		int GetX();
		int GetY();

};

}

#endif

