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
#ifndef COMBOBOX_H
#define COMBOBOX_H

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

class ComboMenu : public jgui::Frame, public jgui::FrameInputListener{

	friend class ComboMenuComponent;

	private:
		jthread::Mutex _menu_mutex;

		std::vector<ComboMenu *> _menus;
		std::vector<MenuListener *> _listeners;
		MenuItem *_current_item;
		ComboMenuComponent *_list;
		int _visible_items;
		jmenu_align_t _menu_align;

	private:
		virtual void MousePressed(MouseEvent *event);
		virtual void MouseReleased(MouseEvent *event);
		virtual void MouseClicked(MouseEvent *event);
		virtual void MouseMoved(MouseEvent *event);
		virtual void MouseWheel(MouseEvent *event);

	public:
		ComboMenu(int x, int y, int width, int visible_items);
		virtual ~ComboMenu();

		virtual bool Show(bool modal = true);
		virtual void SetTitle(std::string title);

		void SetComboMenuAlign(jmenu_align_t align);
		int GetItemsSize();
		int GetVisibleItems();
		void SetLoop(bool loop);
		void SetCurrentIndex(int i);
		void AddMenuItem(MenuItem *item);
		MenuItem * GetCurrentItem();
		int GetCurrentIndex();
		void RemoveItem(int index);
		void RemoveAll();
		
		uint32_t GetItemColor();

		void SetItemColor(uint32_t color);
		void SetForegroundColor(uint32_t color);
		void SetBackgroundColor(uint32_t color);
		void SetItemColor(int red, int green, int blue, int alpha);
		void SetForegroundColor(int red, int green, int blue, int alpha);
		void SetBackgroundColor(int red, int green, int blue, int alpha);

		void RegisterMenuListener(MenuListener *listener);
		void RemoveMenuListener(MenuListener *listener);
		void DispatchEvent(MenuEvent *event);
		std::vector<MenuListener *> & GetMenuListeners();

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
		int _item_red,
			_item_green,
			_item_blue,
			_item_alpha;
		float delta;
		bool _input_locked,
			 _arrows_visible,
			 _loop;

		void SetComboMenu(ComboMenu *menu);
		
	public:
		ComboMenuComponent(int x, int y, int width, int visible_items);
		virtual ~ComboMenuComponent();

		void SetGap(int hgap, int vgap);
		void SetLoop(bool loop);
		void SetCurrentIndex(int i);
		void AddEmptyItem();
		void AddItem(std::string text);
		void AddItem(std::string text, std::string image);
		void AddItem(std::string text, bool checked);
		void AddMenuItem(MenuItem *item);
		MenuItem * GetMenuItem(int index);
		MenuItem * GetCurrentMenuItem();
		int GetCurrentIndex();
		int GetItemsSize();
		void RemoveItem(int index);
		void RemoveAll();

		uint32_t GetItemColor();
		
		void SetItemColor(uint32_t color);
		void SetItemColor(int red, int green, int blue, int alpha);

		virtual void Paint(Graphics *g);
		virtual bool ProcessEvent(KeyEvent *event);


};
class ComboBox : public Component, public MenuListener{//, public jthread::Thread{

	private:
		std::vector<SelectListener *> _select_listeners;
		ComboMenu *_menu;
		int _arrow_size;

	public:
		ComboBox(int x = 0, int y = 0, int width = 0, int height = 0, int visible_items = 3);
		virtual ~ComboBox();

		void SetArrowSize(int size);
		int GetArrowSize();
		void SetVisibleItems(int max_items);
		void SetLoop(bool b);
		void SetIndex(int i);
		std::string GetValue();
		int GetIndex();
		void Add(std::string text);
		void Remove(int index);
		void RemoveAll();

		virtual void Paint(Graphics *g);
		virtual void ItemSelected(MenuEvent *event);
		virtual void ItemChanged(MenuEvent *event);

		void RegisterSelectListener(SelectListener *listener);
		void RemoveSelectListener(SelectListener *listener);
		void DispatchEvent(SelectEvent *event);
		std::vector<SelectListener *> & GetSelectListeners();

		virtual void Run();
		virtual bool ProcessEvent(KeyEvent *event);
		virtual bool ProcessEvent(MouseEvent *event);

};

}

#endif

