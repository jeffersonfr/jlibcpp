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
#ifndef COORDINATELAYOUT_H
#define COORDINATELAYOUT_H

#include "jlayout.h"
#include "jcontainer.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

enum jcoordinate_layout_t {
	CL_HORIZONTAL	= 1,
	CL_VERTICAL		= 2
};

class CoordinateLayout : public Layout{

	private:
		jcoordinate_layout_t _type;
		int _width,
				_height;

	public:
		CoordinateLayout(int hgap = 10, int vgap = 10, jcoordinate_layout_t type = (jcoordinate_layout_t)(CL_HORIZONTAL | CL_VERTICAL));
		virtual ~CoordinateLayout();

		void SetWidth(int width);
		void SetHeight(int height);
		void SetType(jcoordinate_layout_t type);

		int GetWidth();
		int GetHeight();
		jcoordinate_layout_t GetType();

		jsize_t GetPreferredSize(Container *target);

		virtual void DoLayout(Container *parent);

};

}

#endif

