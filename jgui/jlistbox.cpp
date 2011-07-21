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

namespace jgui {

ListBox::ListBox(int x, int y, int width, int height):
  ItemComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ListBox");

	_pressed = false;
	_scroll_width = 30;
	_item_size = DEFAULT_ITEM_SIZE;
	_stone_size = _item_size;
	_centered_interaction = true;
	_top_index = 0;
	_selected_index = -1;
	_scroll = JLS_BAR;
	_selection = JLM_NONE_SELECTION;

	SetFocusable(true);
}

ListBox::~ListBox() 
{
}

int ListBox::GetStoneSize()
{
	return _stone_size;
}

void ListBox::SetStoneSize(int size)
{
	_stone_size = size;

	Repaint();
}

void ListBox::SetCenteredInteraction(bool b)
{
	_centered_interaction = b;
}

int ListBox::GetVisibleItems()
{
	int visible_items = (_size.height-2*(_vertical_gap+_border_size)+_vertical_gap)/(_item_size+_vertical_gap);

	if (visible_items < 1) {
		visible_items = 1;
	}

	return visible_items;
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

void ListBox::SetScrollType(jlistbox_scroll_t type)
{
	jthread::AutoLock lock(&_component_mutex);

	_scroll = type;

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

	if (_selection == JLM_SINGLE_SELECTION) {
		if (_selected_index == i) {
			_selected_index = -1;
		} else {
			_selected_index = i;
		}

		Repaint();
	} else if (_selection == JLM_MULTI_SELECTION) {
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

	if (_selection == JLM_SINGLE_SELECTION) {
		_selected_index = i;

		Repaint();
	} else if (_selection == JLM_MULTI_SELECTION) {
		_items[i]->SetSelected(true);

		Repaint();
	}
}

void ListBox::Deselect(int i)
{
	if (i < 0 || i >= (int)_items.size()) {
		return;
	}

	if (_selection == JLM_SINGLE_SELECTION) {
		_selected_index = -1;

		Repaint();
	} else if (_selection == JLM_MULTI_SELECTION) {
		_items[i]->SetSelected(false);

		Repaint();
	}
}

int ListBox::GetSelectedIndex()
{
	return _selected_index;
}

jsize_t ListBox::GetPreferredSize()
{
	jsize_t t;

	t.width = _size.width;
	t.height = 2*(_vertical_gap+_border_size)+_items.size()*(_item_size+_vertical_gap)-_vertical_gap;

	return t;
}

bool ListBox::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	double diff = (_size.height-2*(_vertical_gap+_border_size)-4*(_scroll_width-2))/(double)(_items.size()-1);

	int x1 = event->GetX(),
			y1 = event->GetY();

	bool catched = false;

	if (event->GetType() == JME_PRESSED && event->GetButton() == JMB_BUTTON1) {
		catched = true;

		RequestFocus();
		
		if (_scroll == JLS_BAR) {
			if (x1 > (_location.x+_size.width-_scroll_width-_horizontal_gap+_border_size) && x1 < (_location.x+_size.width-_horizontal_gap+_border_size)) {
				_pressed = false;

				if (y1 < (_location.y+_vertical_gap+_border_size+_scroll_width+diff*_index)) {
					if (y1 < (_location.y+_vertical_gap+_border_size+_scroll_width)) {
						PreviousItem();
					} else {
						PreviousItem();
					}
				} else if (y1 > (_location.y+_vertical_gap+_border_size+_scroll_width+diff*_index+_stone_size)) {
					if (y1 > (_location.y+_size.height-_vertical_gap-_border_size-_scroll_width)) {
						NextItem();
					} else {
						NextItem();
					}
				} else {
					_pressed = true;
				}
			}
		}
	} else if (event->GetType() == JME_MOVED) {
		if (_pressed == true) {
			if (y1 < (_location.y+_item_size/2+diff*_index+_vertical_gap+_border_size)) {
				PreviousItem();
			} else if (y1 > (_location.y+_item_size/2+diff*_index+_vertical_gap+_border_size+_scroll_width)) {
				NextItem();
			}
		}
	} else {
		_pressed = false;
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

	bool catched = false;

	jkeyevent_symbol_t action = event->GetSymbol();

	if (action == JKS_CURSOR_UP || action == JKS_PAGE_UP) {
		PreviousItem();
		
		catched = true;
	} else if (action == JKS_CURSOR_DOWN || action == JKS_PAGE_DOWN) {
		NextItem();

		catched = true;
	} else if (action == JKS_ENTER) {
		SetSelected(_index);

		if (_items.size() > 0) { 
			DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_ACTION));
		}

		catched = true;
	}

	return catched;
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

	int count = 0,
			space = 4,
			scroll_width = 0,
			scroll_gap = 0;

	for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
		if ((*i)->GetType() == JMT_IMAGE) {
			space += _item_size + 8;

			break;
		}
	}

	if ((int)_items.size() < visible_items) {
		position = 0;
	}

	if (_scroll == JLS_BAR) {
		scroll_width = _scroll_width;
		scroll_gap = 5;
	}

	int i;

	for (i=position; count<visible_items+1 && i<(int)_items.size(); i++, count++) {
		g->SetColor(_item_color);

		if (_index != i) {
			if (_selection == JLM_SINGLE_SELECTION) {	
				if (_selected_index == i) {	
					g->SetColor(_selected_item_color);
				}
			} else if (_selection == JLM_MULTI_SELECTION) {	
				if (_items[i]->IsSelected() == true) {	
					g->SetColor(_selected_item_color);
				}
			}
		} else {
			g->SetColor(_focus_item_color);
		}

		if (count != visible_items) {
			g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w-scroll_width-scroll_gap, _item_size);
		} else {
			int ph = y+(_item_size+_vertical_gap)*count-y-h;

			g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w-scroll_width-scroll_gap, (ph>0)?ph-_vertical_gap:-ph);
		} 

		if (_index == i) {
			g->SetColor(_focus_item_color);
		
			if (count != visible_items) {
				g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w-scroll_width-scroll_gap, _item_size);
			} else {
				int ph = y+(_item_size+_vertical_gap)*count-y-h;

				g->FillRectangle(x, y+(_item_size+_vertical_gap)*count, w-scroll_width-scroll_gap, (ph>0)?ph-_vertical_gap:-ph);
			} 
		}

		g->SetColor(_item_color);

		if (_selection == JLM_SINGLE_SELECTION) {
			if (_selected_index == i) {
				g->SetColor(_selected_item_color);
			}
		} else if (_selection == JLM_MULTI_SELECTION) {	
			if (_items[i]->IsSelected() == true) {	
				g->SetColor(_selected_item_color);
			}
		}

		if (_items[i]->GetType() == JMT_EMPTY) {
		} else if (_items[i]->GetType() == JMT_TEXT) {
		} else if (_items[i]->GetType() == JMT_IMAGE) {
			if (_items[i]->GetImage() != NULL) {
				if (count != visible_items) {
					g->DrawImage(_items[i]->GetImage(), _horizontal_gap, y+(_item_size+_vertical_gap)*count, _item_size, _item_size);
				} else {
					jregion_t clip = g->GetClip();

					int cx = x,
							cy = y+(_item_size+_vertical_gap)*count,
							cw = w-scroll_width-scroll_gap,
							ch = ::abs(cy-y-h)-1;

					if (cx > clip.width) {
						cx = clip.width;
					}

					if (cy > clip.height) {
						cy = clip.height;
					}

					if (cw > (clip.width-cx)) {
						cw = clip.width-cx;
					}

					if (ch > (clip.height-cy)) {
						ch = clip.height-cy;
					}

					g->SetClip(cx, cy, cw, ch);

					g->DrawImage(_items[i]->GetImage(), _horizontal_gap, y+(_item_size+_vertical_gap)*count, _item_size, _item_size);
		
					g->SetClip(clip.x, clip.y, clip.width, clip.height);
				}
			}
		}

		if (IsFontSet() == true) {
			if (_has_focus == true) {
				g->SetColor(_focus_item_fgcolor);
			} else {
				g->SetColor(_item_fgcolor);
			}

			int px = x+space,
					py = y+(_item_size+_vertical_gap)*count,
					pw = (w-space-4)-scroll_width-scroll_gap,
					ph = _item_size-1;

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
				text = _font->TruncateString(text, "...", pw);
			// }

			if (count != visible_items) {
				g->DrawString(text, px, py, pw, ph, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
			} else {
				jregion_t clip = g->GetClip();

				int cx = px,
						cy = py,
						cw = pw,
						ch = ::abs(cy-y-h)-1;

				if (cx > clip.width) {
					cx = clip.width;
				}

				if (cy > clip.height) {
					cy = clip.height;
				}

				if (cw > (clip.width-cx)) {
					cw = clip.width-cx;
				}

				if (ch > (clip.height-cy)) {
					ch = clip.height-cy;
				}

				// INFO:: weird clip
				g->SetClip(cx, cy, cw, ch);
				// g->SetClip(cx, cy, cw, cy+ch);

				g->DrawString(text, px, py, pw, _item_size, _items[count]->GetHorizontalAlign(), _items[count]->GetVerticalAlign(), false);
				// g->DrawString(text, px, py, pw, _item_size, _items[count]->GetHorizontalAlign(), _items[count]->GetVerticalAlign());

				g->SetClip(clip.x, clip.y, clip.width, clip.height);
			}
		}
	}

	for (; count<visible_items+1; count++) {
		int dy = y+(_item_size+_vertical_gap)*count;
		
		g->SetColor(_item_color);

		if ((dy+_item_size) < (_size.height-dy)) {
			g->FillRectangle(x, dy, w-scroll_width-scroll_gap, _item_size);
		} else {
			g->FillRectangle(x, dy, w-scroll_width-scroll_gap, ((dy-y-h)>0)?(dy-y-h)-_vertical_gap:-(dy-y-h));
		}
	}
	
	if (_scroll == JLS_BAR) {
		Color color(0x80, 0x80, 0xe0, 0xff),
					disable = color.Darker(0.1);

		g->SetColor(_item_color);
		g->FillRectangle(_size.width-x-scroll_width, y, scroll_width, h);

		if ((_centered_interaction == true && _index > 0) | (_centered_interaction == false && _top_index > 0)) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		x = _size.width-x-scroll_width,
		
		g->FillTriangle(x, y+scroll_width, x+scroll_width/2, y, x+scroll_width, y+scroll_width);

		if ((_centered_interaction == true && (_index < (int)(_items.size()-1))) || (_centered_interaction == false && (_top_index+visible_items) < (int)(_items.size()))) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		g->FillTriangle(x, y+h-scroll_width, x+scroll_width, y+h-scroll_width, x+scroll_width/2, y+h);
		
		g->SetColor(color);

		if (visible_items <= (int)_items.size()) {
			double diff = (h-4*(scroll_width-2))/(double)(_items.size()-1);
			
			g->FillRectangle(x, (int)(y+scroll_width+diff*_index+2), scroll_width, _stone_size);
		}
	}

	PaintBorderEdges(g);
}

void ListBox::PreviousItem()
{
	if (_items.size() > 0) {
		int old_index = _index;

		_index--;
		// _index = _index - visible_items;

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

			DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_UP)); 
		}
	}
}

void ListBox::NextItem()
{
	int visible_items = GetVisibleItems();

	if (_items.size() > 0) { 
		int old_index = _index;

		_index++;
		// _index = _index + visible_items;

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

			DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_DOWN)); 
		}
	}
}

}
