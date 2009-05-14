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
#include "jcombobox.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

namespace jgui {

ComboMenu::ComboMenu(int x, int y, int width, int visible_items):
   	jgui::Frame("", x, y, width, 1),
	jgui::FrameInputListener()
{
	jcommon::Object::SetClassName("jgui::ComboBox");

	_menu_align = SUBMENU_ALIGN;
	_current_item = NULL;
	_visible_items = visible_items;

	if (_visible_items <= 0) {
		_visible_items = 1;
	}

	// _list = new ComboMenuComponent(_border_size, _border_size, width-2*_border_size, _visible_items);
	_list = new ComboMenuComponent(_border_size, _border_size, width-2*_border_size, _visible_items);
	
	_list->SetComboMenu(this);

	Add(_list);

	_list->RequestFocus();
	
	SetUndecorated(true);
	SetDefaultExitEnabled(false);
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
	SetSize(_list->GetWidth()+2*_border_size, _list->GetHeight()+2*_border_size);

	RegisterInputListener(this);
}

ComboMenu::~ComboMenu() 
{
	Frame::RemoveInputListener(this);

	jthread::AutoLock lock(&_menu_mutex);

	while (_menus.size() > 0) {
		ComboMenu *menu = (*_menus.begin());

		_menus.erase(_menus.begin());

		menu->Release();

		delete menu;
	}

	Release();

	delete _list;
}

bool ComboMenu::Show(bool modal)
{
	if (_list != NULL) {
		_list->RequestFocus();
	}

	return Frame::Show(modal);
}

void ComboMenu::SetComboMenuAlign(jmenu_align_t align)
{
	_menu_align = align;
}

void ComboMenu::SetTitle(std::string title)
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

int ComboMenu::GetItemsSize()
{
	return _list->GetItemsSize();
}

int ComboMenu::GetVisibleItems()
{
	return _visible_items;
}

void ComboMenu::SetLoop(bool b)
{
	if (_list == NULL) {
		return;
	}

	_list->SetLoop(b);
}

uint32_t ComboMenu::GetItemColor()
{
	return _list->GetItemColor();
}
	
void ComboMenu::SetItemColor(uint32_t color)
{
	_list->SetItemColor(color);
}

void ComboMenu::SetBackgroundColor(uint32_t color)
{
	_list->SetBackgroundColor(color);
	Frame::SetBackgroundColor(color);
}

void ComboMenu::SetForegroundColor(uint32_t color)
{
	_list->SetForegroundColor(color);
	Frame::SetForegroundColor(color);
}

void ComboMenu::SetItemColor(int red, int green, int blue, int alpha)
{
	_list->SetItemColor(red, green, blue, alpha);
}

void ComboMenu::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	_list->SetBackgroundColor(red, green, blue, alpha);
	Frame::SetBackgroundColor(red, green, blue, alpha);
}

void ComboMenu::SetForegroundColor(int red, int green, int blue, int alpha)
{
	_list->SetForegroundColor(red, green, blue, alpha);
	Frame::SetForegroundColor(red, green, blue, alpha);
}

void ComboMenu::SetCurrentIndex(int i)
{
	jthread::AutoLock lock(&_menu_mutex);

	_list->SetCurrentIndex(i);
}

void ComboMenu::AddMenuItem(MenuItem *item)
{
	jthread::AutoLock lock(&_menu_mutex);

	if (item == NULL) {
		return;
	}

	_list->AddMenuItem(item);
}

MenuItem * ComboMenu::GetCurrentItem()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		if (_list->GetItemsSize() > 0) {
			return _list->GetCurrentMenuItem();
		}
	} else {
		ComboMenu *menu = (*_menus.rbegin());

		if (menu->_list->GetItemsSize() > 0) {
			return menu->_list->GetMenuItem(GetCurrentIndex());
		}
	}

	return NULL;
}

int ComboMenu::GetCurrentIndex()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		return _list->GetCurrentIndex();
	} else {
		ComboMenu *menu = (*_menus.rbegin());

		return menu->_list->GetCurrentIndex();
	}

	return 0;
}

void ComboMenu::RemoveItem(int index)
{
	{
		jthread::AutoLock lock(&_menu_mutex);

		// TODO:: 
	}
	
	Repaint();
}

void ComboMenu::RemoveAll()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_list == NULL) {
		return;
	}

	_list->RemoveAll();
}

void ComboMenu::InputChanged(KeyEvent *event)
{
	jthread::AutoLock lock(&_menu_mutex);

	if (event->GetType() != JKEY_PRESSED) {
		return;
	}

	ComboMenu *last = NULL;

	if (_menus.size() == 0) {
		last = this;
	} else {
		last = (*_menus.rbegin());
	}

	if (event->GetSymbol() == jgui::JKEY_ESCAPE) {
		while (_menus.size() > 0) {
			ComboMenu *menu = (*_menus.begin());

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

				item->SetSelected(b);

				// CHANGE:: needs friend class
				last->_list->GetCurrentMenuItem()->SetSelected(b);

				last->Repaint();
			
				DispatchEvent(new MenuEvent(last, SELECT_MENU_ITEM_EVENT, item));
				// DispatchEvent(new MenuEvent(last, CHANGE_MENU_ITEM, GetCurrentItem()));
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

					ComboMenu *menu = NULL;
					
					if (_menu_align == MENU_ALIGN) {
						menu = new ComboMenu(last->GetX()+last->GetWidth()+5, last->GetY(), last->GetWidth(), items.size());	
					} else if (_menu_align == SUBMENU_ALIGN) {
						if (_title == "") {
							menu = new ComboMenu(last->GetX()+last->GetWidth()+5, last->GetY()+position*(last->GetHeight()/last->GetVisibleItems()), last->GetWidth(), items.size());	
						} else {
							int ytitle = 0;

							if (_menus.size() == 0) {
								ytitle = _insets.top-position*(_insets.top/last->GetVisibleItems());
							}

							menu = new ComboMenu(last->GetX()+last->GetWidth()+5, ytitle+last->GetY()+position*(last->GetHeight()/last->GetVisibleItems()), last->GetWidth(), items.size());	
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
							ComboMenu *m = *(_menus.begin()+_menus.size()-1);

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
		} else {
			Release();
		}
	}
}

void ComboMenu::MousePressed(MouseEvent *event)
{
	if (_input_locked == true || _enabled == false) {
		return;
	}
	
	if (event->GetButton() != JBUTTON_BUTTON1) {
		return;
	}

	_input_locked = true;

	jthread::AutoLock lock(&_input_mutex);

	if ((event->GetX() < _x || event->GetX() > (_x+_width)) || (event->GetY() < _y || event->GetY() > (_y+_height))) {
		Release();
	}
	
	_input_locked = false;
}

void ComboMenu::MouseReleased(MouseEvent *event)
{
}

void ComboMenu::MouseClicked(MouseEvent *event)
{
}

void ComboMenu::MouseMoved(MouseEvent *event)
{
}

void ComboMenu::MouseWheel(MouseEvent *event)
{
}

void ComboMenu::RegisterMenuListener(MenuListener *listener)
{
	if (listener == NULL) {
		return;
	}

	_listeners.push_back(listener);
}

void ComboMenu::RemoveMenuListener(MenuListener *listener)
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

void ComboMenu::DispatchEvent(MenuEvent *event)
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

std::vector<MenuListener *> & ComboMenu::GetMenuListeners()
{
	return _listeners;
}

ComboMenuComponent::ComboMenuComponent(int x, int y, int width, int visible_items):
   	Component(x, y, width, 1)
{
	jcommon::Object::SetClassName("jgui::ComboMenuComponent");

	bx = x+10;
	by = y+20;
	bwidth = 30;
	bheight = 30;
	_vertical_gap = 15;
	_horizontal_gap = 15;
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

ComboMenuComponent::~ComboMenuComponent() 
{
	delete prefetch;

	for (std::vector<MenuItem *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		OffScreenImage *p = (*i)->_prefetch;

		if (p != NULL) {
			delete p;
		}
	}
}

void ComboMenuComponent::SetComboMenu(ComboMenu *menu)
{
	_menu = menu;
}

int ComboMenuComponent::GetItemsSize()
{
	return _items.size();
}

void ComboMenuComponent::SetGap(int hgap, int vgap)
{
	_vertical_gap = vgap;
	_horizontal_gap = hgap;

	Repaint();
}

void ComboMenuComponent::SetLoop(bool loop)
{
	if (_loop == loop) {
		return;
	}

	_loop = loop;

	Repaint();
}

uint32_t ComboMenuComponent::GetItemColor()
{
	return (_item_alpha & 0xff) << 24 | (_item_red & 0xff) << 16 | (_item_green & 0xff) << 8 | (_item_blue & 0xff) << 0;
}

void ComboMenuComponent::SetItemColor(uint32_t color)
{
	_item_red = (color>>0x10)&0xff;
	_item_green =(color>>0x08)&0xff; 
	_item_blue = (color>>0x00)&0xff;
	_item_alpha = (color>>0x18)&0xff;
}

void ComboMenuComponent::SetItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_red = red;
	_item_green = green;
	_item_blue = blue;
	_item_alpha = alpha;
}

void ComboMenuComponent::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int i,
		count = 0,
		default_y = 0,
		scroll_gap = 5,
		scroll_width = 30,
		space = _horizontal_gap,
		font_height = DEFAULT_COMPONENT_HEIGHT;

	if (_font != NULL) {
		font_height = _font->GetHeight();
	}

	g->SetFont(_font);

	int position = _index-_visible_items/2;

	if (position > (int)(_items.size()-_visible_items)) {
		position = (_items.size()-_visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	default_y = (_item_size+_vertical_gap)*count+_vertical_gap;

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
	
			if ((int)_items.size() > _visible_items) {
				g->FillRectangle(0, (font_height+_vertical_gap)*count, _width-scroll_width-scroll_gap, font_height+10);
			} else {
				g->FillRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10);
			}
		} else {
			if ((int)_items.size() > _visible_items) {
				g->SetColor(_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				FillRectangle(g, 0, (font_height+_vertical_gap)*count, _width-scroll_width-scroll_gap, font_height+10);

				/*
				g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _width-scroll_width-scroll_gap, font_height+10, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _width-scroll_width-scroll_gap, font_height+10, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
				*/
			} else {
				g->SetColor(_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				FillRectangle(g, 0, (font_height+_vertical_gap)*count, _width, font_height+10);

				/*
				g->FillGradientRectangle(, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
				*/
			}
		}

		if (_items[i]->GetType() == EMPTY_MENU_ITEM) {
			// TODO::
		} else if (_items[i]->GetType() == TEXT_MENU_ITEM) {
			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
			g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-scroll_width-scroll_gap-space), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
		} else if (_items[i]->GetType() == IMAGE_MENU_ITEM) {
			if (_items[i]->_prefetch == NULL) {
				g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
				g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-scroll_width-scroll_gap-space), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
			} else {
				g->DrawImage(_items[i]->_prefetch, 10, (font_height+_vertical_gap)*count, font_height, font_height+10);
				g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
				g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-scroll_width-scroll_gap-space), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
			}
		} else if (_items[i]->GetType() == CHECK_MENU_ITEM) {
			if (_items[i]->IsSelected() == true) {
				g->DrawImage(prefetch, 10, 5+(font_height+_vertical_gap)*count, font_height, font_height);
			}

			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
			g->DrawStringJustified(TruncateString(_items[i]->GetValue(), _width-scroll_width-scroll_gap-space), space, (font_height+_vertical_gap)*count+5, _width-2*space, font_height, LEFT_ALIGN);
		}

		if (_items[i]->GetEnabled() == false) {
			g->SetDrawingFlags(BLEND_FLAG);
			g->SetColor(0x00, 0x00, 0x00, 0x80);
			
			if ((int)_items.size() > _visible_items) {
				g->FillRectangle(0, (font_height+_vertical_gap)*count, _width-scroll_width-scroll_gap, font_height+10);
			} else {
				g->FillRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10);
			}

			g->SetDrawingFlags(NOFX_FLAG);
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

	// scroll bar
	if ((int)_items.size() > _visible_items) {
		g->SetColor(_item_red, _item_green, _item_blue, _item_alpha);
		g->FillRectangle(_width-scroll_width, 0, scroll_width, _height);
				
		int dx = _width-scroll_width+2,
			dy = 4;
		
		scroll_width -= 4;

		if (_index > 0) {
			g->SetColor(0x00, 0x40, 0xf0, 0xff);
		} else {
			g->SetColor(0x80, 0x80, 0xe0, 0xff);
		}

		g->SetColor(0x80, 0x80, 0xe0, 0xff);
		g->FillTriangle(dx+0, dy+_item_size/2, dx+scroll_width/2, dy+2, dx+scroll_width, dy+_item_size/2);

		if (_index < (int)(_items.size()-1)) {
			g->SetColor(0x00, 0x40, 0xf0, 0xff);
		} else {
			g->SetColor(0x80, 0x80, 0xe0, 0xff);
		}

		dy = _height-scroll_gap-_item_size/2-4;

		g->SetColor(0x80, 0x80, 0xe0, 0xff);
		g->FillTriangle(dx+0, dy, dx+scroll_width, dy, dx+scroll_width/2, dy+_item_size/2-2);
		
		if (_visible_items <= (int)_items.size()) {
			double diff = (_height-2*_item_size-8)/(double)(_items.size()-1);
			
			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillRectangle(dx+2, (int)(_item_size/2+diff*_index+4), scroll_width-4, _item_size);
		}
	}

	for (; count<_visible_items; count++) {
		g->SetColor(_item_red, _item_green, _item_blue, _item_alpha);
		g->FillRectangle(0, (font_height+_vertical_gap)*count, _width, font_height+10);
	}
}

void ComboMenuComponent::SetCurrentIndex(int i)
{
	if (i < 0) {
		i = 0;
	}

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
}

void ComboMenuComponent::AddEmptyItem()
{
	MenuItem *item = new MenuItem();

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void ComboMenuComponent::AddItem(std::string text)
{
	MenuItem *item = new MenuItem(text);

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void ComboMenuComponent::AddItem(std::string text, std::string image)
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

void ComboMenuComponent::AddItem(std::string text, bool checked)
{
	MenuItem *item = new MenuItem(text, checked);

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

void ComboMenuComponent::AddMenuItem(MenuItem *item)
{
	if (item == NULL) {
		return;
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(item);
	}
}

MenuItem * ComboMenuComponent::GetMenuItem(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return NULL;
	}

	if (_items.size() > 0) {
		return _items[i];
	}

	return NULL;
}

MenuItem * ComboMenuComponent::GetCurrentMenuItem()
{
	if (_items.size() > 0) {
		return _items[_index];
	} 

	return NULL;
}

int ComboMenuComponent::GetCurrentIndex()
{
	return _index;
}

void ComboMenuComponent::RemoveItem(int index)
{
	// remove

	Repaint();
}

void ComboMenuComponent::RemoveAll()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_items.clear();
	}

	Repaint();
}

bool ComboMenuComponent::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_UP) {
		_index--;

		if (_index < 0) {
			if (_loop == false) {
				_index = 0;
			} else {
				_index = (int)(_items.size()-1);
			}
		}

		Repaint();

		catched = true;
	} else if (action == JKEY_CURSOR_DOWN) {
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

		Repaint();

		catched = true;
	} else if (action == JKEY_ENTER) {
		if (_items.size() > 0) {
			if (_items[_index]->GetEnabled() == true) {
				catched = true;
			}
		}
	}

	return catched;
}

ComboBox::ComboBox(int x, int y, int width, int height, int visible_items):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ComboBox");

	_menu = new ComboMenu(_x, _y+_height, _width, visible_items);

	_menu->SetLoop(false);
	_menu->SetCurrentIndex(0);

	_menu->RegisterMenuListener(this);

	SetFocusable(true);
	SetArrowSize(30);
}

ComboBox::~ComboBox()
{
	jthread::AutoLock lock(&_component_mutex);

	// WaitThread();
}

void ComboBox::SetArrowSize(int size)
{
	if (size <= 0) {
		return;
	}

	_arrow_size = size;

	if (_arrow_size > _width/2) {
		_arrow_size = _width/2;
	}

	Repaint();
}

int ComboBox::GetArrowSize()
{
	return _arrow_size;
}

void ComboBox::SetVisibleItems(int max_items)
{
	_menu->GetVisibleItems();
}

void ComboBox::SetLoop(bool b)
{
	_menu->SetLoop(b);
}

bool ComboBox::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	if (event->GetType() == JMOUSE_PRESSED_EVENT) {
		catched = true;

		RequestFocus();

		int x1 = event->GetX(),
			y1 = event->GetY(),
			dx = 10;

		if (x1 > (_x+_width-_arrow_size-dx) && x1 < (_x+_width-dx) && y1 > _y && y1 < (_y+_height)) {
			if (_parent != NULL) {
				_menu->SetPosition(_parent->GetX()+_x, _parent->GetY()+_y+_height+5);
				_menu->Show();
			}
		}
	
		Repaint();
	}

	return catched;
}

bool ComboBox::ProcessEvent(KeyEvent *event)
{
	if (event->GetType() != JKEY_PRESSED) {
		return true;
	}

	jkey_symbol_t action = event->GetSymbol();

	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	if (action == JKEY_ENTER) {
		if (_parent != NULL) {
			_menu->SetPosition(_parent->GetX()+_x, _parent->GetY()+_y+_height+5);
			_menu->Show();
		}
	
		Repaint();

		catched = true;
	}

	return catched;
}

void ComboBox::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	g->SetFont(_font);

	{
		/*
		if (_has_focus == true) {
				g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
		}
		*/

		int dx = _width-_arrow_size-10,
			dy = _height/3;

		g->SetColor(0x80, 0x80, 0xe0, 0xff);
		g->FillTriangle(dx+0, dy+0, dx+_arrow_size, dy+0, dx+_arrow_size/2, dy+_arrow_size/2);

		g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
		g->DrawStringJustified(TruncateString(GetValue(), dx), 5, (CENTER_VERTICAL_TEXT), dx, _height, CENTER_ALIGN);
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _width, _height);
	}
}

void ComboBox::SetIndex(int i)
{
	jthread::AutoLock lock(&_component_mutex);

	_menu->SetCurrentIndex(i);
}

void ComboBox::Add(std::string text)
{
	jthread::AutoLock lock(&_component_mutex);

	_menu->AddMenuItem(new MenuItem(text));
}

void ComboBox::Remove(int index)
{
	jthread::AutoLock lock(&_component_mutex);

	_menu->RemoveItem(index);
}

void ComboBox::RemoveAll()
{
	jthread::AutoLock lock(&_component_mutex);

	_menu->RemoveAll();
}

std::string ComboBox::GetValue()
{
	jthread::AutoLock lock(&_component_mutex);

	MenuItem *item = _menu->GetCurrentItem();

	if ((void *)item == NULL) {
		return "";
	}

	return item->GetValue();
}

int ComboBox::GetIndex()
{
	jthread::AutoLock lock(&_component_mutex);

	MenuItem *item = _menu->GetCurrentItem();

	if (item == NULL) {
		return 0;
	}

	return _menu->GetCurrentIndex();
}

void ComboBox::ItemSelected(MenuEvent *event)
{
}

void ComboBox::ItemChanged(MenuEvent *event)
{
	MenuItem *item = NULL;

	{
		jthread::AutoLock lock(&_component_mutex);

		item = event->GetMenuItem();
	}

	if (item != NULL) {
		Repaint();
	}
}

void ComboBox::RegisterSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	_select_listeners.push_back(listener);
}

void ComboBox::RemoveSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	for (std::vector<SelectListener *>::iterator i=_select_listeners.begin(); i!=_select_listeners.end(); i++) {
		if ((*i) == listener) {
			_select_listeners.erase(i);

			break;
		}
	}
}

void ComboBox::DispatchEvent(SelectEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<SelectListener *>::iterator i=_select_listeners.begin(); i!=_select_listeners.end(); i++) {
		if (event->GetType() == ACTION_ITEM) {
			(*i)->ItemSelected(event);
		} else {
			(*i)->ItemChanged(event);
		}
	}

	delete event;
}

std::vector<SelectListener *> & ComboBox::GetSelectListeners()
{
	return _select_listeners;
}

void ComboBox::Run()
{
	Repaint();
}

}
