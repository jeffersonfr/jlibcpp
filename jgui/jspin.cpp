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
#include "jspin.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

Spin::Spin(int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Spin");

	_index = 0;
	_loop = true;
	_type = HORIZONTAL_SPIN;
	// _type = VERTICAL_SPIN;
	
	SetFocusable(true);
	SetArrowsSize((_height-4)/2);
}

Spin::~Spin()
{
}

void Spin::SetArrowsSize(int size)
{
	if (size <= 0) {
		return;
	}

	_arrows_size = size;

	if (_type == HORIZONTAL_SPIN) {
		if (_arrows_size > (_width-4)/2) {
			_arrows_size = (_width-4)/2;
		}
	} else if (_type == VERTICAL_SPIN) {
		if (_arrows_size > (_height-8)/2) {
			_arrows_size = (_height-8)/2;
		}
	}

	if (_arrows_size < 2) {
		_arrows_size = 2;
	}

	Repaint();
}

int Spin::GetArrowsSize()
{
	return _arrows_size;
}

void Spin::SetType(jspin_type_t type)
{
	if (_type == type) {
		return;
	}

	_type = type;

	Repaint();
}

jspin_type_t Spin::GetType()
{
	return _type;
}

void Spin::SetLoop(bool b)
{
	_loop = b;
}

bool Spin::ProcessEvent(MouseEvent *event)
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

		if (_type == HORIZONTAL_SPIN) {
			if (y1 > _y && y1 < (_y+_height)) {
				if (x1 > (_x+_width-_arrows_size-dx) && x1 < (_x+_width-dx)) {
					_index++;

					if (_index >= (int)_list.size()) {
						if (_list.size() > 0) {
							_index = _list.size()-1;
						} else {
							_index = 0;
						}
					}

					if (_list.size() > 0) {
						Repaint();

						DispatchEvent(new SelectEvent(this, _list[_index], _index, RIGHT_ITEM));
					}
				} else if (x1 > (_x+dx) && x1 < (_x+dx+_arrows_size)) {
					_index--;

					if (_index < 0) {
						_index = 0;
					}

					if (_list.size() > 0) {
						Repaint();

						DispatchEvent(new SelectEvent(this, _list[_index], _index, LEFT_ITEM));
					}
				}
			}
		} else if (_type == VERTICAL_SPIN) {
			int py = (_height-8)/2;

			if (x1 > (_x+_width-2*_arrows_size-10) && x1 < (_x+_width-10)) {
				if (y1 > (_y) && y1 < (_y+py+dy)) {
					_index--;

					if (_index < 0) {
						_index = 0;
					}

					Repaint();

					DispatchEvent(new SelectEvent(this, _list[_index], _index, LEFT_ITEM));
				} else if (y1 > (_y+py+dy) && y1 < (_y+_height)) {
					_index++;

					if (_index >= (int)_list.size()) {
						if (_list.size() > 0) {
							_index = _list.size()-1;
						} else {
							_index = 0;
						}
					}

					Repaint();

					DispatchEvent(new SelectEvent(this, _list[_index], _index, RIGHT_ITEM));
				}
			}
		}

		/*
		if (action == JKEY_ENTER) {
			DispatchEvent(new SelectEvent(this, _list[_index], _index, ACTION_ITEM));
		}
		*/
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

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_CURSOR_LEFT) {
		if (_type == HORIZONTAL_SPIN) {
			_index--;

			if (_index < 0) {
				_index = 0;
			}

			if (_list.size() > 0) {
				Repaint();

				DispatchEvent(new SelectEvent(this, _list[_index], _index, LEFT_ITEM));
			}

			catched = true;
		}
	} else if (action == JKEY_CURSOR_RIGHT) {
		if (_type == HORIZONTAL_SPIN) {
			_index++;

			if (_index >= (int)_list.size()) {
				if (_list.size() > 0) {
					_index = _list.size()-1;
				} else {
					_index = 0;
				}
			}

			if (_list.size() > 0) {
				Repaint();

				DispatchEvent(new SelectEvent(this, _list[_index], _index, RIGHT_ITEM));
			}

			catched = true;
		}
	} else if (action == JKEY_CURSOR_UP) {
		if (_type == VERTICAL_SPIN) {
			_index--;

			if (_index < 0) {
				_index = 0;
			}

			Repaint();

			DispatchEvent(new SelectEvent(this, _list[_index], _index, LEFT_ITEM));

			catched = true;
		}
	} else if (action == JKEY_CURSOR_DOWN) {
		if (_type == VERTICAL_SPIN) {
			_index++;

			if (_index >= (int)_list.size()) {
				if (_list.size() > 0) {
					_index = _list.size()-1;
				} else {
					_index = 0;
				}
			}

			Repaint();

			DispatchEvent(new SelectEvent(this, _list[_index], _index, RIGHT_ITEM));

			catched = true;
		}
	} else if (action == JKEY_ENTER) {
		DispatchEvent(new SelectEvent(this, _list[_index], _index, ACTION_ITEM));

		catched = true;
	}

	return catched;
}

void Spin::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	{
		/*
		if (_has_focus == true) {
				g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
		}
		*/

		if (_type == HORIZONTAL_SPIN) {
			int dx = _width-_arrows_size-4,
				dy = (_height-2*_arrows_size)/2;

			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillTriangle(dx, dy+2, dx+_arrows_size, dy+_arrows_size, dx, dy+2*_arrows_size-4);

			dx = 4;

			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillTriangle(dx, dy+_arrows_size, dx+_arrows_size, dy+2, dx+_arrows_size, dy+2*_arrows_size-4);

			if (_loop == true) {
			} else {
			}

			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

			if (_list.size() > 0) {
				g->DrawString(TruncateString(_list[_index], _width-20), 5, (CENTER_VERTICAL_TEXT), _width-10, _height, CENTER_ALIGN);
			}
		} else if (_type == VERTICAL_SPIN) {
			int dx = _width-2*_arrows_size-10,
				dy = (_height-8)/2;

			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillTriangle(dx, dy+2, dx+_arrows_size, 2, dx+2*_arrows_size, dy+2);

			g->SetColor(0x80, 0x80, 0xe0, 0xff);
			g->FillTriangle(dx, dy+8, dx+2*_arrows_size, dy+8, dx+_arrows_size, _height);

			if (_loop == true) {
			} else {
			}

			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

			if (_list.size() > 0) {
				g->DrawString(TruncateString(_list[_index], _width-20), 10, (CENTER_VERTICAL_TEXT), _width-10, _height, LEFT_ALIGN);
			}
		}
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _width, _height);
	}
}

void Spin::SetIndex(int i)
{
	{
		jthread::AutoLock lock(&_component_mutex);

		_index = i;

		if (_index < 0) {
			_index = 0;
		}

		if (_index >= (int)_list.size()) {
			_index = _list.size()-1;
		}
	}

	Repaint();
}

void Spin::Add(std::string text)
{
	int length;

	length = _list.size();

	{
		jthread::AutoLock lock(&_component_mutex);

		_list.push_back(text);

		_index = 0;
	}

	if (length == 0) {
		Repaint();
	}
}

void Spin::Remove(int index)
{
	jthread::AutoLock lock(&_component_mutex);

	if (index < 0 || index >= (int)_list.size()) {
		return;
	}

	_list.erase(_list.begin()+index);
}

void Spin::RemoveAll()
{
	jthread::AutoLock lock(&_component_mutex);

	_list.clear();
}

std::string Spin::GetValue()
{
	return _list[_index];
}

int Spin::GetIndex()
{
	return _index;
}

void Spin::RegisterSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_select_listeners.begin(), _select_listeners.end(), listener) == _select_listeners.end()) {
		_select_listeners.push_back(listener);
	}
}

void Spin::RemoveSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<SelectListener *>::iterator i = std::find(_select_listeners.begin(), _select_listeners.end(), listener);

	if (i != _select_listeners.end()) {
		_select_listeners.erase(i);
	}
}

void Spin::DispatchEvent(SelectEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<SelectListener *>::iterator i=_select_listeners.begin(); i!=_select_listeners.end(); i++) {
		if (event->GetType() == ACTION_ITEM) {
			(*i)->ItemSelected(event);
		} else {
			(*i)->ItemChanged(event);
		}
	}

	delete event;
}

std::vector<SelectListener *> & Spin::GetSelectListeners()
{
	return _select_listeners;
}

}
