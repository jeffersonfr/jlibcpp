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

#include <string>
#include <iostream>
#include <vector>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

namespace jgui {

class MenuItem;
class TreeComponent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Tree : public jgui::Component{

	friend class TreeComponent;

	private:
		jthread::Mutex _tree_mutex;

		std::vector<Tree *> _trees;
		std::vector<MenuListener *> _tree_listeners;
		MenuItem *_current_item;
		TreeComponent *_list;
		int _visible_items;
		jmenu_align_t _menu_align;
		jcolor_t _item_color;

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
		void SetTreeAlign(jmenu_align_t align);
		
		/**
		 * \brief
		 *
		 */
		int GetItemsSize();
		
		/**
		 * \brief
		 *
		 */
		int GetVisibleItems();
		
		/**
		 * \brief
		 *
		 */
		void SetLoop(bool loop);
		
		/**
		 * \brief
		 *
		 */
		jcolor_t GetItemColor();
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		void SetCurrentIndex(int i);
		
		/**
		 * \brief
		 *
		 */
		void AddMenuItem(MenuItem *item);
		
		/**
		 * \brief
		 *
		 */
		MenuItem * GetCurrentItem();
		
		/**
		 * \brief
		 *
		 */
		int GetCurrentIndex();
		
		/**
		 * \brief
		 *
		 */
		void RemoveItem(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveAll();

		/**
		 * \brief
		 *
		 */
		void RegisterMenuListener(MenuListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveMenuListener(MenuListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchMenuEvent(MenuEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<MenuListener *> & GetMenuListeners();

		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TreeComponent : public Component{

	friend class Tree;

	private:
		std::vector<MenuItem *>_items;
		OffScreenImage *prefetch;
		Tree *_tree;
		int bx,
			by,
			bwidth,
			bheight,
			_item_size,
			_index,
			_visible_items,
			_paint_count,
			_vertical_gap,
			_horizontal_gap;
		jcolor_t _item_color;
		float delta;
		bool _input_locked,
			 _arrows_visible,
			 _loop;

		/**
		 * \brief
		 *
		 */
		void SetTree(Tree *tree);

	public:
		/**
		 * \brief
		 *
		 */
		TreeComponent(int x, int y, int width, int visible_items);
		
		/**
		 * \brief
		 *
		 */
		virtual ~TreeComponent();

		/**
		 * \brief
		 *
		 */
		void SetGap(int hgap, int vgap);
		
		/**
		 * \brief
		 *
		 */
		void SetLoop(bool loop);
		
		/**
		 * \brief
		 *
		 */
		jcolor_t GetItemColor();
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		void SetCurrentIndex(int i);
		
		/**
		 * \brief
		 *
		 */
		void AddEmptyItem();
		
		/**
		 * \brief
		 *
		 */
		void AddItem(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void AddItem(std::string text, std::string image);
		
		/**
		 * \brief
		 *
		 */
		void AddItem(std::string text, bool checked);
		
		/**
		 * \brief
		 *
		 */
		void AddMenuItem(MenuItem *item);
		
		/**
		 * \brief
		 *
		 */
		MenuItem * GetMenuItem(int index);
		
		/**
		 * \brief
		 *
		 */
		MenuItem * GetCurrentMenuItem();
		
		/**
		 * \brief
		 *
		 */
		int GetCurrentIndex();
		
		/**
		 * \brief
		 *
		 */
		int GetItemsSize();
		
		/**
		 * \brief
		 *
		 */
		void RemoveItem(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveAll();

		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);

};

}

#endif 
