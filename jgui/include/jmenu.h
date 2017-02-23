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
#ifndef J_MENU_H
#define J_MENU_H

#include "jwidget.h"
#include "jmouselistener.h"
#include "jitemcomponent.h"
#include "jcondition.h"
#include "jimage.h"

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>

namespace jgui {

/**
 * \brief
 *
 */
enum jmenu_align_t {
	JMA_TITLE,
	JMA_ITEM,
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Menu : public jgui::Widget, public jgui::ItemComponent{

	private:
		/** \brief */
		jthread::Mutex _menu_mutex;
		/** \brief */
		jthread::Condition _menu_sem;
		/** \brief */
		std::vector<Menu *> _menus;
		/** \brief */
		jgui::Image *_check;
		/** \brief */
		jmenu_align_t _menu_align;
		/** \brief */
		std::string _title;
		/** \brief */
		int _top_index;
		/** \brief */
		int _item_size;
		/** \brief */
		int _visible_items;

	public:
		/**
		 * \brief
		 *
		 */
		Menu(int x, int y, int width, int visible_items);
		
		/**
		 * \brief
		 *
		 */
		virtual ~Menu();
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetMenuAlign(jmenu_align_t align);
		
		/**
		 * \brief
		 *
		 */
		virtual int GetVisibleItems();

		/**
		 * \brief
		 *
		 */
		virtual Menu * GetCurrentMenu();

		/**
		 * \brief
		 *
		 */
		virtual void SetItemColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetItemForegroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetBackgroundColor(int red, int green, int blue, int alpha);

		/**
		 * \brief
		 *
		 */
		virtual void SetForegroundColor(int red, int green, int blue, int alpha);

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
		virtual void Repaint(Component *cmp = NULL);

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

		/**
		 * \brief
		 *
		 */
		virtual bool KeyPressed(KeyEvent *event);

		/**
		 * \brief
		 *
		 */
		virtual bool MouseWheel(MouseEvent *event);

};

}

#endif 

