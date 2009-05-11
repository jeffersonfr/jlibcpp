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
#include "jmenuevent.h"
#include "jmenu.h"

namespace jgui {

MenuEvent::MenuEvent(void *source, jmenu_event_t type, MenuItem *item):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jgui::MenuEvent");

	_type = type;
	_item = item;
}

MenuEvent::~MenuEvent()
{
}

jmenu_event_t MenuEvent::GetType()
{
	return _type;
}

MenuItem * MenuEvent::GetMenuItem()
{
	return _item;
}

}
