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
#ifndef MENUGROUP_H
#define MENUGROUP_H

#include "jeventobject.h"
#include "jmenu.h"
#include "jmenulistener.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class MenuGroup : public MenuListener{

	private:
		std::vector<MenuItem *> _items;
		Menu *_menu;

	protected:
		virtual void ItemSelected(MenuEvent *event);
		virtual void ItemChanged(MenuEvent *event);

	public:
		MenuGroup(Menu *menu);
		virtual ~MenuGroup();

		void Add(MenuItem *item);
		void Remove(MenuItem *item);

		void Select(MenuItem *item);
		MenuItem * GetSelected();

};

}

#endif

