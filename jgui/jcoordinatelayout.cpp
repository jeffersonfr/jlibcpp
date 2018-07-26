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
#include "jgui/jcoordinatelayout.h"
#include "jmath/jmath.h"
#include "jexception/joutofboundsexception.h"

namespace jgui {

CoordinateLayout::CoordinateLayout(int width, int height, jcoordinatelayout_orientation_t type):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::CoordinateLayout");

	if (width <= 0 || height <= 0) {
		// throw jexception::OutOfBoundsException("CoordinateLayout cannot work with limits lower than zero");
	}

	_size.width = width;
	_size.height = height;
	_type = type;
}

CoordinateLayout::~CoordinateLayout()
{
}

void CoordinateLayout::SetSize(jgui::jsize_t size)
{
	_size = size;
}

void CoordinateLayout::SetType(jcoordinatelayout_orientation_t type)
{
	_type = type;
}

jgui::jsize_t CoordinateLayout::GetSize()
{
	return _size;
}

jcoordinatelayout_orientation_t CoordinateLayout::GetType()
{
	return _type;
}

jsize_t CoordinateLayout::GetMinimumLayoutSize(Container *parent)
{
	jsize_t t = {0, 0};

	return t;
}

jsize_t CoordinateLayout::GetMaximumLayoutSize(Container *parent)
{
	jsize_t t = {INT_MAX, INT_MAX};

	return t;
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

  jgui::jsize_t 
    tsize = target->GetSize();

	if (tsize.width <= 0 || tsize.height <= 0) {
		return;
	}

	double 
    sx = 1.0,
		sy = 1.0;
	int 
    nmembers = target->GetComponentCount();

	if (_size.width < 0) {
		_size.width = tsize.width;
	}

	if (_size.height < 0) {
		_size.height = tsize.height;
	}

	if ((_type & JCLO_HORIZONTAL) != 0) {
		sx = (double)tsize.width/_size.width;
	}

	if ((_type & JCLO_VERTICAL) != 0) {
		sy = (double)tsize.height/_size.height;
	}

	for (int i=0; i<nmembers; i++) {
		Component *c = target->GetComponents()[i];

		jpoint_t point = c->GetLocation();
		jsize_t size = c->GetPreferredSize();

		// c->SetLocation((int)(point.x*sx), (int)(point.y*sy));
		c->SetBounds((int)(point.x*sx), (int)(point.y*sy), size.width, size.height);
	}

	_size.width = tsize.width;
	_size.height = tsize.height;
}

}

