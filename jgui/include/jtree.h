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

class TreeItem;
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
		TreeItem *_current_item;
		TreeComponent *_list;
		int _visible_items;
		jmenu_align_t _menu_align;

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
		uint32_t GetItemColor();
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(uint32_t color);
		
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
		void AddTreeItem(TreeItem *item);
		
		/**
		 * \brief
		 *
		 */
		TreeItem * GetCurrentItem();
		
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
		void DispatchEvent(MenuEvent *event);
		
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
class TreeItem{

	friend class TreeComponent;
	friend class ComboTreeComponent;

	private:
		std::vector<TreeItem *> _childs;
		Tree *_parent;
		OffScreenImage *_prefetch;
		std::string _value,
			_image;
		jmenuitem_type_t _type;
		bool _is_checked,
			 _is_visible,
			 _enabled;

	public:
		TreeItem()
		{
			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_is_checked = false;
			_is_visible = true;
			_type = EMPTY_MENU_ITEM;
		}

		TreeItem(std::string value)
		{
			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_value = value;
			_is_checked = false;
			_is_visible = true;
			_type = TEXT_MENU_ITEM;
		}

		TreeItem(std::string value, std::string image)
		{
			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_image = image;
			_value = value;
			_is_checked = false;
			_is_visible = true;
			_type = IMAGE_MENU_ITEM;
		}

		TreeItem(std::string value, bool checked)
		{
			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_value = value;
			_is_checked = checked;
			_is_visible = true;
			_type = CHECK_MENU_ITEM;
		}

		~TreeItem()
		{
		}

		Tree * GetParent()
		{
			return _parent;
		}

		std::vector<TreeItem *> & GetSubItems()
		{
			return _childs;
		}

		void SetEnabled(bool b)
		{
			_enabled = b;
		}

		bool GetEnabled()
		{
			return _enabled;
		}

		void SetVisible(bool b)
		{
			_is_visible = b;
		}

		bool IsVisible()
		{
			return _is_visible;
		}

		void SetSelected(bool b)
		{
			_is_checked = b;
		}

		void SetParent(Tree *parent)
		{
			_parent = parent;
		}

		void AddSubItem(TreeItem *item)
		{
			if (_type == jgui::CHECK_MENU_ITEM) {
				throw MenuException("Item cannot accept childs");
			}

			_childs.push_back(item);
		}

		std::string GetValue()
		{
			return _value;
		}

		std::string GetImage()
		{
			return _image;
		}

		jmenuitem_type_t GetType()
		{
			return _type;
		}

		bool IsSelected()
		{
			return _is_checked;
		}

};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class TreeComponent : public Component{

	friend class Tree;

	private:
		std::vector<TreeItem *>_items;
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
		int _item_red,
			_item_green,
			_item_blue,
			_item_alpha;
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
		uint32_t GetItemColor();
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(uint32_t color);
		
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
		void AddTreeItem(TreeItem *item);
		
		/**
		 * \brief
		 *
		 */
		TreeItem * GetTreeItem(int index);
		
		/**
		 * \brief
		 *
		 */
		TreeItem * GetCurrentTreeItem();
		
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
