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
#include "jtreelistview.h"
#include "jthememanager.h"
#include "joutofboundsexception.h"
#include "jthememanager.h"
#include "jdebug.h"

namespace jgui {

TreeListView::TreeListView(int x, int y, int width, int height):
  Component(x, y, width, height),
  ItemComponent()
{
	jcommon::Object::SetClassName("jgui::TreeListView");

	_item_gap = 4;
	_pressed = false;
	_item_size = DEFAULT_COMPONENT_HEIGHT;
	_selected_index = -1;

	SetFocusable(true);
}

TreeListView::~TreeListView() 
{
}

void TreeListView::AddEmptyItem()
{
	Item *item = new Item();

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

void TreeListView::AddTextItem(std::string text)
{
	Item *item = new Item(text);

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

void TreeListView::AddImageItem(std::string text, std::string image)
{
	Item *item = new Item(text, image);

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

void TreeListView::AddCheckedItem(std::string text, bool checked)
{
	Item *item = new Item(text, checked);

	item->SetHorizontalAlign(JHA_LEFT);
		
	AddInternalItem(item);
	AddItem(item);
}

int TreeListView::GetItemSize()
{
	return _item_size;
}

void TreeListView::SetItemSize(int size)
{
	if (size <= 0) {
		return;
	}

	_item_size = size;
	
	Repaint();
}

void TreeListView::SetCurrentIndex(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		throw jcommon::OutOfBoundsException("Index out of bounds exception");
	}

	{
		jthread::AutoLock lock(&_component_mutex);

		_index = i;

		SetScrollY(_index*(GetItemSize()+GetItemGap()));
	}

	Repaint();
}

bool TreeListView::IsSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return false;
	}

	if (_selected_index == i) {
		return true;
	}

	return false;
}

void TreeListView::SetSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	Item *item = _items[i];

	if (item->IsEnabled() == false) {
		return;
	}

	if (_selected_index == i) {
		_selected_index = -1;
	} else {
		_selected_index = i;
	}

	Repaint();
}

void TreeListView::Select(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	Item *item = _items[i];

	if (item->IsEnabled() == false) {
		return;
	}

	_selected_index = i;

	Repaint();
}

void TreeListView::Deselect(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	Item *item = _items[i];

	if (item->IsEnabled() == false) {
		return;
	}

	_selected_index = -1;

	Repaint();
}

void TreeListView::Expand(Item *item)
{
	_expanded_items[item] = true;

	Repaint();
}

void TreeListView::Collapse(Item *item)
{
	_expanded_items[item] = false;

	Repaint();
}

bool TreeListView::IsExpanded(Item *item)
{
	return _expanded_items[item];
}

void TreeListView::ExpandAll(Item *item)
{
	for (std::map<Item *, bool>::iterator i=_expanded_items.begin(); i!=_expanded_items.end(); i++) {
		_expanded_items[i->first] = true;
	}

	Repaint();
}

void TreeListView::CollapseAll(Item *item)
{
	for (std::map<Item *, bool>::iterator i=_expanded_items.begin(); i!=_expanded_items.end(); i++) {
		_expanded_items[i->first] = false;
	}

	Repaint();
}

int TreeListView::GetItemGap()
{
	return _item_gap;
}

void TreeListView::SetItemGap(int gap)
{
	_item_gap = gap;
}

int TreeListView::GetSelectedIndex()
{
	return _selected_index;
}

jsize_t TreeListView::GetPreferredSize()
{
	Theme *theme = GetTheme();
	int bordersize = theme->GetBorderSize("component");

	jsize_t t;

	t.width = _size.width;
	t.height = 2*(_vertical_gap+bordersize)+_items.size()*(_item_size+_item_gap)-_item_gap;

	return t;
}

bool TreeListView::KeyPressed(KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	if (IsEnabled() == false) {
		return false;
	}

	Theme *theme = GetTheme();
	int bordersize = theme->GetBorderSize("component");

	jkeyevent_symbol_t action = event->GetSymbol();

	bool catched = false;

	if (action == JKS_CURSOR_UP) {
		IncrementLines(1);
		
		catched = true;
	} else if (action == JKS_PAGE_UP) {
		IncrementLines((_size.height-2*(bordersize+_vertical_gap))/(_item_size+_item_gap));
		
		catched = true;
	} else if (action == JKS_CURSOR_DOWN) {
		DecrementLines(1);

		catched = true;
	} else if (action == JKS_PAGE_DOWN) {
		DecrementLines((_size.height-2*(bordersize+_vertical_gap))/(_item_size+_item_gap));

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
			DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JSET_ACTION));
		}

		catched = true;
	}

	return catched;
}

bool TreeListView::MousePressed(MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	return false;
}

bool TreeListView::MouseReleased(MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}
	
bool TreeListView::MouseMoved(MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool TreeListView::MouseWheel(MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}
	
	SetScrollY(GetScrollY()+_item_size*event->GetClickCount());

	Repaint();

	return true;
}

void TreeListView::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");
	Color bg = theme->GetColor("component.bg");
	Color fg = theme->GetColor("component.fg");
	Color fgfocus = theme->GetColor("component.fg.focus");
	Color fgdisable = theme->GetColor("component.fg.disable");
	int bordersize = theme->GetBorderSize("component");

	// jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t scroll_location = GetScrollLocation();
	int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
			scrolly = (IsScrollableY() == true)?scroll_location.y:0,
			scrollw = (IsScrollableY() == true)?(_scroll_size+_scroll_gap):0;
	int x = _horizontal_gap+bordersize,
			y = _vertical_gap+bordersize,
			w = _size.width-scrollw-2*x;
			// h = _size.height-2*y;
	int space = 4;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == JIT_IMAGE) {
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
			if (_selected_index == i) {	
				g->SetColor(_selected_item_color);
			}
		} else {
			g->SetColor(_focus_item_color);
		}

		g->FillRectangle(x, y+(_item_size+_item_gap)*i, w, _item_size);

		// g->SetColor(_item_color);
		if (_selected_index == i) {
			g->SetColor(_selected_item_color);
		}

		if (_items[i]->GetType() == JIT_EMPTY) {
		} else if (_items[i]->GetType() == JIT_TEXT) {
		} else if (_items[i]->GetType() == JIT_IMAGE) {
			if (_items[i]->GetImage() != NULL) {
				g->DrawImage(_items[i]->GetImage(), _horizontal_gap, y+(_item_size+_item_gap)*i, _item_size, _item_size);
			}
		}

		if (font != NULL) {
			g->SetFont(font);

			if (_is_enabled == true) {
				if (_has_focus == true) {
					g->SetColor(fgfocus);
				} else {
					g->SetColor(fg);
				}
			} else {
				g->SetColor(fgdisable);
			}

			std::string text = _items[i]->GetValue();

			// if (_wrap == false) {
				text = font->TruncateString(text, "...", w-space);
			// }

			g->DrawString(text, x+space, y+(_item_size+_item_gap)*i, w-space, _item_size, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
		}
	}
}

void TreeListView::IncrementLines(int lines)
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

	if ((_item_size+_item_gap)*_index < scrolly) {
		ScrollToVisibleArea(scrollx, (std::max)(0, (_item_size+_item_gap)*_index), _size.width, _size.height, this);
	} else if ((scrolly+_size.height) < (_item_size+_item_gap)*(int)_index) {
		ScrollToVisibleArea(scrollx, (_item_size+_item_gap)*(_index+1)-_size.height+2*_item_gap, _size.width, _size.height, this);
	}

	if (_index != old_index) {
		Repaint();

		DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JSET_UP)); 
	}
}

void TreeListView::DecrementLines(int lines)
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

	if ((scrolly+_size.height) < (_item_size+_item_gap)*(int)(_index+1)) {
		ScrollToVisibleArea(scrollx, (_item_size+_item_gap)*(_index+1)-_size.height+2*_item_gap, _size.width, _size.height, this);
	} else if ((_item_size+_item_gap)*_index < scrolly) {
		ScrollToVisibleArea(scrollx, (std::max)(0, (_item_size+_item_gap)*_index), _size.width, _size.height, this);
	}

	if (_index != old_index) {
		Repaint();

		DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JSET_DOWN)); 
	}
}

jsize_t TreeListView::GetScrollDimension()
{
	Theme *theme = GetTheme();
	int bordersize = theme->GetBorderSize("component");

	jsize_t size;

	size.width = _size.width;
	size.height = _items.size()*(_item_size+_item_gap)+2*(_vertical_gap+bordersize);

	return  size;
}

}
