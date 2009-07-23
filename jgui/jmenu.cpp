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
#include "jmenu.h"
#include "jcommonlib.h"

namespace jgui {

Menu::Menu(int x, int y, int width, int visible_items):
   	jgui::Frame("", x, y, width, 1),
	jgui::FrameInputListener()
{
	jcommon::Object::SetClassName("jgui::Menu");

	_menu_align = SUBMENU_ALIGN;
	_current_item = NULL;
	_visible_items = visible_items;

	if (_visible_items <= 0) {
		_visible_items = 1;
	}

	// _list = new MenuComponent(_border_size, _border_size, width-2*_border_size, _visible_items);
	_list = new MenuComponent(_border_size, _border_size, width-2*_border_size, _visible_items);
	
	_list->SetBackgroundColor(0x00, 0x00, 0x00, 0x00);
	_list->SetMenu(this);

	Add(_list);

	_list->RequestFocus();
	
	SetUndecorated(true);
	SetDefaultExitEnabled(false);
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
	SetSize(_list->GetWidth()+2*_border_size, _list->GetHeight()+2*_border_size);

	Frame::RegisterInputListener(this);
}

Menu::~Menu() 
{
	Frame::RemoveInputListener(this);

	jthread::AutoLock lock(&_menu_mutex);

	while (_menus.size() > 0) {
		Menu *menu = (*_menus.begin());

		_menus.erase(_menus.begin());

		menu->Release();

		delete menu;
	}

	Release();

	delete _list;
}

void Menu::MousePressed(MouseEvent *event)
{
}

void Menu::MouseReleased(MouseEvent *event)
{
}

void Menu::MouseClicked(MouseEvent *event)
{
}

void Menu::MouseMoved(MouseEvent *event)
{
}

void Menu::MouseWheel(MouseEvent *event)
{
}

bool Menu::Show(bool modal)
{
	if (_list != NULL) {
		_list->RequestFocus();
	}

	return Frame::Show(modal);
}

void Menu::SetMenuAlign(jmenu_align_t align)
{
	_menu_align = align;
}

void Menu::SetTitle(std::string title)
{
	if (_title == title) {
		return;
	}

	_title = title;

	if (_title == "") {
		_list->SetPosition(_border_size, _border_size);
		
		SetSize(_list->GetWidth()+2*_border_size, _list->GetHeight()+2*_border_size);
	} else {
		_list->SetPosition(_border_size, _insets.top);
		
		SetSize(_list->GetWidth()+2*_border_size, _list->GetHeight()+2*_border_size+_insets.top);
	}

	if ((void *)graphics != NULL) {
		Repaint();
	}
}

int Menu::GetItemsSize()
{
	return _list->GetItemsSize();
}

int Menu::GetVisibleItems()
{
	return _visible_items;
}

void Menu::SetLoop(bool b)
{
	if (_list == NULL) {
		return;
	}

	_list->SetLoop(b);
}

uint32_t Menu::GetItemColor()
{
	return _list->GetItemColor();
}
	
void Menu::SetItemColor(uint32_t color)
{
	_list->SetItemColor(color);
}

void Menu::SetBackgroundColor(uint32_t color)
{
	Frame::SetBackgroundColor(color);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetBackgroundColor(color);
	}
}

void Menu::SetForegroundColor(uint32_t color)
{
	Frame::SetForegroundColor(color);
	_list->SetForegroundColor(color);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->_list->SetForegroundColor(color);
	}
}

void Menu::SetItemColor(int red, int green, int blue, int alpha)
{
	_list->SetItemColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->_list->SetItemColor(red, green, blue, alpha);
	}
}

void Menu::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	Frame::SetBackgroundColor(red, green, blue, alpha);

	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetBackgroundColor(red, green, blue, alpha);
	}
}

void Menu::SetForegroundColor(int red, int green, int blue, int alpha)
{
	Frame::SetForegroundColor(red, green, blue, alpha);
	_list->SetForegroundColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->_list->SetForegroundColor(red, green, blue, alpha);
	}
}

void Menu::SetCurrentIndex(int i)
{
	jthread::AutoLock lock(&_menu_mutex);

	_list->SetCurrentIndex(i);
}

void Menu::AddMenuItem(MenuItem *item)
{
	jthread::AutoLock lock(&_menu_mutex);

	if (item == NULL) {
		return;
	}

	_list->AddMenuItem(item);
}

Menu * Menu::GetCurrentMenu()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		return this;
	} else {
		return (*_menus.rbegin());
	}

	return NULL;
}

MenuItem * Menu::GetCurrentItem()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		if (_list->GetItemsSize() > 0) {
			return _list->GetCurrentMenuItem();
		}
	} else {
		Menu *menu = (*_menus.rbegin());

		if (menu->_list->GetItemsSize() > 0) {
			return menu->_list->GetMenuItem(GetCurrentIndex());
		}
	}

	return NULL;
}

int Menu::GetCurrentIndex()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		return _list->GetCurrentIndex();
	} else {
		Menu *menu = (*_menus.rbegin());

		return menu->_list->GetCurrentIndex();
	}

	return 0;
}

void Menu::RemoveItem(int index)
{
	{
		jthread::AutoLock lock(&_menu_mutex);

		// TODO:: 
	}
	
	Repaint();
}

void Menu::RemoveAll()
{
	if (_list == NULL) {
		return;
	}

	jthread::AutoLock lock(&_menu_mutex);

	_list->RemoveAll();
}

void Menu::InputChanged(KeyEvent *event)
{
	jthread::AutoLock lock(&_menu_mutex);

	if (event->GetType() != JKEY_PRESSED) {
		return;
	}

	Menu *last = NULL;

	if (_menus.size() == 0) {
		last = this;
	} else {
		last = (*_menus.rbegin());
	}

	if (event->GetSymbol() == jgui::JKEY_ESCAPE) {
		while (_menus.size() > 0) {
			Menu *menu = (*_menus.begin());

			_menus.erase(_menus.begin());

			menu->Release();

			delete menu;
		}

		Release();
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_UP || event->GetSymbol() == jgui::JKEY_CURSOR_DOWN) {
		if (last != this) {
			if (last->_list->ProcessEvent(event) == true) {
				DispatchEvent(new MenuEvent(this, CHANGE_MENU_ITEM_EVENT, GetCurrentItem()));
			}
		} else {
			DispatchEvent(new MenuEvent(this, CHANGE_MENU_ITEM_EVENT, GetCurrentItem()));
		}
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
		if (last != this) {
			_menus.erase(_menus.begin()+_menus.size()-1);

			last->Release();

			delete last;
		}

		if (_menus.size() == 0) {
			_list->RequestFocus();
		}

		DispatchEvent(new MenuEvent(this, CHANGE_MENU_ITEM_EVENT, GetCurrentItem()));
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT || event->GetSymbol() == jgui::JKEY_ENTER) {
		// TODO:: DispatchEvent

		MenuItem *item = GetCurrentItem();

		if (item != NULL && item->GetEnabled() == true) {
			if (event->GetSymbol() == jgui::JKEY_ENTER && item->GetType() == jgui::CHECK_MENU_ITEM) {
				bool b = true;

				if (item->IsSelected() == true) {
					b = false;
				}

				// item->SetParent(last);
				item->SetSelected(b);

				DispatchEvent(new MenuEvent(last, SELECT_MENU_ITEM_EVENT, item));
			} else {
				std::vector<MenuItem *> items = item->GetSubItems();

				if (items.size() > 0) {
					int position = last->GetCurrentIndex();
					
					if (position > last->GetVisibleItems()/2 && position < (last->GetItemsSize()-last->GetVisibleItems()/2)) {
						position = last->GetVisibleItems()/2;
					} else if (position >= (last->GetItemsSize()-last->GetVisibleItems()/2)) {
						if (last->GetItemsSize() < last->GetVisibleItems()) {
							position = last->GetCurrentIndex();
						} else {
							position = last->GetVisibleItems()-(last->GetItemsSize()-last->GetCurrentIndex());
						}
					}

					Menu *menu = NULL;
					
					if (_menu_align == MENU_ALIGN) {
						menu = new Menu(last->GetX()+last->GetWidth()+5, last->GetY(), last->GetWidth(), items.size());	
					} else if (_menu_align == SUBMENU_ALIGN) {
						if (_title == "") {
							menu = new Menu(last->GetX()+last->GetWidth()+5, last->GetY()+position*(last->GetHeight()/last->GetVisibleItems()), last->GetWidth(), items.size());	
						} else {
							int ytitle = 0;

							if (_menus.size() == 0) {
								ytitle = _insets.top-position*(_insets.top/last->GetVisibleItems());
							}

							menu = new Menu(last->GetX()+last->GetWidth()+5, ytitle+last->GetY()+position*(last->GetHeight()/last->GetVisibleItems()), last->GetWidth(), items.size());	
						}
					}

					uint32_t bg = GetBackgroundColor(),
							 fg = GetForegroundColor(),
							 itemg = GetItemColor();

					menu->SetBackgroundColor((bg>>16)&0xff, (bg>>8)&0xff, (bg>>0)&0xff, (bg>>24)&0xff);
					menu->SetForegroundColor((fg>>16)&0xff, (fg>>8)&0xff, (fg>>0)&0xff, (fg>>24)&0xff);
					menu->SetItemColor((itemg>>16)&0xff, (itemg>>8)&0xff, (itemg>>0)&0xff, (itemg>>24)&0xff);

					for (std::vector<MenuItem *>::iterator i=items.begin(); i!=items.end(); i++) {
						if ((*i)->IsVisible() == true) {
							menu->AddMenuItem((*i));
						}
					}

					_menus.push_back(menu);

					_current_item = (*items.begin());

					menu->SetInputEnabled(false);
					menu->Show(false);

					if (GetComponentInFocus() != NULL) {
						GetComponentInFocus()->ReleaseFocus();
					}
				
					DispatchEvent(new MenuEvent(last, CHANGE_MENU_ITEM_EVENT, GetCurrentItem()));
				} else {
					if (event->GetSymbol() == jgui::JKEY_ENTER) {
						while (_menus.size() > 0) {
							Menu *m = *(_menus.begin()+_menus.size()-1);

							_menus.erase(_menus.begin()+_menus.size()-1);

							m->Release();

							delete m;
						}

						Hide();
						DispatchEvent(new MenuEvent(NULL, SELECT_MENU_ITEM_EVENT, item));

						_frame_sem.Notify();
					}
				}
			}
		}
	}
}

void Menu::RegisterMenuListener(MenuListener *listener)
{
	if (listener == NULL) {
		return;
	}

	_listeners.push_back(listener);
}

void Menu::RemoveMenuListener(MenuListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<MenuListener *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if ((*i) == listener) {
			_listeners.erase(i);

			break;
		}
	}
}

void Menu::DispatchEvent(MenuEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<MenuListener *>::iterator i=_listeners.begin(); i!=_listeners.end(); i++) {
		if (event->GetType() == CHANGE_MENU_ITEM_EVENT) {
			(*i)->ItemChanged(event);
		} else if (event->GetType() == SELECT_MENU_ITEM_EVENT) {
			(*i)->ItemSelected(event);
		}
	}

	delete event;
}

std::vector<MenuListener *> & Menu::GetMenuListeners()
{
	return _listeners;
}

MenuComponent::MenuComponent(int x, int y, int width, int visible_items):
   	Component(x, y, width, 1)
{
	jcommon::Object::SetClassName("jgui::MenuComponent");
	
	_centered_interaction = true;
	_top_index = 0;
	bx = x+10;
	by = y+20;
	bwidth = 30;
	bheight = 30;
	_vertical_gap = 16;
	_horizontal_gap = 16;
	delta = 1.0f;

	_menu = NULL;
	_loop = false;
	_index = 0;
	_input_locked = false;
	_visible_items = visible_items;

	_item_size = DEFAULT_COMPONENT_HEIGHT;

	if (_font != NULL) {
		_item_size = _font->GetHeight();
	}

	prefetch = new OffScreenImage(_item_size, _item_size);
	
	if (prefetch->GetGraphics() != NULL) {
		prefetch->GetGraphics()->DrawImage("./icons/check.png", 0, 0, _item_size, _item_size);
	}

	SetSize(width, (_item_size+_vertical_gap)*visible_items-5);
}

MenuComponent::~MenuComponent() 
{
	delete prefetch;

	for (std::vector<MenuItem *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		OffScreenImage *p = (*i)->_prefetch;

		if (p != NULL) {
			delete p;
		}
	}
}

void MenuComponent::SetCenteredInteraction(bool b)
{
	_centered_interaction = b;
}

void MenuComponent::SetMenu(Menu *menu)
{
	_menu = menu;
}

int MenuComponent::GetItemsSize()
{
	return _items.size();
}

void MenuComponent::SetLoop(bool loop)
{
	if (_loop == loop) {
		return;
	}

	_loop = loop;

	Repaint();
}

uint32_t MenuComponent::GetItemColor()
{
	return (_item_alpha & 0xff) << 24 | (_item_red & 0xff) << 16 | (_item_green & 0xff) << 8 | (_item_blue & 0xff) << 0;
}
	
void MenuComponent::SetItemColor(uint32_t color)
{
	_item_red = (color>>0x10)&0xff;
	_item_green = (color>>0x08)&0xff;
	_item_blue = (color>>0x00)&0xff;
	_item_alpha = (color>>0x18)&0xff;
}

void MenuComponent::SetItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_red = red;
	_item_green = green;
	_item_blue = blue;
	_item_alpha = 0xff;//alpha;
}

void MenuComponent::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	// INFO:: este metodo estah comentado para permitir a atualizacao dos themes
	// Component::Paint(g);

	g->SetFont(_font);

	int i,
		count = 0,
		space = 15,
		font_height = _item_size;

	if (IsFontSet() == true) {
		font_height = _font->GetHeight();
	}

	if (_paint_count == 0) {
		_paint_count = 1;
	}

	int position;

	if (_centered_interaction == true) {
		position = _index-_visible_items/2;
	} else {
		position = _top_index;
	}

	if (position > (int)(_items.size()-_visible_items)) {
		position = (_items.size()-_visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	for (std::vector<MenuItem *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == IMAGE_MENU_ITEM || (*i)->GetType() == CHECK_MENU_ITEM) {
			space = 20+font_height;

			break;
		}
	}

	if ((int)_items.size() < _visible_items) {
		position = 0;
	}

	for (i=position; count<_visible_items && i<(int)_items.size(); i++, count++) {
		if (_index != i) {
			g->SetColor(_item_red, _item_green, _item_blue, _item_alpha);
			g->FillRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10);
		} else {
			g->SetColor(_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			FillRectangle(g, 0, (font_height+_vertical_gap)*count, _width, font_height+10);

			/*
			g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
			*/
		}

		if (_items[i]->GetType() == EMPTY_MENU_ITEM) {
			// TODO::
		} else if (_items[i]->GetType() == TEXT_MENU_ITEM) {
			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
			g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-2*space), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
		} else if (_items[i]->GetType() == IMAGE_MENU_ITEM) {
			if (_items[i]->_prefetch == NULL) {
				g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
				g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-2*space), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
			} else {
				g->DrawImage(_items[i]->_prefetch, 10, (font_height+_vertical_gap)*count, font_height, font_height+10);
				g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
				g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-2*space+font_height+10), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
			}
		} else if (_items[i]->GetType() == CHECK_MENU_ITEM) {
			if (_items[i]->IsSelected() == true) {
				g->DrawImage(prefetch, 10, 5+(font_height+_vertical_gap)*count, font_height, font_height);
			}

			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
			g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-2*space+font_height+10), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
		}

		if (_items[i]->GetEnabled() == false) {
			g->SetColor(0x00, 0x00, 0x00, 0x80);
			FillRectangle(g, 0, (font_height+_vertical_gap)*count, _width, font_height+10);
		}

		if (_menu != NULL) {
			if (_menu->_list->_items[i]->_childs.size() > 0) {
				int dx = _width-font_height/2-4,
					dy = (font_height+_vertical_gap)*count+5;

				g->SetColor(0x80, 0x80, 0xe0, 0xff);
				g->FillTriangle(dx, dy+2, dx+font_height/2, dy+font_height/2, dx, dy+font_height-4);
			}
		}
	}

	for (; count<_visible_items; count++) {
		g->SetColor(_item_red, _item_green, _item_blue, _item_alpha);
		FillRectangle(g, 0, (font_height+_vertical_gap)*count, _width, font_height+10);
	}
}

void MenuComponent::SetCurrentIndex(int i)
{
	if (i < 0) {
		i = 0;
	}

	if (_centered_interaction == true) {
		if (i >= (int)_items.size()) {
			if (_items.size() > 0) {
				i = _items.size()-1;
			} else {
				i = 0;
			}
		}

		if (_index != i) {
			{
				jthread::AutoLock lock(&_component_mutex);

				_index = i;
			}

			Repaint();
		}
	} else {
		if (i >= (int)_items.size()) {
			i = 0;
		}

		if (_index != i) {
			{
				jthread::AutoLock lock(&_component_mutex);

				_index = i;

				if (_index >= (int)_items.size()) {
					_index = 0;
				}

				if (_index < _top_index) {
					_top_index = _index;
				}

				if (_index >= (_top_index + _visible_items)) {
					_top_index = _index-_visible_items+1;

					if (_top_index < 0) {
						_top_index = 0;
					}
				}
			}

			Repaint();
		}
	}
}

void MenuComponent::AddEmptyItem()
{
	MenuItem *item = new MenuItem();

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void MenuComponent::AddItem(std::string text)
{
	MenuItem *item = new MenuItem(text);

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void MenuComponent::AddItem(std::string text, std::string image)
{
	MenuItem *item = new MenuItem(text, image);

	item->_prefetch = new OffScreenImage(_item_size, _item_size);

	if (item->_prefetch->GetGraphics() != NULL) {
		if (item->_prefetch->GetGraphics()->DrawImage(image, 0, 0, _item_size, _item_size) == false) {
			delete item->_prefetch;
			item->_prefetch = NULL;
		}
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void MenuComponent::AddItem(std::string text, bool checked)
{
	MenuItem *item = new MenuItem(text, checked);

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void MenuComponent::AddMenuItem(MenuItem *item)
{
	if (item == NULL) {
		return;
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

MenuItem * MenuComponent::GetMenuItem(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return NULL;
	}

	if (_items.size() > 0) {
		return _items[i];
	}

	return NULL;
}

MenuItem * MenuComponent::GetCurrentMenuItem()
{
	if (_items.size() > 0) {
		return _items[_index];
	} 

	return NULL;
}

int MenuComponent::GetCurrentIndex()
{
	return _index;
}

void MenuComponent::RemoveItem(int index)
{
	{
		jthread::AutoLock lock(&_component_mutex);
		
		// remove
	}

	Repaint();
}

void MenuComponent::RemoveAll()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_items.clear();
	}

	Repaint();
}

bool MenuComponent::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_UP) {
		int old_index = _index;

		_index--;

		if (_index < 0) {
			if (_loop == false) {
				_index = 0;
			} else {
				_index = (int)(_items.size()-1);
			}
		}

		if (_index < _top_index) {
			_top_index = _index;
		}

		if (_index != old_index) {
			Repaint();

			// DispatchEvent(new SelectEvent(this, _items[_index].text, _index, UP_ITEM)); 
		}

		catched = true;
	} else if (action == JKEY_CURSOR_DOWN) {
		int old_index = _index;

		_index++;

		if (_index >= (int)_items.size()) {
			if (_loop == false) {
				if (_items.size() > 0) {
					_index = _items.size()-1;
				} else {
					_index = 0;
				}
			} else {
				_index = 0;
			}
		}

		if (_index >= (_top_index + _visible_items)) {
			_top_index = _index-_visible_items+1;

			if (_top_index < 0) {
				_top_index = 0;
			}
		}

		if (_index != old_index) {
			Repaint();

			// DispatchEvent(new SelectEvent(this, _items[_index].text, _index, DOWN_ITEM)); 
		}

		catched = true;
	} else if (action == JKEY_ENTER) {
		if (_items[_index]->GetEnabled() == true) {
			catched = true;
		}
	}

	return catched;
}


}
