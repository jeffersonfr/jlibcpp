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
#include "jselectevent.h"

namespace jgui {

SelectEvent::SelectEvent(void *source, jgui::Item *item, int index, jselectevent_type_t type):
	jcommon::EventObject(source)
{
	jcommon::Object::SetClassName("jgui::SelectEvent");

	_item = item;
	_index = index;
	_type = type;
}

SelectEvent::~SelectEvent()
{
}

jgui::Item * SelectEvent::GetItem()
{
	return _item;
}

int SelectEvent::GetIndex()
{
	return _index;
}

jselectevent_type_t SelectEvent::GetType()
{
	return _type;
}

}
