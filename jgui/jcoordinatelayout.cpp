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
#include "jcoordinatelayout.h"
#include "jmath.h"

namespace jgui {

CoordinateLayout::CoordinateLayout(int width, int height, jcoordinate_layout_t type):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::CoordinateLayout");

	if (width <= 0 || height <= 0) {
		// throw jcommon::InvalidArgumentException("CoordinateLayout cannot work with limits lower than zero");
	}

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
	if ((void *)target == NULL) {
		jsize_t t = {0, 0};

		return t;
	}

	jinsets_t insets = target->GetInsets();
	jsize_t t = {0, 0};

	int members = target->GetComponentCount();

	for (int i=0; i<members; i++) {
		Component *cmp = target->GetComponents()[i];

		jpoint_t point = cmp->GetLocation();
		jsize_t size = cmp->GetSize();

		t.width = jmath::Math<int>::Max(t.width, point.x+size.width+(insets.left+insets.right));
		t.height = jmath::Math<int>::Max(t.height, point.y+size.height+(insets.top+insets.bottom));
	}

	return t;
}

void CoordinateLayout::DoLayout(Container *target)
{
	if ((void *)target == NULL) {
		return;
	}

	jsize_t tsize = target->GetSize();

	if (tsize.width <= 0 || tsize.height <= 0) {
		return;
	}

	int nmembers = target->GetComponentCount();

	double sx = 1.0,
				 sy = 1.0;

	if (_width < 0) {
		_width = tsize.width;
	}

	if (_height < 0) {
		_height = tsize.height;
	}

	if ((_type & CL_HORIZONTAL) != 0) {
		sx = (double)tsize.width/_width;
	}

	if ((_type & CL_VERTICAL) != 0) {
		sy = (double)tsize.height/_height;
	}

	for (int i = 0 ; i < nmembers ; i++) {
		Component *c = target->GetComponents()[i];

		jpoint_t point = c->GetLocation();
		jsize_t size = c->GetSize();

		// c->SetLocation((int)(point.x*sx), (int)(point.y*sy));
		c->SetBounds((int)(point.x*sx), (int)(point.y*sy), (int)(size.width*sx), (int)(size.height*sy));
	}

	_width = tsize.width;
	_height = tsize.height;
}

}

