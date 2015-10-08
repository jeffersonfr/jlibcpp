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
#include "jtouchevent.h"

namespace jgui {


TouchEvent::TouchEvent(void *source, jtouchevent_type_t type, jpoint_t location, jpoint_t distance, double pressure, int finger_index):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jgui::TouchEvent");
		
	_type = type;
	_location = location;
	_distance = distance;
	_pressure = pressure;
	_delta = 0.0;
	_radians = 0.0;
	_finger_index = finger_index;
	_fingers = -1;
}

TouchEvent::TouchEvent(void *source, jtouchevent_type_t type, jpoint_t distance, double radians, double delta, int fingers):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jgui::TouchEvent");
	
	_type = type;
	// _location = location;
	_distance = distance;
	_pressure = 0.0;
	_delta = delta;
	_radians = radians;
	_finger_index = -1;
	_fingers = fingers;
}

TouchEvent::~TouchEvent()
{
}

jtouchevent_type_t TouchEvent::GetType()
{
	return _type;
}

int TouchEvent::GetFingerIndex()
{
	return _finger_index;
}

jpoint_t TouchEvent::GetLocation()
{
	return _location;
}

jpoint_t TouchEvent::GetDistance()
{
	return _distance;
}

double TouchEvent::GetPressure()
{
	return _pressure;
}

double TouchEvent::GetAngle()
{
	return _radians;
}

double TouchEvent::GetDelta()
{
	return _delta;
}

double TouchEvent::GetFingers()
{
	return _fingers;
}

}

