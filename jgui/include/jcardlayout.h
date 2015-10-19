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
#ifndef J_CARDLAYOUT_H
#define J_CARDLAYOUT_H

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

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class CardLayout : public Layout{

	private:
		/** \brief */
		std::vector<jcardlayout_map_t> vector;
		/** \brief */
		int _hgap;
		/** \brief */
		int _vgap;
		/** \brief */
		int _current_card;

	public:
		/**
		 * \brief
		 *
		 */
		CardLayout(int hgap = 10, int vgap = 10);
		
		/**
		 * \brief
		 *
		 */
		virtual ~CardLayout();

		/**
		 * \brief
		 *
		 */
		virtual int GetHGap();
		
		/**
		 * \brief
		 *
		 */
		virtual int GetVGap();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetHGap(int hgap);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetVGap(int vgap);

		/**
		 * \brief
		 *
		 */
		virtual void AddLayoutComponent(std::string name, Component *comp);
		
		/**
		 * \brief
		 *
		 */
		virtual void RemoveLayoutComponent(Component *comp);

		/**
		 * \brief
		 *
		 */
		virtual void CheckLayout(Container *parent);
	
		/**
		 * \brief
		 *
		 */
		virtual void First(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual void Next(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual void Previous(Container *parent);
		
		/**
		 * \brief
		 *
		 */
		virtual void Last(Container *parent);

		/**
		 * \brief
		 *
		 */
		virtual void ShowDefaultComponent(Container *parent);

		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMinimumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetMaximumLayoutSize(Container *parent);
		
		/**
		 * \brief
		 *
		 */
    virtual jsize_t GetPreferredLayoutSize(Container *parent);

		/**
		 * \brief
		 *
		 */
		virtual void DoLayout(Container *parent);

};

}

#endif

