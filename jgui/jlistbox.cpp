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
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ListBox");

	bx = x+10;
	by = y+20;
	bwidth = 30;
	bheight = 30;
	delta = 1.0f;

	_align = LEFT_ALIGN;
	_centered_interaction = true;
	_loop = false;
	_index = 0;
	_top_index = 0;
	_selected_index = -1;
	_input_locked = false;
	_scroll = SCROLL_BAR;
	_selection = NONE_SELECTION;

	SetAlign(LEFT_ALIGN);

	_item_size = DEFAULT_COMPONENT_HEIGHT;

	if (_font != NULL) {
		_item_size = _font->GetHeight();
	}

	SetFocusable(true);
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
}

ListBox::~ListBox() 
{
	for (std::vector<listbox_t>::iterator i=_items.begin(); i!=_items.end(); i++) {
		OffScreenImage *p = (*i).image;

		if (p != NULL) {
			delete p;
		}
	}
}

void ListBox::SetCenteredInteraction(bool b)
{
	_centered_interaction = b;
}

int ListBox::GetVisibleItems()
{
	int visible_items = _size.height/(_item_size+_vertical_gap);

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

	for (std::vector<listbox_t>::iterator i=_items.begin(); i!=_items.end(); i++) {
		(*i).check = false;
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

int ListBox::GetItemsSize()
{
	return _items.size();
}

void ListBox::SetLoop(bool loop)
{
	if (_loop == loop) {
		return;
	}

	_loop = loop;

	Repaint();
}

jcolor_t ListBox::GetItemColor()
{
	return _item_color;
}

void ListBox::SetItemColor(jcolor_t color)
{
	SetItemColor(color.red, color.green, color.blue, color.alpha);
}

void ListBox::SetItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_color.red = red;
	_item_color.green = green;
	_item_color.blue = blue;
	_item_color.alpha = alpha;
}

void ListBox::SetScrollType(jlist_scroll_type_t type)
{
	jthread::AutoLock lock(&_component_mutex);

	_scroll = type;
}

void ListBox::SetAlign(jalign_t align)
{
	if (_align != align) {
		jthread::AutoLock lock(&_component_mutex);

		_align = align;

		Repaint();
	}
}

jalign_t ListBox::GetAlign()
{
	return _align;
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

	int i,
		count = 0,
		default_y = 0,
		space = 5,
		scroll_width = 0,
		scroll_gap = 0;

	for (std::vector<listbox_t>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i).type == IMAGE_ITEM) {
			space += _item_size + 10;

			break;
		}
	}

	if ((int)_items.size() < visible_items) {
		position = 0;
	}

	default_y = (_item_size+_vertical_gap)*count+_vertical_gap;

	if (_scroll == SCROLL_BAR) {
		scroll_width = 30;
		scroll_gap = 5;
	}

	for (i=position; count<visible_items && i<(int)_items.size(); i++, count++) {
		default_y = (_item_size+_vertical_gap)*count+_vertical_gap;

		if (_index != i) {
			if (_selection == SINGLE_SELECTION) {	
				if (_selected_index == i) {	
					g->SetColor(_fg_color);
					FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
				} else {
					g->SetColor(_item_color);
					FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
				}
			} else if (_selection == MULTI_SELECTION) {	
				if (_items[i].check == true) {	
					g->SetColor(_fg_color);
					FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
				} else {
					g->SetColor(_item_color);
					FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
				}
			} else {
				g->SetColor(_item_color);
				FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);
			}
		} else {
			g->SetColor(_bgfocus_color);
			FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size);

			/*
			g->FillGradientRectangle(_horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, (int)((_item_size)*0.6), _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			g->FillGradientRectangle(_horizontal_gap, default_y+(_item_size)/2, _width-2*_horizontal_gap-scroll_width-scroll_gap, (_item_size)/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
			*/
		}

		if (_selection == SINGLE_SELECTION) {
			if (_selected_index == i) {
				g->SetColor(_item_color);
			} else {
				g->SetColor(_fg_color);
			}
		} else if (_selection == MULTI_SELECTION) {	
			if (_items[i].check == true) {	
				g->SetColor(_item_color);
			} else {
				g->SetColor(_fg_color);
			}
		} else {
			g->SetColor(_fg_color);
		}

		if (_items[i].type == EMPTY_ITEM) {
		} else if (_items[i].type == TEXT_ITEM) {
			g->DrawString(TruncateString(_items[i].text, _size.width-space-_horizontal_gap-scroll_width-scroll_gap), space+_horizontal_gap, default_y, _size.width-space-_horizontal_gap-scroll_width-scroll_gap, _item_size, _align);
		} else if (_items[i].type == IMAGE_ITEM) {
			if (_items[i].image == NULL) {
				g->DrawString(TruncateString(_items[i].text, _size.width-space-_horizontal_gap-scroll_width-scroll_gap), space+_horizontal_gap, default_y, _size.width-space-_horizontal_gap-scroll_width-scroll_gap, _item_size, _align);
			} else {
				g->DrawString(TruncateString(_items[i].text, _size.width-space-_horizontal_gap-scroll_width-scroll_gap), space+_horizontal_gap, default_y, _size.width-space-_horizontal_gap-scroll_width-scroll_gap, _item_size, _align);
				g->DrawImage(_items[i].image, 10, default_y, _item_size, _item_size);
			}
		}
	}

	for (; count<visible_items+1; count++) {
		default_y = (_item_size+_vertical_gap)*count+_vertical_gap;
		
		int gap = 0;

		if ((default_y+_item_size) >= (_size.height-_vertical_gap)) {
			gap = (default_y+_item_size)-(_size.height-_vertical_gap);
		}

		g->SetColor(_item_color);
		FillRectangle(g, _horizontal_gap, default_y, _size.width-2*_horizontal_gap-scroll_width-scroll_gap, _item_size-gap);
	}
	
	if (_scroll == SCROLL_BAR) {
		g->SetColor(_item_color);
		FillRectangle(g, _size.width-_horizontal_gap-scroll_width, _vertical_gap, scroll_width, _size.height-2*_vertical_gap);
				
		int dx = _size.width-_horizontal_gap-scroll_width+2,
			dy = _vertical_gap+4;
		
		scroll_width -= 4;

		if (_centered_interaction == true) {
			if (_index > 0) {
				g->SetColor(0x00, 0x40, 0xf0, 0xff);
			} else {
				g->SetColor(0x80, 0x80, 0xe0, 0xff);
			}
		} else {
			if (_top_index > 0) {
				g->SetColor(0x00, 0x40, 0xf0, 0xff);
			} else {
				g->SetColor(0x80, 0x80, 0xe0, 0xff);
			}
		}

		g->SetColor(0x80, 0x80, 0xe0, 0xff);
		g->FillTriangle(dx+0, dy+_item_size/2, dx+scroll_width/2, dy+2, dx+scroll_width, dy+_item_size/2);

		if (_centered_interaction == true) {
			if (_index < (int)(_items.size()-1)) {
				g->SetColor(0x00, 0x40, 0xf0, 0xff);
			} else {
				g->SetColor(0x80, 0x80, 0xe0, 0xff);
			}
		} else {
			if ((_top_index+visible_items) < (int)(_items.size())) {
				g->SetColor(0x00, 0x40, 0xf0, 0xff);
			} else {
				g->SetColor(0x80, 0x80, 0xe0, 0xff);
			}
		}

		dy = _size.height-_vertical_gap-_item_size/2-4;

		g->SetColor(0x80, 0x80, 0xe0, 0xff);
		g->FillTriangle(dx+0, dy, dx+scroll_width, dy, dx+scroll_width/2, dy+_item_size/2-2);
		
		if (visible_items <= (int)_items.size()) {
			double diff = (_size.height-2*_vertical_gap-2*_item_size-8)/(double)(_items.size()-1);
			
			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillRectangle(dx+2, (int)(_vertical_gap+_item_size/2+diff*_index+4), scroll_width-4, _item_size);
		}
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _size.width, _size.height);
	}
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
		return _items[i].check;
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
		if (_items[i].check == true) {
			_items[i].check = false;
		} else {
			_items[i].check = true;
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
		_items[i].check = true;

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
		_items[i].check = false;

		Repaint();
	}
}

void ListBox::AddEmptyItem()
{
	listbox_t t;

	t.text = "";
	t.image = NULL;
	t.check = false;
	t.type = EMPTY_ITEM;

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(t);
	}

	Repaint();
}

void ListBox::AddItem(std::string text)
{
	listbox_t t;

	t.text = text;
	t.image = NULL;
	t.check = false;
	t.type = TEXT_ITEM;

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(t);
	}

	Repaint();
}

void ListBox::AddItem(std::string text, std::string image)
{
	listbox_t t;

	t.text = text;
	t.check = false;
	t.type = IMAGE_ITEM;

	t.image = new OffScreenImage(_item_size, _item_size);
	
	if (t.image->GetGraphics() != NULL) {
		t.image->GetGraphics()->DrawImage(image, 0, 0, _item_size, _item_size);
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.push_back(t);
	}

	Repaint();
}

std::string ListBox::GetValue(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return "";
	}

	if (_items.size() > 0) {
		return _items[i].text;
	}

	return "";
}

std::string ListBox::GetCurrentValue()
{
	if (_items.size() > 0) {
		return _items[_index].text;
	} 

	return "";
}

int ListBox::GetCurrentIndex()
{
	return _index;
}

int ListBox::GetSelectedIndex()
{
	return _selected_index;
}

void ListBox::RemoveItem(int index)
{
	if (index < 0 || index >= (int)_items.size()) {
		return;
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_items.erase(_items.begin()+index);
	}

	Repaint();
}

void ListBox::RemoveAll()
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_items.clear();
	}

	Repaint();
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

	/*
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

				DispatchEvent(new SelectEvent(this, _items[_index].text, _index, UP_ITEM)); 
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

				DispatchEvent(new SelectEvent(this, _items[_index].text, _index, DOWN_ITEM)); 
			}
		}

		catched = true;
	} else if (action == JKEY_ENTER) {
		SetSelected(_index);

		if (_items.size() > 0) { 
			DispatchEvent(new SelectEvent(this, _items[_index].text, _index, ACTION_ITEM));
		}

		catched = true;
	}
	*/

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

				DispatchEvent(new SelectEvent(this, _items[_index].text, _index, UP_ITEM)); 
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

				DispatchEvent(new SelectEvent(this, _items[_index].text, _index, DOWN_ITEM)); 
			}
		}

		catched = true;
	} else if (action == JKEY_ENTER) {
		SetSelected(_index);

		if (_items.size() > 0) { 
			DispatchEvent(new SelectEvent(this, _items[_index].text, _index, ACTION_ITEM));
		}

		catched = true;
	}

	return catched;
}

void ListBox::RegisterSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_listbox_listeners.begin(), _listbox_listeners.end(), listener) == _listbox_listeners.end()) {
		_listbox_listeners.push_back(listener);
	}
}

void ListBox::RemoveSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<SelectListener *>::iterator i = std::find(_listbox_listeners.begin(), _listbox_listeners.end(), listener);

	if (i != _listbox_listeners.end()) {
		_listbox_listeners.erase(i);
	}
}

void ListBox::DispatchEvent(SelectEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_listbox_listeners.size()) {
		SelectListener *listener = _listbox_listeners[k-1];

		if (event->GetType() == ACTION_ITEM) {
			listener->ItemSelected(event);
		} else {
			listener->ItemChanged(event);
		}
	}

	/*
	for (std::vector<SelectListener *>::iterator i=_listbox_listeners.begin(); i!=_listbox_listeners.end(); i++) {
		if (event->GetType() == ACTION_ITEM) {
			(*i)->ItemSelected(event);
		} else {
			(*i)->ItemChanged(event);
		}
	}
	*/

	delete event;
}

std::vector<SelectListener *> & ListBox::GetSelectListeners()
{
	return _listbox_listeners;
}

}
