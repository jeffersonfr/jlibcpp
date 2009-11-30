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
#ifndef J_COMBOBOX_H
#define J_COMBOBOX_H

#include "jselectlistener.h"
#include "jcomponent.h"
#include "jmenu.h"

#include <iostream>
#include <cstdlib>
#include <map>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

namespace jgui {

class MenuItem;
class ComboMenuComponent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class ComboMenu : public jgui::Frame, public jgui::FrameInputListener{

	friend class ComboMenuComponent;

	private:
		jthread::Mutex _menu_mutex;

		std::vector<ComboMenu *> _menus;
		std::vector<MenuListener *> _menu_listeners;
		MenuItem *_current_item;
		ComboMenuComponent *_list;
		int _visible_items;
		jmenu_align_t _menu_align;

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
		ComboMenu(int x, int y, int width, int visible_items);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComboMenu();

		/**
		 * \brief
		 *
		 */
		virtual bool Show(bool modal = true);
		
		/**
		 * \brief
		 *
		 */
		virtual void SetTitle(std::string title);

		/**
		 * \brief
		 *
		 */
		void SetComboMenuAlign(jmenu_align_t align);
		
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
		void SetForegroundColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetBackgroundColor(jcolor_t color);
		
		/**
		 * \brief
		 *
		 */
		void SetItemColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		void SetForegroundColor(int red, int green, int blue, int alpha);
		
		/**
		 * \brief
		 *
		 */
		void SetBackgroundColor(int red, int green, int blue, int alpha);

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
		virtual void InputChanged(KeyEvent *event);
};

class ComboMenuComponent : public Component{

	friend class ComboMenu;

	private:
		std::vector<MenuItem *>_items;
		OffScreenImage *prefetch;
		ComboMenu *_menu;
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
		void SetComboMenu(ComboMenu *menu);
		
	public:
		/**
		 * \brief
		 *
		 */
		ComboMenuComponent(int x, int y, int width, int visible_items);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComboMenuComponent();

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
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);


};
class ComboBox : public Component, public MenuListener{//, public jthread::Thread{

	private:
		std::vector<SelectListener *> _select_listeners;
		ComboMenu *_menu;
		int _arrow_size,
				_old_index;

	public:
		/**
		 * \brief
		 *
		 */
		ComboBox(int x = 0, int y = 0, int width = 0, int height = 0, int visible_items = 3);
		
		/**
		 * \brief
		 *
		 */
		virtual ~ComboBox();

		/**
		 * \brief
		 *
		 */
		void SetArrowSize(int size);
		
		/**
		 * \brief
		 *
		 */
		int GetArrowSize();
		
		/**
		 * \brief
		 *
		 */
		void SetVisibleItems(int max_items);
		
		/**
		 * \brief
		 *
		 */
		void SetLoop(bool b);
		
		/**
		 * \brief
		 *
		 */
		void SetIndex(int i);
		
		/**
		 * \brief
		 *
		 */
		std::string GetValue();
		
		/**
		 * \brief
		 *
		 */
		int GetIndex();
		
		/**
		 * \brief
		 *
		 */
		void Add(std::string text);
		
		/**
		 * \brief
		 *
		 */
		void Remove(int index);
		
		/**
		 * \brief
		 *
		 */
		void RemoveAll();

		/**
		 * \brief
		 *
		 */
		virtual void Paint(Graphics *g);
		
		/**
		 * \brief
		 *
		 */
		virtual void ItemSelected(MenuEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual void ItemChanged(MenuEvent *event);

		/**
		 * \brief
		 *
		 */
		void RegisterSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void RemoveSelectListener(SelectListener *listener);
		
		/**
		 * \brief
		 *
		 */
		void DispatchEvent(SelectEvent *event);
		
		/**
		 * \brief
		 *
		 */
		std::vector<SelectListener *> & GetSelectListeners();

		/**
		 * \brief
		 *
		 */
		virtual void Run();
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(KeyEvent *event);
		
		/**
		 * \brief
		 *
		 */
		virtual bool ProcessEvent(MouseEvent *event);

};

}

#endif

