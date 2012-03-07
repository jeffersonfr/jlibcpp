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
#include "jscrollbar.h"
#include "jdebug.h"

namespace jgui {

ScrollBar::ScrollBar(int x, int y, int width, int height):
	SliderComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ScrollBar");

	_pressed = false;
	_stone_size = 40;
	_label_visible = true;
	
	SetFocusable(true);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetScrollOrientation(jscroll_orientation_t type)
{
	if (_type == type) {
		return;
	}

	_type = type;

	Repaint();
}

jscroll_orientation_t ScrollBar::GetScrollOrientation()
{
	return _type;
}

void ScrollBar::SetStoneSize(int size)
{
	int x = _vertical_gap-_border_size,
			y = _horizontal_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y,
			arrow_size;

	if (_type == JSO_HORIZONTAL) {
		arrow_size = h/2;
	} else {
		arrow_size = w/2;
	}

	_stone_size = size;

	if (_type == JSO_HORIZONTAL) {
		if (_stone_size > (_size.width-2*arrow_size)/2) {
			_stone_size = (_size.width-2*arrow_size)/2;
		}
	} else if (_type == JSO_VERTICAL) {
		if (_stone_size > (_size.height-2*arrow_size)/2) {
			_stone_size = (_size.height-2*arrow_size)/2;
		}
	}

	Repaint();
}

int ScrollBar::GetStoneSize()
{
	return _stone_size;
}

bool ScrollBar::ProcessEvent(KeyEvent *event)
{
	if (IsEnabled() == false) {
		return false;
	}

	jkeyevent_symbol_t action = event->GetSymbol();

	bool catched = false;

	if (_type == JSO_HORIZONTAL) {
		if (action == JKS_CURSOR_LEFT) {
			SetValue(_value-_minimum_tick);

			catched = true;
		} else if (action == JKS_CURSOR_RIGHT) {
			SetValue(_value+_minimum_tick);

			catched = true;
		} else if (action == JKS_PAGE_DOWN) {
			SetValue(_value-_maximum_tick);

			catched = true;
		} else if (action == JKS_PAGE_UP) {
			SetValue(_value+_maximum_tick);

			catched = true;
		}
	} else if (_type == JSO_VERTICAL) {
		if (action == JKS_CURSOR_UP) {
			SetValue(_value-_minimum_tick);

			catched = true;
		} else if (action == JKS_CURSOR_DOWN) {
			SetValue(_value+_minimum_tick);

			catched = true;
		} else if (action == JKS_PAGE_DOWN) {
			SetValue(_value-_maximum_tick);

			catched = true;
		} else if (action == JKS_PAGE_UP) {
			SetValue(_value+_maximum_tick);

			catched = true;
		}
	}

	return catched || Component::ProcessEvent(event);
}

bool ScrollBar::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	int x = _horizontal_gap-_border_size,
			y = _vertical_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;
	int arrow_size,
			x1 = event->GetX(),
			y1 = event->GetY(),
			dx = _vertical_gap-_border_size,
			dy = _horizontal_gap-_border_size,
			dw = _size.width-2*dx-_stone_size,
			dh = _size.height-2*dy-_stone_size;
	bool catched = false;

	if (_type == JSO_HORIZONTAL) {
		arrow_size = h/2;
	} else {
		arrow_size = w/2;
	}

	if (event->GetType() == JMT_PRESSED) {
		if (event->GetButton() != JMB_BUTTON1) {
			return false;
		}

		catched = true;

		if (_type == JSO_HORIZONTAL) {
			if (y1 > 0 && y1 < (_size.height)) {
				int d = (int)((_value*(dw-2*arrow_size))/(GetMaximum()-GetMinimum()));

				_pressed = false;

				if (x1 > (dx) && x1 < (arrow_size+dx)) {
					SetValue(_value-_minimum_tick);
				} else if (x1 > (_size.width-arrow_size-dx) && x1 < (_size.width-dx)) {
					SetValue(_value+_minimum_tick);
				} else if (x1 > (arrow_size+dx) && x1 < (arrow_size+dx+d)) {
					SetValue(_value-_maximum_tick);
				} else if (x1 > (arrow_size+dx+d+_stone_size) && x1 < (_size.width-arrow_size)) {
					SetValue(_value+_maximum_tick);
				} else if (x1 > (arrow_size+dx+d) && x1 < (arrow_size+dx+d+_stone_size)) {
					_pressed = true;
				}
			}
		} else if (_type == JSO_VERTICAL) {
			if (x1 > 0 && x1 < (_size.width)) {
				int d = (int)((_value*(dh-2*arrow_size))/(GetMaximum()-GetMinimum()));

				_pressed = false;

				if (y1 > (dy) && y1 < (arrow_size+dy)) {
					SetValue(_value-_minimum_tick);
				} else if (y1 > (_size.height-arrow_size-dy) && y1 < (_size.height-dy)) {
					SetValue(_value+_minimum_tick);
				} else if (y1 > (arrow_size+dy) && y1 < (arrow_size+dy+d)) {
					SetValue(_value-_maximum_tick);
				} else if (y1 > (arrow_size+dy+d+_stone_size) && y1 < (_size.height-arrow_size)) {
					SetValue(_value+_maximum_tick);
				} else if (y1 > (arrow_size+dy+d) && y1 < (arrow_size+dy+d+_stone_size)) {
					_pressed = true;
				}
			}
		}
	} else if (event->GetType() == JMT_MOVED) {
		if (_pressed == true) {
			int diff = GetMaximum()-GetMinimum();

			if (_type == JSO_HORIZONTAL) {
				SetValue(diff*(x1-_stone_size/2-arrow_size)/(dw-2*arrow_size));
			} else if (_type == JSO_VERTICAL) {
				SetValue(diff*(y1-_stone_size/2-arrow_size)/(dh-2*arrow_size));
			}
		}
	} else {
		_pressed = false;

		if (event->GetType() == JMT_ROTATED) {
			SetValue(GetValue()+_minimum_tick*event->GetClickCount());
		}
	}

	return catched;
}

void ScrollBar::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Color color = _scrollbar_color;
	
	int x = _horizontal_gap-_border_size,
			y = _vertical_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	if (_type == JSO_HORIZONTAL) {
		int arrow_size = h/2,
				limit = w-_stone_size-2*arrow_size;

		double d = (_value*limit)/(GetMaximum()-GetMinimum());

		if (d > limit) {
			d = limit;
		}

		g->SetColor(color);
		g->FillRectangle((int)d+arrow_size+x, y, _stone_size, h);

		g->FillTriangle(x+w, y+arrow_size, x+w-arrow_size, y, x+w-arrow_size, y+2*arrow_size);
		g->FillTriangle(x, y+arrow_size, x+arrow_size, y, x+arrow_size, y+2*arrow_size);
	} else if (_type == JSO_VERTICAL) {
		int arrow_size = w/2,
				limit = h-_stone_size-2*arrow_size;

		double d = (_value*limit)/(GetMaximum()-GetMinimum());

		if (d > limit) {
			d = limit;
		}

		g->SetColor(color);
		g->FillRectangle(x, (int)d+arrow_size+y, w, _stone_size);

		g->FillTriangle(x, y+arrow_size, x+w/2, y,x+w, y+arrow_size);
		g->FillTriangle(x, y+h-arrow_size, x+w/2, y+h,x+w, y+h-arrow_size);
	}
}

}
