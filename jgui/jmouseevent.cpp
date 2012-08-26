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
#include "Stdafx.h"
#include "jmouseevent.h"

namespace jgui {

MouseEvent::MouseEvent(void *source, jmouseevent_type_t type, jmouseevent_button_t button, jmouseevent_button_t buttons, int click_count, int x, int y):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jgui::MouseEvent");

	_x = x;
	_y = y;
	_click_count = click_count;
	_button = button;
	_buttons = buttons;
	_type = type;
}

MouseEvent::~MouseEvent()
{
}

jmouseevent_type_t MouseEvent::GetType()
{
	return _type;
}

int MouseEvent::GetClickCount()
{
	return _click_count;
}

jmouseevent_button_t MouseEvent::GetButton()
{
	return _button;
}

jmouseevent_button_t MouseEvent::GetButtons()
{
	return _buttons;
}

int MouseEvent::GetX()
{
	return _x;
}

int MouseEvent::GetY()
{
	return _y;
}

void MouseEvent::SetX(int x)
{
	_x = x;
}

void MouseEvent::SetY(int y)
{
	_y = y;
}

}

