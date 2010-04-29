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
#include "jlistbox.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

ListBox::ListBox(int x, int y, int width, int height):
  ItemComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ListBox");

	_centered_interaction = true;
	_top_index = 0;
	_selected_index = -1;
	_input_locked = false;
	_scroll = SCROLL_BAR;
	_selection = NONE_SELECTION;

	_item_size = DEFAULT_COMPONENT_HEIGHT;

	if (_font != NULL) {
		_item_size = _font->GetHeight();
	}

	SetFocusable(true);
}

ListBox::~ListBox() 
{
}

void ListBox::SetCenteredInteraction(bool b)
{
	_centered_interaction = b;
}

int ListBox::GetVisibleItems()
{
	int visible_items = (_size.height-2*(_vertical_gap+_border_size))/(_item_size+_vertical_gap);

	if (visible_items < 1) {
		visible_items = 1;
	}

	return visible_items;
}

void ListBox::SetSelectionType(jlist_selection_type_t type)
{
	if (_selection == type) {
		return;
	}

	_selection = type;
	_selected_index = -1;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		(*i)->SetSelected(false);
	}
}

int ListBox::GetItemSize()
{
	return _item_size;
}

void ListBox::SetItemSize(int size)
{
	if (size <= 0) {
		return;
	}

	_item_size = size;
}

void ListBox::SetScrollType(jlist_scroll_type_t type)
{
	jthread::AutoLock lock(&_component_mutex);

	_scroll = type;
}

void ListBox::SetForegroundColor(int red, int green, int blue, int alpha)
{
	SetItemForegroundColor(red, green, blue, alpha);
}

void ListBox::SetForegroundFocusColor(int red, int green, int blue, int alpha)
{
	SetItemForegroundFocusColor(red, green, blue, alpha);
}

void ListBox::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int position,
			visible_items = GetVisibleItems();

	if (_centered_interaction == true) {
		position = _index-visible_items/2;
	} else {
		position = _top_index;
	}

	if (position > (int)(_items.size()-visible_items)) {
		position = (_items.size()-visible_items);
	}

	if (position < 0) {
		position = 0;
	}

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	int i,
			count = 0,
			default_y = 0,
			space = 5,
			scroll_width = 0,
			scroll_gap = 0;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == IMAGE_MENU_ITEM) {
			space += _item_size + 10;

			break;
		}
	}

	if ((int)_items.size() < visible_items) {
		position = 0;
	}

	default_y = (_item_size+y)*count+y;

	if (_scroll == SCROLL_BAR) {
		scroll_width = 30;
		scroll_gap = 5;
	}

	for (i=position; count<visible_items && i<(int)_items.size(); i++, count++) {
		default_y = (_item_size+y)*count+y;

		if (_index != i) {
			g->SetColor(_item_color);

			if (_selection == SINGLE_SELECTION) {	
				if (_selected_index == i) {	
					g->SetColor(_selected_item_color);
				}
			} else if (_selection == MULTI_SELECTION) {	
				if (_items[i]->IsSelected() == true) {	
					g->SetColor(_selected_item_color);
				}
			}
		} else {
			g->SetColor(_focus_item_color);
		}

		FillRectangle(g, x, default_y, w-scroll_width-scroll_gap, _item_size);

		g->SetColor(_item_color);

		if (_selection == SINGLE_SELECTION) {
			if (_selected_index == i) {
				g->SetColor(_selected_item_color);
			}
		} else if (_selection == MULTI_SELECTION) {	
			if (_items[i]->IsSelected() == true) {	
				g->SetColor(_selected_item_color);
			}
		}

		if (_items[i]->GetType() == EMPTY_MENU_ITEM) {
		} else if (_items[i]->GetType() == TEXT_MENU_ITEM) {
		} else if (_items[i]->GetType() == IMAGE_MENU_ITEM) {
			if (_items[i]->GetImage() != NULL) {
				g->DrawImage(_items[i]->GetImage(), 10, default_y, _item_size, _item_size);
			}
		}

		if (IsFontSet() == true) {
			if (_has_focus == true) {
				g->SetColor(_focus_item_fgcolor);
			} else {
				g->SetColor(_item_fgcolor);
			}

			int gapx = space;

			int px = x+gapx,
					py = default_y,
					pw = w-gapx-scroll_width-scroll_gap,
					ph = _item_size;

			x = (x < 0)?0:x;
			y = (y < 0)?0:y;
			w = (w < 0)?0:w;
			h = (h < 0)?0:h;

			px = (px < 0)?0:px;
			py = (py < 0)?0:py;
			pw = (pw < 0)?0:pw;
			ph = (ph < 0)?0:ph;

			std::string text = _items[i]->GetValue();

			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", w);
			// }

			g->SetClip(0, 0, x+w, y+h);
			g->DrawString(text, px, py, pw, ph, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
			g->SetClip(0, 0, _size.width, _size.height);
		}
	}

	for (; count<visible_items+1; count++) {
		default_y = (_item_size+y)*count+y;
		
		int gap = 0;

		if ((default_y+_item_size) >= (_size.height-y)) {
			gap = (default_y+_item_size)-(_size.height-y);
		}

		g->SetColor(_item_color);
		FillRectangle(g, x, default_y, w-scroll_width-scroll_gap, _item_size-gap);
	}
	
	if (_scroll == SCROLL_BAR) {
		jcolor_t color,
						 disable;
		int diff = 0x40;

		color.red = 0x80;
		color.green = 0x80;
		color.blue = 0xe0;
		color.alpha = 0xff;

		disable = color.Darker(diff, diff, diff, 0x00);

		g->SetColor(_item_color);
		FillRectangle(g, _size.width-x-scroll_width, y, scroll_width, h);

		if ((_centered_interaction == true && _index > 0) | (_centered_interaction == false && _top_index > 0)) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		x = _size.width-x-scroll_width,
		
		g->FillTriangle(x, y+_item_size/2, x+scroll_width/2, y, x+scroll_width, y+_item_size/2);

		if ((_centered_interaction == true && (_index < (int)(_items.size()-1))) || (_centered_interaction == false && (_top_index+visible_items) < (int)(_items.size()))) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		g->FillTriangle(x, y+h-_item_size/2, x+scroll_width, y+h-_item_size/2, x+scroll_width/2, y+h);
		
		g->SetColor(color);

		if (visible_items <= (int)_items.size()) {
			double diff = (h-2*_item_size-8)/(double)(_items.size()-1);
			
			FillRectangle(g, x, (int)(y+_item_size/2+diff*_index+4), scroll_width, _item_size);
		}
	}

	PaintEdges(g);
}

void ListBox::SetCurrentIndex(int i)
{
	int visible_items = GetVisibleItems();

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

				if (_index >= (_top_index + visible_items)) {
					_top_index = _index-visible_items+1;

					if (_top_index < 0) {
						_top_index = 0;
					}
				}
			}

			Repaint();
		}
	}
}

bool ListBox::IsSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return false;
	}

	if (_selection == SINGLE_SELECTION) {
		if (_selected_index == i) {
			return true;
		}
	} else if (_selection == MULTI_SELECTION) {
		return _items[i]->IsSelected();
	}

	return false;
}

void ListBox::SetSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	if (_selection == SINGLE_SELECTION) {
		if (_selected_index == i) {
			_selected_index = -1;
		} else {
			_selected_index = i;
		}

		Repaint();
	} else if (_selection == MULTI_SELECTION) {
		if (_items[i]->IsSelected()) {
			_items[i]->SetSelected(false);
		} else {
			_items[i]->SetSelected(true);
		}

		Repaint();
	}
}

void ListBox::Select(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	if (_selection == SINGLE_SELECTION) {
		_selected_index = i;

		Repaint();
	} else if (_selection == MULTI_SELECTION) {
		_items[i]->SetSelected(true);

		Repaint();
	}
}

void ListBox::Deselect(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	if (_selection == SINGLE_SELECTION) {
		_selected_index = -1;

		Repaint();
	} else if (_selection == MULTI_SELECTION) {
		_items[i]->SetSelected(false);

		Repaint();
	}
}

int ListBox::GetSelectedIndex()
{
	return _selected_index;
}

bool ListBox::ProcessEvent(MouseEvent *event)
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
	}

	return catched;
}

bool ListBox::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	int visible_items = GetVisibleItems();
	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_UP || action == JKEY_PAGE_UP) {
		if (_items.size() > 0) {
			int old_index = _index;

			if (action == JKEY_CURSOR_UP) {
				_index--;
			} else {
				_index = _index - visible_items;
			}

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

				DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, UP_ITEM)); 
			}
		}

		catched = true;
	} else if (action == JKEY_CURSOR_DOWN || action == JKEY_PAGE_DOWN) {
		if (_items.size() > 0) { 
			int old_index = _index;

			if (action == JKEY_CURSOR_DOWN) {
				_index++;
			} else {
				_index = _index + visible_items;
			}

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

			if (_index >= (_top_index + visible_items)) {
				_top_index = _index-visible_items+1;

				if (_top_index < 0) {
					_top_index = 0;
				}
			}

			if (_index != old_index) {
				Repaint();

				DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, DOWN_ITEM)); 
			}
		}

		catched = true;
	} else if (action == JKEY_ENTER) {
		SetSelected(_index);

		if (_items.size() > 0) { 
			DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, ACTION_ITEM));
		}

		catched = true;
	}

	return catched;
}

}
