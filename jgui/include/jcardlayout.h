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
#ifndef CARDLAYOUT_H
#define CARDLAYOUT_H

#include "jlayout.h"
#include "jcomponent.h"

#include <iostream>
#include <cstdlib>
#include <map>

namespace jgui {

struct jcardlayout_map_t{
	std::string name;
	Component *component;
};

class Container;

class CardLayout : public Layout{

	private:
		std::vector<jcardlayout_map_t> vector;
		int _hgap,
				_vgap,
				_current_card;

	public:
		CardLayout(int hgap = 10, int vgap = 10);
		virtual ~CardLayout();

		int GetHGap();
		int GetVGap();
		void SetHGap(int hgap);
		void SetVGap(int vgap);

		void AddLayoutComponent(std::string name, Component *comp);
		void RemoveLayoutComponent(Component *comp);

		void CheckLayout(Container *parent);
	
		void First(Container *parent);
		void Next(Container *parent);
		void Previous(Container *parent);
		void Last(Container *parent);

		void ShowDefaultComponent(Container *parent);

    virtual jsize_t GetMinimumLayoutSize(Container *parent);
    virtual jsize_t GetMaximumLayoutSize(Container *parent);
    virtual jsize_t GetPreferredLayoutSize(Container *parent);

		virtual void DoLayout(Container *parent);

};

}

#endif

