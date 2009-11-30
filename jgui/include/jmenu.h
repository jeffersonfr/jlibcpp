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

#include "jmenulistener.h"
#include "jframe.h"
#include "jlistbox.h"
#include "jmenuexception.h"
#include "joffscreenimage.h"
#include "jmouselistener.h"

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
 */
enum jmenu_align_t {
	MENU_ALIGN,
	SUBMENU_ALIGN
};

/**
 * \brief
 *
 */
enum jmenuitem_type_t {
	TEXT_MENU_ITEM,
	IMAGE_MENU_ITEM,
	CHECK_MENU_ITEM,
	EMPTY_MENU_ITEM
};

class MenuItem;
class MenuComponent;
class ComboMenuComponent;

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class Menu : public jgui::Frame, public jgui::FrameInputListener{

	friend class MenuComponent;

	private:
		jthread::Mutex _menu_mutex;

		std::vector<Menu *> _menus;
		std::vector<MenuListener *> _menu_listeners;
		MenuItem *_current_item;
		MenuComponent *_list;
		jmenu_align_t _menu_align;
		int _visible_items;

	private:
		virtual void MousePressed(MouseEvent *event);
		virtual void MouseReleased(MouseEvent *event);
		virtual void MouseClicked(MouseEvent *event);
		virtual void MouseMoved(MouseEvent *event);
		virtual void MouseWheel(MouseEvent *event);

	public:
		Menu(int x, int y, int width, int visible_items);
		virtual ~Menu();

		virtual void SetTitle(std::string title);
		virtual bool Show(bool modal = true);

		void SetMenuAlign(jmenu_align_t align);
		int GetItemsSize();
		int GetVisibleItems();
		void SetLoop(bool loop);
		void SetCurrentIndex(int i);
		void AddMenuItem(MenuItem *item);
		void AddMenuItem(MenuItem *item, int index);
		void AddMenuItems(std::vector<MenuItem *> &items);
		Menu * GetCurrentMenu();
		MenuItem * GetCurrentItem();
		int GetCurrentIndex();
		void RemoveAll();

		jcolor_t GetItemColor();

		void SetItemColor(jcolor_t color);
		void SetBackgroundColor(jcolor_t color);
		void SetForegroundColor(jcolor_t color);
		void SetItemColor(int red, int green, int blue, int alpha);
		void SetBackgroundColor(int red, int green, int blue, int alpha);
		void SetForegroundColor(int red, int green, int blue, int alpha);

		void RegisterMenuListener(MenuListener *listener);
		void RemoveMenuListener(MenuListener *listener);
		void DispatchEvent(MenuEvent *event);
		std::vector<MenuListener *> & GetMenuListeners();

		virtual void Paint(Graphics *g);
		virtual void InputChanged(KeyEvent *event);
};

class MenuItem : public virtual jcommon::Object{

	friend class MenuComponent;
	friend class ComboMenuComponent;

	private:
		std::vector<MenuItem *> _childs;
		Menu *_parent;
		OffScreenImage *_prefetch;
		std::string _value,
			_image;
		jmenuitem_type_t _type;
		bool _is_checked,
			 _is_visible,
			 _enabled;

	public:
		MenuItem()
		{
			jcommon::Object::SetClassName("jcommon::MenuItem");

			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_is_checked = false;
			_is_visible = true;
			_type = EMPTY_MENU_ITEM;
		}

		MenuItem(std::string value)
		{
			jcommon::Object::SetClassName("jcommon::MenuItem");

			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_value = value;
			_is_checked = false;
			_is_visible = true;
			_type = TEXT_MENU_ITEM;
		}

		MenuItem(std::string value, std::string image)
		{
			jcommon::Object::SetClassName("jcommon::MenuItem");

			_enabled = true;
			_parent = NULL;
			_prefetch = NULL;
			_image = image;
			_value = value;
			_is_checked = false;
			_is_visible = true;
			_type = IMAGE_MENU_ITEM;
		}

		MenuItem(std::string value, bool checked)
	{
		jcommon::Object::SetClassName("jcommon::MenuItem");

		_enabled = true;
		_parent = NULL;
		_prefetch = NULL;
		_value = value;
		_is_checked = checked;
		_is_visible = true;
		_type = CHECK_MENU_ITEM;
	}

	~MenuItem()
	{
	}

	Menu * GetParent()
	{
		return _parent;
	}

	std::vector<MenuItem *> & GetSubItems()
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

	bool IsSelected()
	{
		return _is_checked;
	}

	void SetSelected(bool b)
	{
		if (_is_checked == b) {
			return;
		}

		_is_checked = b;

		/*
		if (_parent != NULL) {
			_parent->Repaint();
		}
		*/
	}

	void SetParent(Menu *parent)
	{
		_parent = parent;
	}

	void AddSubItem(MenuItem *item)
	{
		if (_type == jgui::CHECK_MENU_ITEM) {
			throw MenuException("Item cannot accept childs");
		}

		_childs.push_back(item);
	}

	void AddSubItem(MenuItem *item, int index)
	{
		if (index > (int)_childs.size()) {
			index = _childs.size();
		}

		_childs.insert(_childs.begin()+index, item);
	}

	void AddSubItems(std::vector<MenuItem *> items)
	{
		for (std::vector<MenuItem *>::iterator i=items.begin(); i!=items.end(); i++) {
			_childs.push_back((*i));
		}
	}

	void RemoveSubItem(int index)
	{
		if (_childs.size() > 0 && index < (int)_childs.size()) {
			_childs.erase(_childs.begin()+index);
		}
	}

	void RemoveSubItem(MenuItem *item)
	{
		std::vector<MenuItem *>::iterator i = std::find(_childs.begin(), _childs.end(), item);

		if (i != _childs.end()) {
			_childs.erase(i);
		}
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

};

class MenuComponent : public Component{

friend class Menu;

private:
	std::vector<MenuItem *>_items;
	OffScreenImage *prefetch;
	Menu *_menu;
	int bx,
		by,
		bwidth,
		bheight,
			_item_size,
			_index,
			_top_index,
			_visible_items,
			_paint_count,
			_vertical_gap,
			_horizontal_gap;
		jcolor_t _item_color;
		float delta;
		bool _input_locked,
			 _arrows_visible,
			 _loop,
			 _centered_interaction;

		void SetMenu(Menu *menu);

	public:
		MenuComponent(int x, int y, int width, int visible_items);
		virtual ~MenuComponent();

		void SetCenteredInteraction(bool b);
		void SetLoop(bool loop);
		jcolor_t GetItemColor();
		void SetItemColor(jcolor_t color);
		void SetItemColor(int red, int green, int blue, int alpha);
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

		virtual bool ProcessEvent(KeyEvent *event);
		virtual void Paint(Graphics *g);

};

}

#endif 
