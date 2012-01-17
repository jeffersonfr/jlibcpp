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
#include "jslider.h"
#include "jdebug.h"

namespace jgui {

Slider::Slider(int x, int y, int width, int height):
   	SliderComponent(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Slider");

	_pressed = false;
	_stone_size = 30;
	_inverted = false;

	SetFocusable(true);
}

Slider::~Slider()
{
}

int Slider::GetStoneSize()
{
	return _stone_size;
}
		
void Slider::SetStoneSize(int size)
{
	_stone_size = size;

	if (_type == JSO_HORIZONTAL) {
		if (_stone_size > (_size.width-_horizontal_gap-_border_size)) {
			_stone_size = (_size.width-_horizontal_gap-_border_size);
		}
	} else if (_type == JSO_VERTICAL) {
		if (_stone_size > (_size.height-_vertical_gap-_border_size)) {
			_stone_size = (_size.height-_vertical_gap-_border_size);
		}
	}

	Repaint();
}

void Slider::SetInverted(bool b)
{
	if (_inverted == b) {
		return;
	}

	_inverted = b;

	Repaint();
}

bool Slider::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	int x1 = event->GetX(),
			y1 = event->GetY(),
			dx = _horizontal_gap+_border_size,
			dy = _vertical_gap+_border_size,
			dw = _size.width-2*dx-_stone_size,
			dh = _size.height-2*dy-_stone_size;

	bool catched = false;

	if (event->GetType() == JME_PRESSED && event->GetButton() == JMB_BUTTON1) {
		catched = true;

		if (_type == JSO_HORIZONTAL) {
			if (y1 > 0 && y1 < (_size.height)) {
				int d = (int)((_value*dw)/(GetMaximum()-GetMinimum()));

				_pressed = false;

				if (x1 > (dx) && x1 < (dx+d)) {
					SetValue(_value-_maximum_tick);
				} else if (x1 > (dx+d+_stone_size) && x1 < (_size.width)) {
					SetValue(_value+_maximum_tick);
				} else if (x1 > (dx+d) && x1 < (dx+d+_stone_size)) {
					_pressed = true;
				}
			}
		} else if (_type == JSO_VERTICAL) {
			if (x1 > 0 && x1 < (_size.width)) {
				int d = (int)((_value*dh)/(GetMaximum()-GetMinimum()));

				_pressed = false;

				if (y1 > (dy) && y1 < (dy+d)) {
					SetValue(_value-_maximum_tick);
				} else if (y1 > (dy+d+_stone_size) && y1 < (_size.height)) {
					SetValue(_value+_maximum_tick);
				}
			}
		}
	} else if (event->GetType() == JME_MOVED) {
		if (_pressed == true) {
			if (_type == JSO_HORIZONTAL) {
				SetValue((((GetMaximum()-GetMinimum())*(x1-_stone_size/2-GetX()))/dw));
			} else if (_type == JSO_VERTICAL) {
				SetValue((((GetMaximum()-GetMinimum())*(y1-_stone_size/2-GetY()))/dh));
			}
		}
	} else {
		_pressed = false;
	} 

	return catched;
}

bool Slider::ProcessEvent(KeyEvent *event)
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

void Slider::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Color color = _scrollbar_color,
				bar = color.Darker(0.1);

	int x = _vertical_gap-_border_size,
			y = _horizontal_gap-_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y;

	if (_type == JSO_HORIZONTAL) {
		int d = (int)((_value*(w-_stone_size))/(GetMaximum()-GetMinimum()));

		if (d > (w-_stone_size)) {
			d = w-_stone_size;
		}

		g->SetColor(bar);
		g->FillRectangle(x, (h-10)/2+y, w, 10);
		g->SetColor(color);

		if (_inverted == false) {
			jgui::jpoint_t p[] = {
				{0, 0},
				{_stone_size, 0},
				{_stone_size, (int)(h*0.4)},
				{_stone_size/2, h},
				{0, (int)(h*0.4)}
			};

			g->FillPolygon((int)d+x, y, p, 5);
		} else {
			jgui::jpoint_t p[] = {
				{_stone_size/2, 0},
				{_stone_size, (int)(h*0.6)},
				{_stone_size, h},
				{0, h},
				{0, (int)(h*0.6)}
			};

			g->FillPolygon((int)d+x, y, p, 5);
		}
	} else if (_type == JSO_VERTICAL) {
		int d = (int)((_value*(h-_stone_size))/(GetMaximum()-GetMinimum()));

		if (d > (h-_stone_size)) {
			d = h-_stone_size;
		}

		g->SetColor(bar);
		g->FillRectangle((w-10)/2+x, y, 10, h);
		g->SetColor(color);

		if (_inverted == false) {
			jgui::jpoint_t p[] = {
				{0, 0},
				{(int)(_size.width*0.4), 0},
				{w, _stone_size/2},
				{(int)(_size.width*0.4), _stone_size},
				{0, _stone_size}
			};

			g->FillPolygon(x, (int)d+y, p, 5);
		} else {
			jgui::jpoint_t p[] = {
				{0, _stone_size/2},
				{(int)(_size.width*0.6), 0},
				{w, 0},
				{w, _stone_size},
				{(int)(_size.width*0.6), _stone_size}
			};

			g->FillPolygon(x, (int)d+y, p, 5);
		}
	}
}

}
