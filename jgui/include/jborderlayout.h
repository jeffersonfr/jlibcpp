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
#ifndef BORDERLAYOUT_H
#define BORDERLAYOUT_H

#include "jlayout.h"
#include "jcomponent.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

enum jborderlayout_align_t {
	BL_UNKNOWN,
	BL_NORTH,
	BL_SOUTH,
	BL_EAST,
	BL_WEST,
	BL_CENTER,
	BL_BEFORE_FIRST_LINE,
	BL_AFTER_LAST_LINE,
	BL_BEFORE_LINE_BEGINS,
	BL_AFTER_LINE_ENDS,
	BL_PAGE_START = BL_BEFORE_FIRST_LINE,
	BL_PAGE_END = BL_AFTER_LAST_LINE,
	BL_LINE_START = BL_BEFORE_LINE_BEGINS,
	BL_LINE_END = BL_AFTER_LINE_ENDS
};

class BorderLayout : public Layout{

	private:
		Component *north,
							*west,
							*east,
							*south,
							*center,
							*firstLine,
							*lastLine,
							*firstItem,
							*lastItem;
		int _hgap,
				_vgap;

	public:
		BorderLayout(int hgap = 10, int vgap = 10);
		virtual ~BorderLayout();

		int GetHGap();
		int GetVGap();
		void SetHGap(int hgap);
		void SetVgap(int vgap);

		void AddLayoutComponent(Component *c, jborderlayout_align_t align);
		void RemoveLayoutComponent(Component *c);
		void RemoveLayoutComponents();

		jborderlayout_align_t GetConstraints(Component *c);

		Component * GetLayoutComponent(jborderlayout_align_t align);
		Component * GetLayoutComponent(Container *target, jborderlayout_align_t align);
		Component * GetChild(jborderlayout_align_t key, bool ltr);

    virtual jsize_t GetMinimumLayoutSize(Container *parent);
    virtual jsize_t GetMaximumLayoutSize(Container *parent);
    virtual jsize_t GetPreferredLayoutSize(Container *parent);

		virtual void DoLayout(Container *target);

};

}

#endif

