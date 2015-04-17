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
#include "jthememanager.h"

namespace jgui {

Menu::Menu(int x, int y, int width, int visible_items):
	jgui::Frame(x, y, 0, 0),
	jgui::ItemComponent()
{
	jcommon::Object::SetClassName("jgui::Menu");

	_top_index = 0;
	_menu_align = JMA_ITEM;
	_visible_items = visible_items;
	_item_size = DEFAULT_ITEM_SIZE;

	if (_visible_items < 1) {
		_visible_items = 1;
	}

	SetSize(width, _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size));

	_check = Image::CreateImage(_DATA_PREFIX"/images/check.png");

	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update(this);
}

Menu::~Menu() 
{
	jthread::AutoLock lock(&_menu_mutex);

	while (_menus.size() > 0) {
		Menu *menu = (*_menus.begin());

		_menus.erase(_menus.begin());

		menu->Release();

		delete menu;
	}

	if (_check != NULL) {
		delete _check;
		_check = NULL;
	}
}

bool Menu::KeyPressed(KeyEvent *event)
{
	/*
	if (Frame::KeyPressed(event) == true) {
		return true;
	}
	*/

	Menu *last = NULL;

	if (_menus.size() == 0) {
		last = this;
	} else {
		last = (*_menus.rbegin());
	}

	if (event->GetSymbol() == JKS_ESCAPE || event->GetSymbol() == JKS_EXIT) {
		Hide();

		while (_menus.size() > 0) {
			Menu *menu = *(_menus.begin()+_menus.size()-1);

			_menus.erase(_menus.begin()+_menus.size()-1);

			menu->Release();

			delete menu;
		}

		Release();
	} else if (event->GetSymbol() == jgui::JKS_CURSOR_UP || event->GetSymbol() == jgui::JKS_CURSOR_DOWN) {
		Menu *menu = last;

		jkeyevent_symbol_t action = event->GetSymbol();

		if (action == JKS_CURSOR_UP) {
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

				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_UP)); 
			}
		} else if (action == JKS_CURSOR_DOWN) {
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

				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_DOWN)); 
			}
		} else if (action == JKS_ENTER) {
			if (menu->_items[menu->_index]->IsEnabled() == true) {
				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_ACTION)); 
			}
		}
	} else if (event->GetSymbol() == jgui::JKS_CURSOR_LEFT) {
		if (last != this) {
			_menus.erase(_menus.begin()+_menus.size()-1);

			last->Release();

			delete last;
		}

		DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_LEFT));
	} else if (event->GetSymbol() == jgui::JKS_CURSOR_RIGHT || event->GetSymbol() == jgui::JKS_ENTER) {
		Item *item = GetCurrentItem();

		if (item != NULL && item->IsEnabled() == true) {
			if (event->GetSymbol() == jgui::JKS_ENTER && item->GetType() == JIT_CHECK) {
				item->SetSelected(item->IsSelected()^true);

				last->Repaint();

				DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_ACTION)); 
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
					
					if (_menu_align == JMA_TITLE) {
						menu = new Menu(last->GetX()+last->GetWidth()+5, last->GetY(), last->GetWidth(), items.size());	
					} else if (_menu_align == JMA_ITEM) {
						jinsets_t insets = last->GetInsets();
						int x = last->GetX()+last->GetWidth()+5,
								y = last->GetY()+position*((last->GetHeight()-_vertical_gap-_border_size)/last->GetVisibleItems());

						if (_title != "" && _menus.size() == 0) {
							y = last->GetY()+position*((last->GetHeight()-insets.top-_vertical_gap-_border_size)/last->GetVisibleItems())+insets.top;
						}

						menu = new Menu(x, y, last->GetWidth(), items.size());
					}

					InputManager::GetInstance()->RemoveKeyListener(menu);
					InputManager::GetInstance()->RemoveMouseListener(menu);

					for (std::vector<Item *>::iterator i=items.begin(); i!=items.end(); i++) {
						if ((*i)->IsVisible() == true) {
							menu->AddItem((*i));
						}
					}

					_menus.push_back(menu);

					menu->SetInputEnabled(false);
					menu->Show(false);

					DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_RIGHT));
				} else {
					if (event->GetSymbol() == jgui::JKS_ENTER) {
						Item *item = GetCurrentItem();
						int index = GetCurrentIndex();

						Hide();

						while (_menus.size() > 0) {
							Menu *menu = *(_menus.begin()+_menus.size()-1);

							_menus.erase(_menus.begin()+_menus.size()-1);

							menu->Release();

							delete menu;
						}
						
						DispatchSelectEvent(new SelectEvent(this, item, index, JSET_ACTION)); 

						Release();
					}
				}
			}
		}
	}

	return true;
}

bool Menu::MouseWheel(MouseEvent *event)
{
	int count = event->GetClickCount();

	Menu *menu = this;

	if (_menus.size() > 0) {
		menu = (*_menus.rbegin());
	}

	if (count < 0) {
		int old_index = menu->_index;

		menu->_index = menu->_index + count;

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

			DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_UP)); 
		}
	} else {
		int old_index = menu->_index;

		menu->_index = menu->_index + count;

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

			DispatchSelectEvent(new SelectEvent(GetCurrentMenu(), GetCurrentItem(), GetCurrentIndex(), JSET_DOWN)); 
		}
	}

	return true;
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

	int w = _size.width,
			h = _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size);

	if (_title != "") {
		h = h + GetInsets().top-_vertical_gap;
	}

	SetSize(w, h);
}

int Menu::GetVisibleItems()
{
	return _visible_items;
}

void Menu::SetItemColor(int red, int green, int blue, int alpha)
{
	SetItemColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetItemColor(red, green, blue, alpha);
	}
}

void Menu::SetItemForegroundColor(int red, int green, int blue, int alpha)
{
	SetItemForegroundColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetItemForegroundColor(red, green, blue, alpha);
	}
}

void Menu::SetBackgroundColor(int red, int green, int blue, int alpha)
{
	SetBackgroundColor(red, green, blue, alpha);

	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetBackgroundColor(red, green, blue, alpha);
	}
}

void Menu::SetForegroundColor(int red, int green, int blue, int alpha)
{
	SetForegroundColor(red, green, blue, alpha);
	
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
	jgui::Graphics *g = GetGraphics();

	if (g != NULL) {
		Paint(g);
	}
}

void Menu::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Frame::Paint(g);

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	x = (x < 0)?0:x;
	y = (y < 0)?0:y;
	w = (w < 0)?0:w;
	h = (h < 0)?0:h;

	if (_title != "") {
		jinsets_t insets = GetInsets();

		g->SetGradientStop(0.0, _bgcolor);
		g->SetGradientStop(1.0, _scrollbar_color);
		g->FillLinearGradient(_border_size, _border_size, _size.width-2*_border_size, insets.top-2*_border_size, 0, 0, _size.width-2*_border_size, insets.top-2*_border_size);
		g->ResetGradientStop();

		if (IsFontSet() == true) {
			std::string text = _title;
			
			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", w);
			// }

			g->SetColor(_fgcolor);
			g->DrawString(text, x+(w-_font->GetStringWidth(text))/2, (insets.top-_font->GetLineSize()+_font->GetLeading())/2);
		}
		
		y = y + insets.top - 2*_vertical_gap;
	}

	int count = 0,
			space = 0,
			position = _top_index;

	if (position > (int)(_items.size()-_visible_items)) {
		position = (_items.size()-_visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == JIT_IMAGE || (*i)->GetType() == JIT_CHECK) {
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

			if (_items[i]->GetType() == JIT_EMPTY) {
			} else if (_items[i]->GetType() == JIT_TEXT) {
			} else if (_items[i]->GetType() == JIT_IMAGE) {
				if (_items[i]->GetImage() == NULL) {
				} else {
					g->DrawImage(_items[i]->GetImage(), x, y+(_item_size+_vertical_gap)*count+2, _item_size, _item_size-4);
				}
			} else if (_items[i]->GetType() == JIT_CHECK) {
				if (_items[i]->IsSelected() == true) {
					g->DrawImage(_check, x, y+(_item_size+_vertical_gap)*count+2, _item_size, _item_size-4);
				}
			}

			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", pw);
			// }

			g->DrawString(text, px, py, pw, ph, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
		}

		if (_items[i]->IsEnabled() == false) {
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

void Menu::SetCurrentIndex(int i)
{
	if (i < 0) {
		i = 0;
	}

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
