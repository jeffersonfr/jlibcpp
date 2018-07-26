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
#include "jgui/jrectangle.h"

namespace jgui {

Rectangle::~Rectangle()
{
}

bool Rectangle::Contains(jregion_t region1, jregion_t region2)
{
	return Contains(region1.x, region1.y, region1.width, region1.height, region2.x, region2.y, region2.width, region2.height);
}

bool Rectangle::Contains(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	return (x2 >= x1) && (y2 >= y1) && ((x2+w2) <= w1) && ((y2+h2) <= h1);
}

bool Rectangle::Intersects(jregion_t region1, jregion_t region2)
{
	return Intersects(region1.x, region1.y, region1.width, region1.height, region2.x, region2.y, region2.width, region2.height);
}

bool Rectangle::Intersects(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	int ax = x1, 
			ay = y1,
			bx = ax+w1,
			by = ay+h1;
	int cx = x2, 
			cy = y2,
			dx = cx+w2, 
			dy = cy+h2;

	return (((ax > dx)||(bx < cx)||(ay > dy)||(by < cy)) == 0);
}

jregion_t Rectangle::Intersection(jregion_t region1, jregion_t region2)
{
	return Intersection(region1.x, region1.y, region1.width, region1.height, region2.x, region2.y, region2.width, region2.height);
}

jregion_t Rectangle::Intersection(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{
	jregion_t region;

	region.x = 0;
	region.y = 0;
	region.width = 0;
	region.height = 0;

	int left = (std::max)(x1, x2),
		top = (std::max)(y1, y2),
		right = (std::min)(x1+w1, x2+w2),
		bottom = (std::min)(y1+h1, y2+h2);

	if (right > left && bottom > top) {
		region.x = left;
		region.y = top;
		region.width = right-left;
		region.height = bottom-top;
	}

	return region;
}

}

