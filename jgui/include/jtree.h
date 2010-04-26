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
#ifndef J_TREE_H
#define J_TREE_H

#include "jmenu.h"
#include "jitemcomponent.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Tree : 
	public jgui::Component,
	public jgui::ItemComponent
{

	private:
		jthread::Mutex _tree_mutex;

		jgui::OffScreenImage *_icon_plus,
			*_icon_minus;
		std::string _title;
		int _top_index,
				_item_size,
				_centered_interaction;

	private:
		/**
		 * \brief
		 *
		 */
		virtual void MousePressed(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseReleased(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseClicked(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseMoved(MouseEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual void MouseWheel(MouseEvent *event);

	public:
		/**
		 * \brief
		 *
		 */
		Tree(int x = 0, int y = 0, int width = 0, int visible_items = 3);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Tree();

		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);
		
		/**
		 * \brief
		 *
		 */
		void SetCenteredInteraction(bool b);

		/*
		 * \brief
		 *
		 */
		virtual void SetCurrentIndex(int i);

		/*
		 * \brief
		 *
		 */
		virtual Item * GetCurrentItem();

		/**
		 * \brief
		 *
		 */
		virtual int GetCurrentIndex();

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);

};

}

#endif
