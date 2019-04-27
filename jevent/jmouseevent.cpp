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
#include "jevent/jmouseevent.h"

namespace jevent {

MouseEvent::MouseEvent(void *source, jmouseevent_type_t type, jmouseevent_button_t button, jmouseevent_button_t buttons, int click_count, int x, int y):
  jevent::EventObject(source)
{
  jcommon::Object::SetClassName("jevent::MouseEvent");

  _click_count = click_count;
  _location.x = x;
  _location.y = y;
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

jgui::jpoint_t MouseEvent::GetLocation()
{
  return _location;
}

void MouseEvent::SetLocation(int x, int y)
{
  _location.x = x;
  _location.y = y;
}

void MouseEvent::SetLocation(jgui::jpoint_t location)
{
  _location = location;
}

}

