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
#include "jcoordinatelayout.h"
#include "jcontainer.h"

#include <limits.h>

namespace jgui {

CoordinateLayout::CoordinateLayout(int width, int height, jcoordinate_layout_t type):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::CoordinateLayout");

	_width = width;
	_height = height;
	_type = type;
}

CoordinateLayout::~CoordinateLayout()
{
}

void CoordinateLayout::SetWidth(int width)
{
	_width = width;
}

void CoordinateLayout::SetHeight(int height)
{
	_height = height;
}

void CoordinateLayout::SetType(jcoordinate_layout_t type)
{
	_type = type;
}

int CoordinateLayout::GetWidth()
{
	return _width;
}

int CoordinateLayout::GetHeight()
{
	return _height;
}

jcoordinate_layout_t CoordinateLayout::GetType()
{
	return _type;
}

jsize_t CoordinateLayout::GetPreferredSize(Container *target) 
{
	jsize_t t = {0, 0};

	int nmembers = target->GetComponentCount();

	for (int i=0; i<nmembers; i++) {
		Component *cmp = target->GetComponents()[i];

		t.width = std::max(t.width, cmp->GetX()+cmp->GetPreferredWidth());
		t.height = std::max(t.height, cmp->GetY()+cmp->GetPreferredHeight());
	}

	return t;
}

void CoordinateLayout::DoLayout(Container *target)
{
	if ((void *)target == NULL || target->GetWidth() == 0 || target->GetHeight() == 0) {
		return;
	}

	double sx = 1.0,
				 sy = 1.0;
	int nmembers = target->GetComponentCount();

	if ((_type & CL_HORIZONTAL) != 0) {
		sx = (double)target->GetWidth()/_width;
		_width = target->GetWidth();
	}

	if ((_type & CL_VERTICAL) != 0) {
		sy = (double)target->GetHeight()/_height;
		_height = target->GetHeight();
	}

	for (int i = 0 ; i < nmembers ; i++) {
		Component *m = target->GetComponents()[i];

		m->SetBounds((int)(m->GetX()*sx), (int)(m->GetY()*sy), m->GetPreferredWidth(), m->GetPreferredHeight());
	}
}

}

