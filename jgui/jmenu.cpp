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

#include <algorithm>

namespace jgui {

Menu::Menu(int x, int y, int width, int visible_items):
 	jgui::Frame("", x, y, width, 1),
	jgui::FrameInputListener()
{
	jcommon::Object::SetClassName("jgui::Menu");

	_menu_align = SUBMENU_ALIGN;
	_visible_items = visible_items;
	_centered_interaction = true;

	if (_visible_items <= 0) {
		_visible_items = 1;
	}

	_item_size = DEFAULT_COMPONENT_HEIGHT;

	prefetch = new OffScreenImage(_item_size, _item_size);
	
	if (prefetch->GetGraphics() != NULL) {
		prefetch->GetGraphics()->DrawImage("./icons/check.png", 0, 0, _item_size, _item_size);
	}

	SetUndecorated(true);
	SetDefaultExitEnabled(false);
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
	SetSize(width, _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size)-_vertical_gap);

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

	if (prefetch != NULL) {
		delete prefetch;
		prefetch = NULL;
	}
	
	Release();
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
		SetSize(_size.width, _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size)-_vertical_gap);
	} else {
		SetSize(_size.width, _visible_items*(_item_size+_vertical_gap)+2*(_vertical_gap+_border_size)-_vertical_gap+_insets.top);
	}
}

int Menu::GetVisibleItems()
{
	return _visible_items;
}

void Menu::SetItemColor(jcolor_t color)
{
	SetItemColor(color.red, color.green, color.blue, color.alpha);
}

void Menu::SetBackgroundColor(jcolor_t color)
{
	SetBackgroundColor(color.red, color.green, color.blue, color.alpha);
}

void Menu::SetForegroundColor(jcolor_t color)
{
	SetForegroundColor(color.red, color.green, color.blue, color.alpha);
}

void Menu::SetItemColor(int red, int green, int blue, int alpha)
{
	ItemComponent::SetItemColor(red, green, blue, alpha);
	
	for (std::vector<Menu *>::iterator i=_menus.begin(); i!=_menus.end(); i++) {
		(*i)->SetItemColor(red, green, blue, alpha);
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

void Menu::Paint(Graphics *g)
{
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
		y = y + _insets.top;
		
		g->SetColor(0xf0, 0xf0, 0xf0, 0x80);
		g->FillRectangle(_insets.left, _insets.top-10, _size.width-_insets.left-_insets.right, 5);

		if (IsFontSet() == true) {
			std::string text = _title;
			
			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", (_size.width-_insets.left-_insets.right));
			// }

			g->SetColor(_fg_color);
		
			g->SetClip(0, 0, _size.width, _insets.top);
			g->DrawString(text, _insets.left+(_size.width-_insets.left-_insets.right-_font->GetStringWidth(text))/2, _insets.top-_font->GetHeight()-15);
			g->SetClip(0, 0, _size.width, _size.height);
		}
	}

	int i,
		count = 0,
		space = 15;

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

	for (std::vector<Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == IMAGE_MENU_ITEM || (*i)->GetType() == CHECK_MENU_ITEM) {
			space = 20+_item_size;

			break;
		}
	}

	if ((int)_items.size() < _visible_items) {
		position = 0;
	}

	for (i=position; count<_visible_items && i<(int)_items.size(); i++, count++) {
		if (_index != i) {
			g->SetColor(_item_color);
			g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w, _item_size);
		} else {
			g->SetColor(_bgfocus_color);
			FillRectangle(g, x, y+(_item_size+_vertical_gap)*count, w, _item_size);

			/*
			g->FillGradientRectangle(0, (_item_size+_vertical_gap)*count, _width, _item_size+10, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			g->FillGradientRectangle(0, (_item_size+_vertical_gap)*count, _width, _item_size+10, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
			*/
		}

		if (IsFontSet() == true) {
			std::string text = _items[i]->GetValue();
			int px = space,
					py = (_item_size+_vertical_gap)*count,
					pw = _size.width-2*space,
					ph = _item_size;

			g->SetColor(_fg_color);

			if (_items[i]->GetType() == EMPTY_MENU_ITEM) {
				// TODO::
			} else if (_items[i]->GetType() == TEXT_MENU_ITEM) {
				// pw = _size.width-2*space;
			} else if (_items[i]->GetType() == IMAGE_MENU_ITEM) {
				if (_items[i]->GetImage() == NULL) {
					// w = _size.width-2*space;
				} else {
					g->DrawImage(_items[i]->GetImage(), x+10, y+(_item_size+_vertical_gap)*count+2, _item_size, _item_size-4);

					pw = pw-_item_size-10;
				}
			} else if (_items[i]->GetType() == CHECK_MENU_ITEM) {
				if (_items[i]->IsSelected() == true) {
					g->DrawImage(prefetch, x+10, y+(_item_size+_vertical_gap)*count+2, _item_size, _item_size-4);
				}

				pw = pw-_item_size-10;
			}

			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", pw);
			// }

			// TODO:: g->SetClip(px, py, pw, ph);
			g->DrawString(text, x+px, y+py, pw, ph, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
			// g->SetClip(0, 0, _size.width, _size.height);
		}

		if (_items[i]->GetEnabled() == false) {
			g->SetColor(0x00, 0x00, 0x00, 0x80);
			FillRectangle(g, x, y+(_item_size+_vertical_gap)*count, w, _item_size+10);
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
		FillRectangle(g, x, y+(_item_size+_vertical_gap)*count, _size.width, _item_size+10);
	}
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

				DispatchSelectEvent(new SelectEvent(this, menu->_items[menu->_index], menu->_index, UP_ITEM)); 
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

				DispatchSelectEvent(new SelectEvent(this, menu->_items[_index], _index, DOWN_ITEM)); 
			}
		} else if (action == JKEY_ENTER) {
			if (menu->_items[menu->_index]->GetEnabled() == true) {
				DispatchSelectEvent(new SelectEvent(this, menu->_items[menu->_index], menu->_index, ACTION_ITEM)); 
			}
		}
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_LEFT) {
		if (last != this) {
			_menus.erase(_menus.begin()+_menus.size()-1);

			last->Release();

			delete last;
		}

		DispatchSelectEvent(new SelectEvent(this, GetCurrentItem(), GetCurrentIndex(), LEFT_ITEM));
	} else if (event->GetSymbol() == jgui::JKEY_CURSOR_RIGHT || event->GetSymbol() == jgui::JKEY_ENTER) {
		Item *item = GetCurrentItem();

		if (item != NULL && item->GetEnabled() == true) {
			if (event->GetSymbol() == jgui::JKEY_ENTER && item->GetType() == jgui::CHECK_MENU_ITEM) {
				bool b = true;

				if (item->IsSelected() == true) {
					b = false;
				}

				item->SetSelected(b);

				last->Repaint();

				DispatchSelectEvent(new SelectEvent(this, item, GetCurrentIndex(), ACTION_ITEM));
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
						int x = last->GetX()+last->GetWidth()+5,
								y = last->GetY()+position*((last->GetHeight()-_vertical_gap-_border_size)/last->GetVisibleItems());

						if (_title != "" && _menus.size() == 0) {
							y = last->GetY()+position*((last->GetHeight()-_insets.top-_vertical_gap-_border_size)/last->GetVisibleItems())+_insets.top;
						}

						menu = new Menu(x, y, last->GetWidth(), items.size());	
					}

					menu->SetBackgroundColor(GetBackgroundColor());
					menu->SetForegroundColor(GetForegroundColor());
					menu->SetItemColor(GetItemColor());

					for (std::vector<Item *>::iterator i=items.begin(); i!=items.end(); i++) {
						if ((*i)->IsVisible() == true) {
							menu->AddItem((*i));
						}
					}

					_menus.push_back(menu);

					menu->SetInputEnabled(false);
					menu->Show(false);

					if (GetComponentInFocus() != NULL) {
						GetComponentInFocus()->ReleaseFocus();
					}
				
					DispatchSelectEvent(new SelectEvent(this, GetCurrentItem(), GetCurrentIndex(), RIGHT_ITEM));
				} else {
					if (event->GetSymbol() == jgui::JKEY_ENTER) {
						while (_menus.size() > 0) {
							Menu *m = *(_menus.begin()+_menus.size()-1);

							_menus.erase(_menus.begin()+_menus.size()-1);

							m->Release();

							delete m;
						}

						Hide();
						
						DispatchSelectEvent(new SelectEvent(NULL, item, GetCurrentIndex(), ACTION_ITEM));

						_frame_sem.Notify();
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

/*
void Menu::InputChanged(KeyEvent *event)
{
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

			// DispatchSelectEvent(new SelectEvent(this, _items[_index].text, _index, UP_ITEM)); 
		}
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

			// DispatchSelectEvent(new SelectEvent(this, _items[_index].text, _index, DOWN_ITEM)); 
		}
	} else if (action == JKEY_ENTER) {
		if (_items[_index]->GetEnabled() == true) {
		}
	}
}
*/

}
