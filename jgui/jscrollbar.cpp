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
#include "jscrollbar.h"

#include <algorithm>

namespace jgui {

ScrollBar::ScrollBar(int x, int y, int width, int height):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ScrollBar");

	_stone_size = 40;
	_label_visible = true;
	_running = false;
	_position = 0.0;
	_old_position = 0.0;
	_count_paint = 0;
	_minimum_tick = 1;
	_maximum_tick = 10;
	_type = LEFT_RIGHT_SCROLL;
	// _type = BOTTOM_UP_SCROLL;
	
	SetFocusable(true);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetOrientation(jscroll_orientation_t type)
{
	if (_type == type) {
		return;
	}

	_type = type;

	Repaint(true);
}

jscroll_orientation_t ScrollBar::GetOrientation()
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

	if (_stone_size < 1) {
		_stone_size = 1;
	}

	Repaint();
}

int ScrollBar::GetStoneSize()
{
	return _stone_size;
}

double ScrollBar::GetPosition()
{
	return _position;
}

void ScrollBar::SetPosition(double i)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_old_position = _position;
		_position = i;

		if (_position < 0.0) {
			_position = 0.0;
		}

		if (_position > 100.0) {
			_position = 100.0;
		}

		jadjustment_type_t t = UNIT_INCREMENT;
		int diff = (int)(_position-_old_position);

		if (diff > _minimum_tick) {
			t = BLOCK_INCREMENT;
		} else if (diff < -_minimum_tick) {
			t = BLOCK_DECREMENT;
		} else if (diff > 0 && diff <= _minimum_tick) {
			t = UNIT_INCREMENT;
		} else if (diff < 0 && diff >= -_minimum_tick) {
			t = UNIT_DECREMENT;
		}

		DispatchAdjustmentEvent(new AdjustmentEvent(this, t, _position));
	}

	Repaint();
}

double ScrollBar::GetMinorTickSpacing()
{
	return _minimum_tick;
}

double ScrollBar::GetMajorTickSpacing()
{
	return _maximum_tick;
}

void ScrollBar::SetMinorTickSpacing(double i)
{
	_minimum_tick = i;

	if (_minimum_tick < 0.0) {
		_minimum_tick = 0.0;
	}

	if (_minimum_tick > 100.0) {
		_minimum_tick = 100.0;
	}
}

void ScrollBar::SetMajorTickSpacing(double i)
{
	_maximum_tick = i;

	if (_maximum_tick < 0.0) {
		_maximum_tick = 0.0;
	}

	if (_maximum_tick > 100.0) {
		_maximum_tick = 100.0;
	}
}

bool ScrollBar::ProcessEvent(MouseEvent *event)
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

		int x1 = event->GetX(),
			y1 = event->GetY(),
			dx = 2,
			dy = 2;

		RequestFocus();

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

		if (_type == LEFT_RIGHT_SCROLL) {
			if (y1 > _location.y && y1 < (_location.y+_size.height)) {
				double d = (_position*(_size.width-_stone_size-2*arrow_size-20))/100.0;

				if (x1 > (_location.x+dx) && x1 < (_location.x+arrow_size+dx)) {
					SetPosition(_position-_minimum_tick);
				} else if (x1 > (_location.x+_size.width-arrow_size-dx) && x1 < (_location.x+_size.width-dx)) {
					SetPosition(_position+_minimum_tick);
				} else if (x1 > (_location.x+arrow_size+dx) && x1 < (_location.x+arrow_size+dx+(int)d)) {
					SetPosition(_position-_maximum_tick);
				} else if (x1 > (_location.x+arrow_size+dx+(int)d+_stone_size) && x1 < (_location.x+_size.width-arrow_size)) {
					SetPosition(_position+_maximum_tick);
				}
			}
		} else if (_type == BOTTOM_UP_SCROLL) {
			if (x1 > _location.x && x1 < (_location.x+_size.width)) {
				double d = (_position*(_size.height-_stone_size-2*arrow_size-20))/100.0;

				if (y1 > (_location.y+dy) && y1 < (_location.y+arrow_size+dy)) {
					SetPosition(_position-_minimum_tick);
				} else if (y1 > (_location.y+_size.height-arrow_size-dy) && y1 < (_location.y+_size.height-dy)) {
					SetPosition(_position+_minimum_tick);
				} else if (y1 > (_location.y+arrow_size+dy) && y1 < (_location.y+arrow_size+dy+(int)d)) {
					SetPosition(_position-_maximum_tick);
				} else if (y1 > (_location.y+arrow_size+dy+(int)d+_stone_size) && y1 < (_location.y+_size.height-arrow_size)) {
					SetPosition(_position+_maximum_tick);
				}
			}
		}
	}

	return catched;
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
			SetPosition(_position-_minimum_tick);

			catched = true;
		} else if (action == JKEY_CURSOR_RIGHT) {
			SetPosition(_position+_minimum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_DOWN) {
			SetPosition(_position-_maximum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_UP) {
			SetPosition(_position+_maximum_tick);

			catched = true;
		}
	} else if (_type == BOTTOM_UP_SCROLL) {
		if (action == JKEY_CURSOR_UP) {
			SetPosition(_position-_minimum_tick);

			catched = true;
		} else if (action == JKEY_CURSOR_DOWN) {
			SetPosition(_position+_minimum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_DOWN) {
			SetPosition(_position-_maximum_tick);

			catched = true;
		} else if (action == JKEY_PAGE_UP) {
			SetPosition(_position+_maximum_tick);

			catched = true;
		}
	}

	return catched;
}

void ScrollBar::Paint(Graphics *g)
{
	// JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (_count_paint == 0) {
		_count_paint = 1;
	}

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

		double d = (_position*limit)/100.0;

		if (d > limit) {
			d = limit;
		}

		g->SetColor(color);
		FillRectangle(g, (int)d+arrow_size+x, y, _stone_size, h);

		g->FillTriangle(x+w, y+arrow_size, x+w-arrow_size, y, x+w-arrow_size, y+2*arrow_size);
		g->FillTriangle(x, y+arrow_size, x+arrow_size, y, x+arrow_size, y+2*arrow_size);
	} else if (_type == BOTTOM_UP_SCROLL) {
		int arrow_size = w/2,
				limit = h-_stone_size-2*arrow_size;

		double d = (_position*limit)/100.0;

		if (d > limit) {
			d = limit;
		}

		g->SetColor(color);
		FillRectangle(g, x, (int)d+arrow_size+y, w, _stone_size);

		g->FillTriangle(x, y+arrow_size, x+w/2, y,x+w, y+arrow_size);
		g->FillTriangle(x, y+h-arrow_size, x+w/2, y+h,x+w, y+h-arrow_size);
	}

	PaintEdges(g);
}

void ScrollBar::RegisterAdjustmentListener(AdjustmentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_adjust_listeners.begin(), _adjust_listeners.end(), listener) == _adjust_listeners.end()) {
		_adjust_listeners.push_back(listener);
	}
}

void ScrollBar::RemoveAdjustmentListener(AdjustmentListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<AdjustmentListener *>::iterator i = std::find(_adjust_listeners.begin(), _adjust_listeners.end(), listener);

	if (i != _adjust_listeners.end()) {
		_adjust_listeners.erase(i);
	}
}

void ScrollBar::DispatchAdjustmentEvent(AdjustmentEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_adjust_listeners.size()) {
		_adjust_listeners[k-1]->AdjustmentValueChanged(event);
	}

	/*
	for (std::vector<AdjustmentListener *>::iterator i=_adjust_listeners.begin(); i!=_adjust_listeners.end(); i++) {
		(*i)->AdjustmentValueChanged(event);
	}
	*/

	delete event;
}

std::vector<AdjustmentListener *> & ScrollBar::GetAdjustmentListeners()
{
	return _adjust_listeners;
}

}
