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
#include "Stdafx.h"
#include "jmenu.h"
#include "jselectlistener.h"

namespace jgui {

Menu::Menu(int x, int y, int width, int visible_items):
	jgui::ItemComponent(0, 0, 0, 0)
{
	jcommon::Object::SetClassName("jgui::Menu");

	_menu_align = SUBMENU_ALIGN;
	_visible_items = visible_items;
	_centered_interaction = true;
	_item_size = DEFAULT_ITEM_SIZE;

	if (_visible_items < 1) {
		_visible_items = 1;
	}

	_frame = new Frame("", x, y, width, _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size));

	_frame->SetUndecorated(true);
	_frame->SetDefaultExitEnabled(false);

	_frame->RegisterInputListener(this);
	
	prefetch = Image::CreateImage(_DATA_PREFIX"/images/check.png");
	
	ThemeManager::GetInstance()->RegisterThemeListener(this);
}

Menu::~Menu() 
{
	ThemeManager::GetInstance()->RemoveThemeListener(this);

	_frame->RemoveInputListener(this);

	jthread::AutoLock lock(&_menu_mutex);

	while (_menus.size() > 0) {
		Menu *menu = (*_menus.begin());

		_menus.erase(_menus.begin());

		menu->Release();

		delete menu;
	}

	if (prefetch != NULL) {
		delete prefetch;
		prefetch = NULL;
	}
	
	_frame->Release();

	delete _frame;
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

void Menu::SetBounds(int x, int y, int w, int h)
{
	_frame->SetBounds(x, y, w, h);
}

void Menu::SetLocation(int x, int y)
{
	_frame->SetLocation(x, y);
}

void Menu::SetSize(int w, int h)
{
	_frame->SetSize(w, h);
}

void Menu::Move(int x, int y)
{
	_frame->Move(x, y);
}

jpoint_t Menu::GetLocation()
{
	return _frame->GetLocation();
}

jsize_t Menu::GetSize()
{
	return _frame->GetSize();
}

int Menu::GetX()
{
	return _frame->GetX();
}

int Menu::GetY()
{
	return _frame->GetY();
}

int Menu::GetWidth()
{
	return _frame->GetWidth();
}

int Menu::GetHeight()
{
	return _frame->GetHeight();
}

bool Menu::Show(bool modal)
{
	if (_frame->Show(false) == false) {
		return false;
	}

	Repaint();

	if (modal == true) {
		_menu_sem.Wait();
	}

	return true;
}

bool Menu::Hide()
{
	return _frame->Hide();
}

void Menu::Release()
{
	_menu_sem.Notify();

	_frame->Release();
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

	_size = _frame->GetSize();
	_location = _frame->GetLocation();
	_horizontal_gap = _frame->GetHorizontalGap();
	_vertical_gap = _frame->GetVerticalGap();
	_border_size = _frame->GetBorderSize();

	int w = _size.width,
			h = _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size);

	if (_title != "") {
		h = h + _frame->GetInsets().top-_vertical_gap;
	}

	_frame->SetSize(w, h);
}

int Menu::GetVisibleItems()
{
	return _visible_items;
}

void Menu::SetItemColor(int red, int green, int blue, int alpha)
{
	ItemComponent::SetItemColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetItemColor(red, green, blue, alpha);
	}
}

void Menu::SetItemForegroundColor(int red, int green, int blue, int alpha)
{
	ItemComponent::SetItemForegroundColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetItemForegroundColor(red, green, blue, alpha);
	}
}

void Menu::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	ItemComponent::SetBackgroundColor(red, green, blue, alpha);

	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetBackgroundColor(red, green, blue, alpha);
	}
}

void Menu::SetForegroundColor(int red, int green, int blue, int alpha)
{
	ItemComponent::SetForegroundColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetForegroundColor(red, green, blue, alpha);
	}
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

Item * Menu::GetCurrentItem()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		if (GetItemsSize() > 0) {
			return _items[_index];
		}
	} else {
		Menu *menu = (*_menus.rbegin());

		if (GetItemsSize() > 0) {
			return menu->GetCurrentItem();
		}
	}

	return NULL;
}

int Menu::GetCurrentIndex()
{
	jthread::AutoLock lock(&_menu_mutex);

	if (_menus.size() == 0) {
		return _index;
	} else {
		Menu *menu = (*_menus.rbegin());

		return menu->GetCurrentIndex();
	}

	return 0;
}

void Menu::Repaint()
{
	if (_ignore_repaint == true) {
		return;
	}

	if (_frame->GetGraphics() != NULL) {
		Paint(_frame->GetGraphics());
	}
}

void Menu::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	// Component::Paint(g);

	_size = _frame->GetSize();
	_location = _frame->GetLocation();
	_horizontal_gap = _frame->GetHorizontalGap();
	_vertical_gap = _frame->GetVerticalGap();
	_border_size = _frame->GetBorderSize();

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	x = (x < 0)?0:x;
	y = (y < 0)?0:y;
	w = (w < 0)?0:w;
	h = (h < 0)?0:h;

	if (_title != "") {
		jinsets_t insets = _frame->GetInsets();

		g->SetColor(0xf0, 0xf0, 0xf0, 0x80);
		g->FillRectangle(x, insets.top, w, 5);

		if (IsFontSet() == true) {
			std::string text = _title;
			
			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", w);
			// }

			g->SetColor(_fgcolor);
			g->DrawString(text, x+(w-_font->GetStringWidth(text))/2, y+(insets.top-_font->GetHeight())/2);
		}
		
		y = y + insets.top;
	}

	int count = 0,
			space = 0,
			position = _top_index;

	if (_centered_interaction == true) {
		position = _index-_visible_items/2;
	}

	if (position > (int)(_items.size()-_visible_items)) {
		position = (_items.size()-_visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == IMAGE_MENU_ITEM || (*i)->GetType() == CHECK_MENU_ITEM) {
			space = _item_size+10;

			break;
		}
	}

	if ((int)_items.size() < _visible_items) {
		position = 0;
	}

	for (int i=position; count<_visible_items && i<(int)_items.size(); i++, count++) {
		if (_index != i) {
			g->SetColor(_item_color);
			g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w, _item_size);
		} else {
			g->SetColor(_focus_item_color);
			g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w, _item_size);

			/*
			g->FillGradientRectangle(0, (_item_size+_vertical_gap)*count, _width, _item_size+10, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			g->FillGradientRectangle(0, (_item_size+_vertical_gap)*count, _width, _item_size+10, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
			*/
		}

		if (IsFontSet() == true) {
			std::string text = _items[i]->GetValue();
			int px = x+space,
					py = y+(_item_size+_vertical_gap)*count,
					pw = w-space,
					ph = _item_size;

			if (_has_focus == true) {
				g->SetColor(_focus_item_fgcolor);
			} else {
				g->SetColor(_item_fgcolor);
			}

			if (_items[i]->GetType() == EMPTY_MENU_ITEM) {
			} else if (_items[i]->GetType() == TEXT_MENU_ITEM) {
			} else if (_items[i]->GetType() == IMAGE_MENU_ITEM) {
				if (_items[i]->GetImage() == NULL) {
				} else {
					g->DrawImage(_items[i]->GetImage(), x, y+(_item_size+_vertical_gap)*count+2, _item_size, _item_size-4);
				}
			} else if (_items[i]->GetType() == CHECK_MENU_ITEM) {
				if (_items[i]->IsSelected() == true) {
					g->DrawImage(prefetch, x, y+(_item_size+_vertical_gap)*count+2, _item_size, _item_size-4);
				}
			}

			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", pw);
			// }

			g->DrawString(text, px, py, pw, ph, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
		}

		if (_items[i]->GetEnabled() == false) {
			g->SetColor(0x00, 0x00, 0x00, 0x80);
			g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w, _item_size+10);
		}

		if (_items[i]->GetChildsSize() > 0) {
			int dx = x+w-_item_size/2-4,
					dy = y+(_item_size+_vertical_gap)*count;

			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillTriangle(dx, dy+2, dx+_item_size/2, dy+_item_size/2, dx, dy+_item_size-4);
		}
	}

	for (; count<_visible_items; count++) {
		g->SetColor(_item_color);
		g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, _size.width, _item_size+10);
	}

	g->Flip();
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
		Menu *menu = last;

		jkey_symbol_t action = event->GetSymbol();

		if (action == JKEY_CURSOR_UP) {
			int old_index = menu->_index;

			menu->_index--;

			if (menu->_index < 0) {
				if (menu->_loop == false) {
					menu->_index = 0;
				} else {
					menu->_index = (int)(menu->_items.size()-1);
				}
			}

			if (menu->_index < menu->_top_index) {
				menu->_top_index = menu->_index;
			}

			if (menu->_index != old_index) {
				menu->Repaint();

				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), UP_ITEM)); 
			}
		} else if (action == JKEY_CURSOR_DOWN) {
			int old_index = menu->_index;

			menu->_index++;

			if (menu->_index >= (int)menu->_items.size()) {
				if (menu->_loop == false) {
					if (menu->_items.size() > 0) {
						menu->_index = menu->_items.size()-1;
					} else {
						menu->_index = 0;
					}
				} else {
					menu->_index = 0;
				}
			}

			if (menu->_index >= (menu->_top_index + menu->_visible_items)) {
				menu->_top_index = menu->_index-menu->_visible_items+1;

				if (menu->_top_index < 0) {
					menu->_top_index = 0;
				}
			}

			if (menu->_index != old_index) {
				menu->Repaint();

				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), DOWN_ITEM)); 
			}
		} else if (action == JKEY_ENTER) {
			if (menu->_items[menu->_index]->GetEnabled() == true) {
				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), ACTION_ITEM)); 
			}
		}
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
		if (last != this) {
			_menus.erase(_menus.begin()+_menus.size()-1);

			last->Release();

			delete last;
		}

		DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), LEFT_ITEM));
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT || event->GetSymbol() == jgui::JKEY_ENTER) {
		Item *item = GetCurrentItem();

		if (item != NULL && item->GetEnabled() == true) {
			if (event->GetSymbol() == jgui::JKEY_ENTER && item->GetType() == jgui::CHECK_MENU_ITEM) {
				item->SetSelected(item->IsSelected()^true);

				last->Repaint();

				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), ACTION_ITEM)); 
			} else {
				std::vector<Item *> items = item->GetChilds();

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
						jinsets_t insets = _frame->GetInsets();
						int x = last->GetX()+last->GetWidth()+5,
								y = last->GetY()+position*((last->GetHeight()-_vertical_gap-_border_size)/last->GetVisibleItems());

						if (_title != "" && _menus.size() == 0) {
							y = last->GetY()+position*((last->GetHeight()-insets.top-_vertical_gap-_border_size)/last->GetVisibleItems())+insets.top;
						}

						menu = new Menu(x, y, last->GetWidth(), items.size());
					}

					for (std::vector<Item *>::iterator i=items.begin(); i!=items.end(); i++) {
						if ((*i)->IsVisible() == true) {
							menu->AddItem((*i));
						}
					}

					_menus.push_back(menu);

					menu->_frame->SetInputEnabled(false);

					menu->Show(false);
					menu->Repaint();

					DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), RIGHT_ITEM));
				} else {
					if (event->GetSymbol() == jgui::JKEY_ENTER) {
						Item *item = GetCurrentItem();
						int index = GetCurrentIndex();

						Hide();

						while (_menus.size() > 0) {
							Menu *m = *(_menus.begin()+_menus.size()-1);

							_menus.erase(_menus.begin()+_menus.size()-1);

							m->Release();

							delete m;
						}
						
						DispatchSelectEvent(new SelectEvent(this, item, index, ACTION_ITEM)); 

						_menu_sem.Notify();
					}
				}
			}
		}
	}
}

void Menu::SetCenteredInteraction(bool b)
{
	_centered_interaction = b;
}

void Menu::SetCurrentIndex(int i)
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

void Menu::RegisterInputListener(FrameInputListener *listener)
{
	_frame->RegisterInputListener(listener);
}

void Menu::RemoveInputListener(FrameInputListener *listener)
{
	_frame->RemoveInputListener(listener);
}

std::vector<FrameInputListener *> & Menu::GetFrameInputListeners()
{
	return _frame->GetFrameInputListeners();
}

void Menu::ThemeChanged(ThemeEvent *event)
{
	Theme *theme = event->GetTheme();

	theme->Update(this);

	Repaint();
}

}
