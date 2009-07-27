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
#ifndef SCROLLPANE_H
#define SCROLLPANE_H

#include "jguilib.h"
#include "jcontainer.h"
#include "jscrollbar.h"

#include <string>
#include <vector>
#include <algorithm>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jscrollpane_type_t {
	HORIZONTAL_SCROLL,
	VERTICAL_SCROLL,
	BOTH_SCROLL
};

class Layout;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ScrollPane : public jgui::Container{

	friend class Component;

	protected:
		ScrollBar *_horizontal_scroll,
					 *_vertical_scroll;
		jscrollpane_type_t _scroll_type;
		int _scroll_x,
			_scroll_y;
		int _scale_width, 
			_scale_height;
		bool _auto_scroll;

	public:
		/**
		 * \brief
		 *
		 */
		ScrollPane(int x = 0, int y = 0, int width = 0, int height = 0, int scale_width = DEFAULT_SCALE_WIDTH, int scale_height = DEFAULT_SCALE_HEIGHT);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ScrollPane();

		/**
		 * \brief
		 *
		 */
		virtual void SetAutoScroll(bool b);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetScrollType(jscrollpane_type_t type);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif

