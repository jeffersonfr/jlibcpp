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
#include "jgui/jlistbox.h"
#include "jlogger/jloggerlib.h"
#include "jexception/joutofboundsexception.h"

namespace jgui {

ListBox::ListBox(int x, int y, int width, int height):
  Component(x, y, width, height),
  ItemComponent()
{
	jcommon::Object::SetClassName("jgui::ListBox");

	_pressed = false;
	_selected_index = -1;
	_mode = JLBM_NONE_SELECTION;

	SetFocusable(true);
}

ListBox::~ListBox() 
{
}

void ListBox::SetSelectionType(jlistbox_mode_t type)
{
	if (_mode == type) {
		return;
	}

	_mode = type;
	_selected_index = -1;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		(*i)->SetSelected(false);
	}
}

jlistbox_mode_t ListBox::GetSelectionType()
{
	return _mode;
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

void ListBox::AddImageItem(std::string text, jgui::Image *image)
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

void ListBox::SetCurrentIndex(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		throw jexception::OutOfBoundsException("Index out of bounds exception");
	}

	Theme *theme = GetTheme();

  if (theme == NULL) {
    return;
  }

	int 
		is = theme->GetIntegerParam("item.size"),
		ig = theme->GetIntegerParam("item.gap");

	_index = i;

	SetScrollY(_index*(is + ig));

	Repaint();
}

bool ListBox::IsSelected(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return false;
	}

	if (_mode == JLBM_SINGLE_SELECTION) {
		if (_selected_index == i) {
			return true;
		}
	} else if (_mode == JLBM_MULTI_SELECTION) {
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

	if (_mode == JLBM_SINGLE_SELECTION) {
		if (_selected_index == i) {
			_selected_index = -1;
		} else {
			_selected_index = i;
		}

		Repaint();
	} else if (_mode == JLBM_MULTI_SELECTION) {
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

	if (_mode == JLBM_SINGLE_SELECTION) {
		_selected_index = i;

		Repaint();
	} else if (_mode == JLBM_MULTI_SELECTION) {
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

	if (_mode == JLBM_SINGLE_SELECTION) {
		_selected_index = -1;

		Repaint();
	} else if (_mode == JLBM_MULTI_SELECTION) {
		item->SetSelected(false);

		Repaint();
	}
}

int ListBox::GetSelectedIndex()
{
	return _selected_index;
}

jsize_t ListBox::GetPreferredSize()
{
	Theme *theme = GetTheme();

	jsize_t t {0, 0};

  if (theme == NULL) {
    return t;
  }

  int
    // gx = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		gy = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size");
	int 
		is = theme->GetIntegerParam("item.size"),
		ig = theme->GetIntegerParam("item.gap");

	t.width = _size.width;
	t.height = 2*gy + _items.size()*(is + ig) - ig;

	return t;
}

bool ListBox::KeyPressed(jevent::KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	if (IsEnabled() == false) {
		return false;
	}

	Theme *theme = GetTheme();

  if (theme == NULL) {
    return false;
  }
	
  int
    bs = theme->GetIntegerParam("component.border.size"),
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap"),
    // hp = theme->GetIntegerParam("component.hgap"),
		vg = theme->GetIntegerParam("component.vgap");
	jevent::jkeyevent_symbol_t action = event->GetSymbol();
	bool catched = false;

	if (action == jevent::JKS_CURSOR_UP) {
		IncrementLines(1);
		
		catched = true;
	} else if (action == jevent::JKS_PAGE_UP) {
		IncrementLines((_size.height-2*(bs + vg))/(is + ig));
		
		catched = true;
	} else if (action == jevent::JKS_CURSOR_DOWN) {
		DecrementLines(1);

		catched = true;
	} else if (action == jevent::JKS_PAGE_DOWN) {
		DecrementLines((_size.height-2*(bs + vg))/(is + ig));

		catched = true;
	} else if (action == jevent::JKS_HOME) {
		IncrementLines(_items.size());
		
		catched = true;
	} else if (action == jevent::JKS_END) {
		DecrementLines(_items.size());
		
		catched = true;
	} else if (action == jevent::JKS_ENTER) {
		SetSelected(_index);

		if (_items.size() > 0) { 
			DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_ACTION));
		}

		catched = true;
	}

	return catched;
}

bool ListBox::MousePressed(jevent::MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	return false;
}

bool ListBox::MouseReleased(jevent::MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}
	
bool ListBox::MouseMoved(jevent::MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool ListBox::MouseWheel(jevent::MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}
	
	Theme *theme = GetTheme();

  if (theme == NULL) {
    return false;
  }
	
	SetScrollY(GetScrollY() + theme->GetIntegerParam("item.size")*event->GetClickCount());

	Repaint();

	return true;
}

void ListBox::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Theme *theme = GetTheme();
  
  if (theme == NULL) {
    return;
  }
	
	jgui::Font 
    *font = theme->GetFont("component.font");
	Color 
    bg = theme->GetIntegerParam("component.bg"),
    fg = theme->GetIntegerParam("component.fg"),
    fgfocus = theme->GetIntegerParam("component.fg.focus"),
    fgdisable = theme->GetIntegerParam("component.fg.disable"),
    itembg = theme->GetIntegerParam("item.bg"),
    itemfg = theme->GetIntegerParam("item.fg"),
    itembgselect = theme->GetIntegerParam("item.bg.select"),
    itembgdisable = theme->GetIntegerParam("item.bg.disable"),
    itemfgdisable = theme->GetIntegerParam("item.fg.disable"),
    itembgfocus = theme->GetIntegerParam("item.bg.focus"),
    itemfgfocus = theme->GetIntegerParam("item.fg.focus");
	// int 
    // bs = theme->GetIntegerParam("component.border.size"),
    // ss = theme->GetIntegerParam("component.scroll.size"),
    // sg = theme->GetIntegerParam("component.scroll.gap");
	int 
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap");
  int
    hg = theme->GetIntegerParam("component.hgap");
		// vg = theme->GetIntegerParam("component.vgap");
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
		w = _size.width - 2*x;
		// h = _size.height - 2*y;

	// jsize_t scroll_dimension = GetScrollDimension();
	jpoint_t 
    scroll_location = GetScrollLocation();
	int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
		scrolly = (IsScrollableY() == true)?scroll_location.y:0;
		// scrollw = (IsScrollableY() == true)?(ss + sg):0;
	int 
    offset = 4;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == JIT_IMAGE) {
			offset += is + 8;

			break;
		}
	}

	// CHANGE:: try enhance scroll behaviour
	x = x - scrollx;
	y = y - scrolly;

	for (int i=0; i<(int)_items.size(); i++) {
		int dy = y+(is + ig)*i;

		if ((dy + is) < 0 || dy > _size.height) {
			continue;
		}

		Item *item = _items[i];

		if (item->IsEnabled() == true) {
			g->SetColor(itembg);
		} else {
			g->SetColor(itembgdisable);
		}

		if (_index != i) {
			if (_mode == JLBM_SINGLE_SELECTION) {	
				if (_selected_index == i) {	
					g->SetColor(itembgselect);
				}
			} else if (_mode == JLBM_MULTI_SELECTION) {	
				if (item->IsSelected() == true) {	
					g->SetColor(itembgselect);
				}
			}
		} else {
			g->SetColor(itembgfocus);
		}

		g->FillRectangle(x, y + (is + ig)*i, w, is);

		if (_mode == JLBM_SINGLE_SELECTION) {
			if (_selected_index == i) {
				g->SetColor(itembgselect);
			}
		} else if (_mode == JLBM_MULTI_SELECTION) {	
			if (_items[i]->IsSelected() == true) {	
				g->SetColor(itembgselect);
			}
		} else {
			g->SetColor(itembg);
		}

		if (_items[i]->GetType() == JIT_EMPTY) {
		} else if (_items[i]->GetType() == JIT_TEXT) {
		} else if (_items[i]->GetType() == JIT_IMAGE) {
			if (_items[i]->GetImage() != NULL) {
				g->DrawImage(_items[i]->GetImage(), hg, y+(is + ig)*i, is, is);
			}
		}

		if (font != NULL) {
			g->SetFont(font);

			if (_is_enabled == true) {
				if (_has_focus == true) {
					g->SetColor(itemfgfocus);
				} else {
					g->SetColor(itemfg);
				}
			} else {
				g->SetColor(itemfgdisable);
			}

			std::string text = _items[i]->GetValue();

			// if (_wrap == false) {
				text = font->TruncateString(text, "...", w - offset);
			// }

			g->DrawString(text, x + offset, y + (is + ig)*i, w - offset, is, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
		}
	}
}

void ListBox::IncrementLines(int lines)
{
	if (_items.size() == 0) {
		return;
	}

	Theme *theme = GetTheme();
  
  if (theme == NULL) {
    return;
  }
	
	int 
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap");

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

	if ((is + ig)*_index < scrolly) {
		ScrollToVisibleArea(scrollx, (std::max)(0, (is + ig)*_index), _size.width, _size.height, this);
	} else if ((scrolly+_size.height) < (is + ig)*(int)_index) {
		ScrollToVisibleArea(scrollx, (is + ig)*(_index + 1) - _size.height + 2*ig, _size.width, _size.height, this);
	}

	if (_index != old_index) {
		Repaint();

		DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_UP)); 
	}
}

void ListBox::DecrementLines(int lines)
{
	if (_items.size() == 0) { 
		return;
	}

	Theme *theme = GetTheme();
  
  if (theme == NULL) {
    return;
  }
	
	int 
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap");
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

	if ((scrolly + _size.height) < (is + ig)*(int)(_index + 1)) {
		ScrollToVisibleArea(scrollx, (is + ig)*(_index + 1)-_size.height+2*ig, _size.width, _size.height, this);
	} else if ((is + ig)*_index < scrolly) {
		ScrollToVisibleArea(scrollx, (std::max)(0, (is + ig)*_index), _size.width, _size.height, this);
	}

	if (_index != old_index) {
		Repaint();

		DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_DOWN)); 
	}
}

jsize_t ListBox::GetScrollDimension()
{
	Theme *theme = GetTheme();

	jsize_t t {0, 0};

  if (theme == NULL) {
    return t;
  }

	int 
    bs = theme->GetIntegerParam("component.border.size");
	int 
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap");
	int 
    // hg = theme->GetIntegerParam("component.hgap"),
    vg = theme->GetIntegerParam("component.vgap");

	t.width = _size.width;
	t.height = _items.size()*(is + ig) + 2*(vg + bs);

	return t;
}

}
