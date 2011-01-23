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

void ScrollBar::SetStoneSize(int size)
{
	int x = _vertical_gap-_border_size,
			y = _horizontal_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y,
			arrow_size;

	if (_type == LEFT_RIGHT_SCROLL) {
		arrow_size = h/2;
	} else {
		arrow_size = w/2;
	}

	_stone_size = size;

	if (_type == LEFT_RIGHT_SCROLL) {
		if (_stone_size > (_size.width-2*arrow_size)/2) {
			_stone_size = (_size.width-2*arrow_size)/2;
		}
	} else if (_type == BOTTOM_UP_SCROLL) {
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
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (_type == LEFT_RIGHT_SCROLL) {
		if (action == JKEY_CURSOR_LEFT) {
			SetValue(_value-_minimum_tick);

			catched = true;
		} else if (action == JKEY_CURSOR_RIGHT) {
			SetValue(_value+_minimum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_DOWN) {
			SetValue(_value-_maximum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_UP) {
			SetValue(_value+_maximum_tick);

			catched = true;
		}
	} else if (_type == BOTTOM_UP_SCROLL) {
		if (action == JKEY_CURSOR_UP) {
			SetValue(_value-_minimum_tick);

			catched = true;
		} else if (action == JKEY_CURSOR_DOWN) {
			SetValue(_value+_minimum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_DOWN) {
			SetValue(_value-_maximum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_UP) {
			SetValue(_value+_maximum_tick);

			catched = true;
		}
	}

	return catched;
}

bool ScrollBar::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	int arrow_size,
			x1 = event->GetX(),
			y1 = event->GetY(),
			dx = _vertical_gap-_border_size,
			dy = _horizontal_gap-_border_size,
			dw = _size.width-2*dx-_stone_size,
			dh = _size.height-2*dy-_stone_size;

	bool catched = false;

	if (event->GetType() == JMOUSE_PRESSED_EVENT && event->GetButton() == JMOUSE_BUTTON1) {
		catched = true;

		RequestFocus();

		if (_type == LEFT_RIGHT_SCROLL) {
			arrow_size = dh/2;
		} else {
			arrow_size = dw/2;
		}

		if (_type == LEFT_RIGHT_SCROLL) {
			if (y1 > _location.y && y1 < (_location.y+_size.height)) {
				int d = (int)((_value*(dw-2*arrow_size))/(GetMaximum()-GetMinimum()));

				_pressed = false;

				if (x1 > (_location.x+dx) && x1 < (_location.x+arrow_size+dx)) {
					SetValue(_value-_minimum_tick);
				} else if (x1 > (_location.x+_size.width-arrow_size-dx) && x1 < (_location.x+_size.width-dx)) {
					SetValue(_value+_minimum_tick);
				} else if (x1 > (_location.x+arrow_size+dx) && x1 < (_location.x+arrow_size+dx+d)) {
					SetValue(_value-_maximum_tick);
				} else if (x1 > (_location.x+arrow_size+dx+d+_stone_size) && x1 < (_location.x+_size.width-arrow_size)) {
					SetValue(_value+_maximum_tick);
				} else if (x1 > (_location.x+arrow_size+dx+d) && x1 < (_location.x+arrow_size+dx+d+_stone_size)) {
					_pressed = true;
				}
			}
		} else if (_type == BOTTOM_UP_SCROLL) {
			if (x1 > _location.x && x1 < (_location.x+_size.width)) {
				int d = (int)((_value*(dh-2*arrow_size))/(GetMaximum()-GetMinimum()));

				_pressed = false;

				if (y1 > (_location.y+dy) && y1 < (_location.y+arrow_size+dy)) {
					SetValue(_value-_minimum_tick);
				} else if (y1 > (_location.y+_size.height-arrow_size-dy) && y1 < (_location.y+_size.height-dy)) {
					SetValue(_value+_minimum_tick);
				} else if (y1 > (_location.y+arrow_size+dy) && y1 < (_location.y+arrow_size+dy+d)) {
					SetValue(_value-_maximum_tick);
				} else if (y1 > (_location.y+arrow_size+dy+d+_stone_size) && y1 < (_location.y+_size.height-arrow_size)) {
					SetValue(_value+_maximum_tick);
				} else if (y1 > (_location.y+arrow_size+dy+d) && y1 < (_location.y+arrow_size+dy+d+_stone_size)) {
					_pressed = true;
				}
			}
		}
	} else if (event->GetType() == JMOUSE_MOVED_EVENT) {
		if (_pressed == true) {
			if (_type == LEFT_RIGHT_SCROLL) {
				SetValue((((GetMaximum()-GetMinimum())*(x1-_stone_size/2-GetX()))/dw));
			} else if (_type == BOTTOM_UP_SCROLL) {
				SetValue((((GetMaximum()-GetMinimum())*(y1-_stone_size/2-GetY()))/dh));
			}
		}
	} else {
		_pressed = false;
	}

	return catched;
}

void ScrollBar::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	jcolor_t color;

	color.red = 0x80;
	color.green = 0x80;
	color.blue = 0xe0;
	color.alpha = 0xff;
	
	int x = _vertical_gap-_border_size,
			y = _horizontal_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	if (_type == LEFT_RIGHT_SCROLL) {
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
	} else if (_type == BOTTOM_UP_SCROLL) {
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

	PaintBorderEdges(g);
}

}
