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

bool Rectangle::Contains(jrect_t<int> r1, jrect_t<int> r2)
{
  return (r2.point.x >= r1.point.x) && (r2.point.y >= r1.point.y) && ((r2.point.x + r2.size.width) <= r1.size.width) && ((r2.point.y + r2.size.height) <= r1.size.height);
}

bool Rectangle::Intersects(jrect_t<int> r1, jrect_t<int> r2)
{
  return (((r1.point.x > (r2.point.x + r2.size.width))||((r1.point.x + r1.size.width) < r2.point.x)||(r1.point.y > (r2.point.y + r2.size.height))||((r1.point.y + r1.size.height) < r2.point.y)) == 0);
}

jrect_t<int> Rectangle::Intersection(jrect_t<int> r1, jrect_t<int> r2)
{
  int 
    left = (std::max)(r1.point.x, r2.point.x),
    top = (std::max)(r1.point.y, r2.point.y),
    right = (std::min)(r1.point.x + r1.size.width, r2.point.x + r2.size.width),
    bottom = (std::min)(r1.point.y + r1.size.height, r2.point.y + r2.size.height);

  if (right > left && bottom > top) {
    return {{left, top}, {right - left, bottom - top}};
  }

  return {0, 0, 0, 0};
}

}

