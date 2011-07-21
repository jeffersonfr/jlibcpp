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
#include "jspin.h"
#include "jdebug.h"

namespace jgui {

Spin::Spin(int x, int y, int width, int height):
	ItemComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Spin");

	_type = JSO_HORIZONTAL;
	// _type = JSO_VERTICAL;
	
	SetFocusable(true);
}

Spin::~Spin()
{
}

void Spin::SetScrollOrientation(jscroll_orientation_t type)
{
	if (_type == type) {
		return;
	}

	_type = type;

	Repaint(true);
}

jscroll_orientation_t Spin::GetScrollOrientation()
{
	return _type;
}

void Spin::NextItem()
{
	_index++;

	if (_index >= (int)_items.size()) {
		_index = _items.size()-1;

		if (_loop == true) {
			_index = 0;
		}
	}

	Repaint();

	DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_RIGHT));
}

void Spin::PreviousItem()
{
	_index--;

	if (_index < 0) {
		_index = 0;

		if (_loop == true) {
			_index = _items.size()-1;
		}
	}

	Repaint();

	DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_LEFT));
}

bool Spin::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	if (_items.size() == 0) {
		return true;
	}

	bool catched = false;

	if (event->GetType() == JME_PRESSED && event->GetButton() == JMB_BUTTON1) {
		catched = true;

		int x1 = event->GetX(),
				y1 = event->GetY();
		int x = _vertical_gap+_border_size,
				y = _horizontal_gap+_border_size,
				// w = _size.width-2*x,
				h = _size.height-2*y,
				arrow_size;

		if (_type == JSO_HORIZONTAL) {
			arrow_size = h/2;
		} else {
			arrow_size = (h-8)/2;
		}

		RequestFocus();

		if (_type == JSO_HORIZONTAL) {
			if (y1 > (_location.y+y) && y1 < (_location.y+y+_size.height)) {
				if (x1 > (_location.x+_size.width-arrow_size-x) && x1 < (_location.x+_size.width-x)) {
					NextItem();
				} else if (x1 > (_location.x+x) && x1 < (_location.x+x+arrow_size)) {
					PreviousItem();
				}
			}
		} else if (_type == JSO_VERTICAL) {
			if (x1 > (_location.x+_size.width-2*arrow_size-x) && x1 < (_location.x+_size.width-x)) {
				if (y1 > (_location.y+y) && y1 < (_location.y+h/2)) {
					PreviousItem();
				} else if (y1 > (_location.y+y+h/2) && y1 < (_location.y+y+h)) {
					NextItem();
				}
			}
		}
	}

	return catched;
}

bool Spin::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	if (_items.size() == 0) {
		return true;
	}

	bool catched = false;

	jkeyevent_symbol_t action = event->GetSymbol();

	if (action == JKS_CURSOR_LEFT) {
		if (_type == JSO_HORIZONTAL) {
			PreviousItem();

			catched = true;
		}
	} else if (action == JKS_CURSOR_RIGHT) {
		if (_type == JSO_HORIZONTAL) {
			NextItem();

			catched = true;
		}
	} else if (action == JKS_CURSOR_UP) {
		if (_type == JSO_VERTICAL) {
			PreviousItem();

			catched = true;
		}
	} else if (action == JKS_CURSOR_DOWN) {
		if (_type == JSO_VERTICAL) {
			NextItem();

			catched = true;
		}
	} else if (action == JKS_ENTER) {
		DispatchSelectEvent(new SelectEvent(this, _items[_index], _index, JST_ACTION));

		catched = true;
	}

	return catched;
}

void Spin::AddEmptyItem()
{
	Item *item = new Item();

	if (_type == JSO_HORIZONTAL) {
		item->SetHorizontalAlign(JHA_CENTER);
	}
		
	AddInternalItem(item);
	AddItem(item);
}

void Spin::AddTextItem(std::string text)
{
	Item *item = new Item(text);

	if (_type == JSO_HORIZONTAL) {
		item->SetHorizontalAlign(JHA_CENTER);
	}
		
	AddInternalItem(item);
	AddItem(item);
}

void Spin::AddImageItem(std::string text, std::string image)
{
	Item *item = new Item(text, image);

	if (_type == JSO_HORIZONTAL) {
		item->SetHorizontalAlign(JHA_CENTER);
	}
		
	AddInternalItem(item);
	AddItem(item);
}

void Spin::AddCheckedItem(std::string text, bool checked)
{
	Item *item = new Item(text, checked);

	if (_type == JSO_HORIZONTAL) {
		item->SetHorizontalAlign(JHA_CENTER);
	}
		
	AddInternalItem(item);
	AddItem(item);
}

void Spin::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Color color(0x80, 0x80, 0xe0, 0xff),
				disable = color.Darker(0.1);

	/*
	if (_has_focus == true) {
		g->FillGradientRectangle(0, 0, _size.width, _size.height/2+1, 
			_bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
		g->FillGradientRectangle(0, _size.height/2, _size.width, _size.height/2, 
			_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	}
	*/

	int x = _vertical_gap-_border_size,
			y = _horizontal_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y,
			arrow_size;

	if (_type == JSO_HORIZONTAL) {
		arrow_size = h/2;
	} else if (_type == JSO_VERTICAL) {
		arrow_size = (h-8)/2;
	}

	if (_type == JSO_HORIZONTAL) {
		if (_loop == true || (_index < ((int)_items.size()-1))) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		g->FillTriangle(x+w, y+arrow_size, x+w-arrow_size, y, x+w-arrow_size, y+2*arrow_size);

		if (_loop == true || (_index > 0 && _items.size() > 0)) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		g->FillTriangle(x, y+arrow_size, x+arrow_size, y, x+arrow_size, y+2*arrow_size);

		if (_items.size() > 0) {
			if (_font != NULL) {
				if (_has_focus == true) {
					g->SetColor(_focus_fgcolor);
				} else {
					g->SetColor(_fgcolor);
				}

				int x = _horizontal_gap+_border_size,
						y = _vertical_gap+_border_size,
						w = _size.width-2*x,
						h = _size.height-2*y,
						gapx = arrow_size+4,
						gapy = 0;
				int px = x+gapx,
						py = y+gapy,
						pw = w-2*gapx,
						ph = h-gapy;

				x = (x < 0)?0:x;
				y = (y < 0)?0:y;
				w = (w < 0)?0:w;
				h = (h < 0)?0:h;

				px = (px < 0)?0:px;
				py = (py < 0)?0:py;
				pw = (pw < 0)?0:pw;
				ph = (ph < 0)?0:ph;

				std::string text = _items[_index]->GetValue();

				// if (_wrap == false) {
					text = _font->TruncateString(text, "...", w);
				// }

				g->DrawString(text, px, py, pw, ph, _items[_index]->GetHorizontalAlign(), _items[_index]->GetVerticalAlign());
			}
		}
	} else if (_type == JSO_VERTICAL) {
		if (_loop == true || (_index < ((int)_items.size()-1))) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		g->FillTriangle(_size.width-2*arrow_size-x, y+arrow_size, _size.width-x, y+arrow_size, _size.width-arrow_size-x, y);

		if (_loop == true || (_index > 0 && _items.size() > 0)) {
			g->SetColor(color);
		} else {
			g->SetColor(disable);
		}

		g->FillTriangle(_size.width-2*arrow_size-x, y+arrow_size+8, _size.width-x, y+arrow_size+8, _size.width-arrow_size-x, y+h);

		if (_items.size() > 0) {
			if (_font != NULL) {
				if (_has_focus == true) {
					g->SetColor(_focus_fgcolor);
				} else {
					g->SetColor(_fgcolor);
				}

				int x = _horizontal_gap+_border_size,
						y = _vertical_gap+_border_size,
						w = _size.width-2*x,
						h = _size.height-2*y,
						gapx = 0,
						gapy = 0;
				int px = x+gapx,
						py = y+gapy,
						pw = w-2*gapx,
						ph = h-gapy;

				x = (x < 0)?0:x;
				y = (y < 0)?0:y;
				w = (w < 0)?0:w;
				h = (h < 0)?0:h;

				px = (px < 0)?0:px;
				py = (py < 0)?0:py;
				pw = (pw < 0)?0:pw;
				ph = (ph < 0)?0:ph;

				std::string text = _items[_index]->GetValue();

				// if (_wrap == false) {
					text = _font->TruncateString(text, "...", w);
				// }

				g->DrawString(text, px, py, pw, ph, _items[_index]->GetHorizontalAlign(), _items[_index]->GetVerticalAlign());
			}
		}
	}

	PaintBorderEdges(g);
}

}

