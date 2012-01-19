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
#include "jlistbox.h"
#include "jdebug.h"
#include "joutofboundsexception.h"

namespace jgui {

ListBox::ListBox(int x, int y, int width, int height):
  ItemComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ListBox");

	_item_gap = 4;
	_pressed = false;
	_item_size = DEFAULT_ITEM_SIZE;
	_selected_index = -1;
	_selection = JLM_NONE_SELECTION;

	SetFocusable(true);
}

ListBox::~ListBox() 
{
}

void ListBox::SetSelectionType(jlistbox_mode_t type)
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

void ListBox::AddEmptyItem()
{
	Item *item = new Item();

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

void ListBox::AddTextItem(std::string text)
{
	Item *item = new Item(text);

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

void ListBox::AddImageItem(std::string text, std::string image)
{
	Item *item = new Item(text, image);

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

void ListBox::AddCheckedItem(std::string text, bool checked)
{
	Item *item = new Item(text, checked);

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
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
	
	Repaint();
}

void ListBox::SetForegroundColor(int red, int green, int blue, int alpha)
{
	SetItemForegroundColor(red, green, blue, alpha);
}

void ListBox::SetForegroundFocusColor(int red, int green, int blue, int alpha)
{
	SetItemForegroundFocusColor(red, green, blue, alpha);
}

void ListBox::SetCurrentIndex(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		throw jcommon::OutOfBoundsException("Index out of bounds exception");
	}

	jthread::AutoLock lock(&_component_mutex);

	_index = i;

	Repaint();
}

bool ListBox::IsSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return false;
	}

	if (_selection == JLM_SINGLE_SELECTION) {
		if (_selected_index == i) {
			return true;
		}
	} else if (_selection == JLM_MULTI_SELECTION) {
		return _items[i]->IsSelected();
	}

	return false;
}

void ListBox::SetSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	Item *item = _items[i];

	if (item->IsEnabled() == false) {
		return;
	}

	if (_selection == JLM_SINGLE_SELECTION) {
		if (_selected_index == i) {
			_selected_index = -1;
		} else {
			_selected_index = i;
		}

		Repaint();
	} else if (_selection == JLM_MULTI_SELECTION) {
		if (item->IsSelected()) {
			item->SetSelected(false);
		} else {
			item->SetSelected(true);
		}

		Repaint();
	}
}

void ListBox::Select(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	Item *item = _items[i];

	if (item->IsEnabled() == false) {
		return;
	}

	if (_selection == JLM_SINGLE_SELECTION) {
		_selected_index = i;

		Repaint();
	} else if (_selection == JLM_MULTI_SELECTION) {
		item->SetSelected(true);

		Repaint();
	}
}

void ListBox::Deselect(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	Item *item = _items[i];

	if (item->IsEnabled() == false) {
		return;
	}

	if (_selection == JLM_SINGLE_SELECTION) {
		_selected_index = -1;

		Repaint();
	} else if (_selection == JLM_MULTI_SELECTION) {
		item->SetSelected(false);

		Repaint();
	}
}

int ListBox::GetItemGap()
{
	return _item_gap;
}

void ListBox::SetItemGap(int gap)
{
	_item_gap = gap;
}

int ListBox::GetSelectedIndex()
{
	return _selected_index;
}

jsize_t ListBox::GetPreferredSize()
{
	jsize_t t;

	t.width = _size.width;
	t.height = 2*(_vertical_gap+_border_size)+_items.size()*(_item_size+_item_gap)-_item_gap;

	return t;
}

bool ListBox::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	return false;
}

bool ListBox::ProcessEvent(KeyEvent *event)
{
	if (IsEnabled() == false) {
		return false;
	}

	jkeyevent_symbol_t action = event->GetSymbol();

	bool catched = false;

	if (action == JKS_CURSOR_UP) {
		IncrementLines(1);
		
		catched = true;
	} else if (action == JKS_PAGE_UP) {
		IncrementLines((_size.height-2*(_border_size+_vertical_gap))/(_item_size+_item_gap));
		
		catched = true;
	} else if (action == JKS_CURSOR_DOWN) {
		DecrementLines(1);

		catched = true;
	} else if (action == JKS_PAGE_DOWN) {
		DecrementLines((_size.height-2*(_border_size+_vertical_gap))/(_item_size+_item_gap));

		catched = true;
	} else if (action == JKS_HOME) {
		IncrementLines(_items.size());
		
		catched = true;
	} else if (action == JKS_END) {
		DecrementLines(_items.size());
		
		catched = true;
	} else if (action == JKS_ENTER) {
		SetSelected(_index);

		if (_items.size() > 0) { 
			DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_ACTION));
		}

		catched = true;
	}

	return catched || Component::ProcessEvent(event);
}

void ListBox::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0,
			scrollw = (IsScrollableY() == true)?(_scroll_size+_scroll_gap):0;
	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-scrollw-2*x;
			// h = _size.height-2*y;
	int space = 4;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == JMT_IMAGE) {
			space += _item_size + 8;

			break;
		}
	}

	// TODO:: tentar ajeitar
	x = x - scrollx;
	y = y - scrolly;

	for (int i=0; i<(int)_items.size(); i++) {
		int dy = y+(_item_size+_item_gap)*i;

		if ((dy+_item_size) < 0 || dy > _size.height) {
			continue;
		}

		Item *item = _items[i];

		if (item->IsEnabled() == true) {
			g->SetColor(GetItemColor());
		} else {
			g->SetColor(GetDisabledItemColor());
		}

		if (_index != i) {
			if (_selection == JLM_SINGLE_SELECTION) {	
				if (_selected_index == i) {	
					g->SetColor(_selected_item_color);
				}
			} else if (_selection == JLM_MULTI_SELECTION) {	
				if (item->IsSelected() == true) {	
					g->SetColor(_selected_item_color);
				}
			}
		} else {
			g->SetColor(_focus_item_color);
		}

		g->FillRectangle(x, y+(_item_size+_item_gap)*i, w, _item_size);

		if (_selection == JLM_SINGLE_SELECTION) {
			if (_selected_index == i) {
				g->SetColor(_selected_item_color);
			}
		} else if (_selection == JLM_MULTI_SELECTION) {	
			if (_items[i]->IsSelected() == true) {	
				g->SetColor(_selected_item_color);
			}
		} else {
			g->SetColor(_item_color);
		}

		if (_items[i]->GetType() == JMT_EMPTY) {
		} else if (_items[i]->GetType() == JMT_TEXT) {
		} else if (_items[i]->GetType() == JMT_IMAGE) {
			if (_items[i]->GetImage() != NULL) {
				g->DrawImage(_items[i]->GetImage(), _horizontal_gap, y+(_item_size+_item_gap)*i, _item_size, _item_size);
			}
		}

		if (IsFontSet() == true) {
			if (_is_enabled == true) {
				if (_has_focus == true) {
					g->SetColor(_focus_fgcolor);
				} else {
					g->SetColor(_fgcolor);
				}
			} else {
				g->SetColor(_disabled_fgcolor);
			}

			std::string text = _items[i]->GetValue();

			// if (_wrap == false) {
				text = _font->TruncateString(text, "...", w-space);
			// }

			g->DrawString(text, x+space, y+(_item_size+_item_gap)*i, w-space, _item_size, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
		}
	}
}

void ListBox::IncrementLines(int lines)
{
	if (_items.size() == 0) {
		return;
	}

	int old_index = _index;

	_index = _index - lines;

	if (_index < 0) {
		if (_loop == false) {
			_index = 0;
		} else {
			_index = (int)(_items.size()-1);
		}
	}

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;

	if (scrolly > 0) {
		ScrollToVisibleArea(scrollx, std::max(0, (_item_size+_item_gap)*_index), _size.width, _size.height, this);
	}

	if (_index != old_index) {
		Repaint();

		DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_UP)); 
	}
}

void ListBox::DecrementLines(int lines)
{
	if (_items.size() == 0) { 
		return;
	}

	int old_index = _index;

	_index = _index + lines;

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

	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0;

	if ((scrolly+_size.height) < (_item_size+_item_gap)*_items.size()) {
		ScrollToVisibleArea(scrollx, (_item_size+_item_gap)*_index, _size.width, _size.height, this);
	}

	if (_index != old_index) {
		Repaint();

		DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_DOWN)); 
	}
}

jsize_t ListBox::GetScrollDimension()
{
	jsize_t size;

	size.width = _size.width;
	size.height = _items.size()*(_item_size+_item_gap)+2*(_vertical_gap+_border_size);

	return  size;
}

}
