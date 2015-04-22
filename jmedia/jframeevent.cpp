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
#include "jframeevent.h"

namespace jmedia {

FrameEvent::FrameEvent(void *source, jframe_event_t type, jgui::Image *image):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jmedia::FrameEvent");

	_type = type;
	_image = image;
}
		
FrameEvent::~FrameEvent()
{
}

jframe_event_t FrameEvent::GetType()
{
	return _type;
}

jgui::Image * FrameEvent::GetFrame()
{
	return _image;
}

}
