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
#include "jcheckbutton.h"
#include "jcheckbuttonlistener.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

CheckButton::CheckButton(jcheckbox_type_t type, std::string label, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::CheckButton");

	_type = type;
	_label = label;
	_checked = false;

	SetFocusable(true);
	SetSize(width, height);
}

CheckButton::~CheckButton()
{
}

void CheckButton::SetType(jcheckbox_type_t type)
{
	jthread::AutoLock lock(&_component_mutex);

	_type = type;
}

bool CheckButton::IsSelected()
{
	return _checked;
}

void CheckButton::SetSelected(bool b)
{
	if (_checked != b) {
		{
			jthread::AutoLock lock(&_component_mutex);

			_checked = b;
		}

		DispatchEvent(new CheckButtonEvent(this, _checked));

		Repaint();
	}
}

bool CheckButton::ProcessEvent(MouseEvent *event)
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
			size = _height;

		if (_height > _width) {
			size = _width;
		}

		RequestFocus();

		if ((x1 > _x && x1 < (_x+size)) && (y1 > _y && y1 < (_y+size))) {
			if (_type == CHECK_TYPE) {
				if (_checked == true) {
					SetSelected(false);
				} else {
					SetSelected(true);
				}
			} else {
				SetSelected(true);
			}
		}
	}

	return catched;
}

bool CheckButton::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_ENTER) {
		if (_type == CHECK_TYPE) {
			if (_checked == true) {
				SetSelected(false);
			} else {
				SetSelected(true);
			}
		} else {
			SetSelected(true);
		}

		catched = true;
	}

	return catched;
}

jcheckbox_type_t CheckButton::GetType()
{
	jthread::AutoLock lock(&_component_mutex);

	return _type;
}

void CheckButton::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int size = DEFAULT_COMPONENT_HEIGHT-2*_border_size,
			maxwh = std::min(_width, _height);

	if (size > maxwh) {
		size = maxwh;
	}

	int raio = size/2 - 4;

	if (raio < 0) {
		raio = 0;
	}

	if (IsFontSet() == true) {
		/*
		if (_has_focus == true) {
			g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
		}
		*/

		g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
		g->DrawString(TruncateString(_label, _width-(size+15)), size+10, (CENTER_VERTICAL_TEXT), _width, _height, LEFT_ALIGN);
	}

	g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

	int center = (_height-size)/2;

	if (_checked == true) {
		if (_type == CHECK_TYPE) {
			FillRectangle(g, 2, 2+center, size-4, size-4);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			FillRectangle(g, 6, 6+center, size-12, size-12);
			// g->DrawRectangle(2, 2, size-4, size-4);
		} else {
			g->FillCircle(size/2, size/2+center, raio);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			g->FillCircle(size/2, size/2+center, (int)(size*0.2));
			// g->DrawCircle(size/2, size/2, size/2);
		}
	} else {
		if (_type == CHECK_TYPE) {
			FillRectangle(g, 2, 2+center, size-4, size-4);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			// g->DrawRectangle(2, 2, size-4, size-4);
		} else {
			g->FillCircle(size/2, size/2+center, raio);
			g->SetColor(0x00, 0x00, 0x00, 0xff);
			// g->DrawCircle(size/2, size/2, size/2);
		}

	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _width, _height);
	}
}

void CheckButton::RegisterCheckButtonListener(CheckButtonListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_check_listeners.begin(), _check_listeners.end(), listener) == _check_listeners.end()) {
		_check_listeners.push_back(listener);
	}
}

void CheckButton::RemoveCheckButtonListener(CheckButtonListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<CheckButtonListener *>::iterator i = std::find(_check_listeners.begin(), _check_listeners.end(), listener);
	
	if (i != _check_listeners.end()) {
		_check_listeners.erase(i);
	}
}

void CheckButton::DispatchEvent(CheckButtonEvent *event)
{
	if (event == NULL) {
		return;
	}

	for (std::vector<CheckButtonListener *>::iterator i=_check_listeners.begin(); i!=_check_listeners.end(); i++) {
		(*i)->ButtonSelected(event);
	}

	delete event;
}

std::vector<CheckButtonListener *> & CheckButton::GetCheckButtonListeners()
{
	return _check_listeners;
}

}
