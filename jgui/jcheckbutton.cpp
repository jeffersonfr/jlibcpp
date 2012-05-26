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
#include "jcheckbutton.h"
#include "jcheckbuttonlistener.h"
#include "jdebug.h"

namespace jgui {

CheckButton::CheckButton(jcheckbox_type_t type, std::string text, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::CheckButton");

	_halign = JHA_LEFT;
	_valign = JVA_CENTER;

	_type = type;
	_text = text;
	_checked = false;
	_wrap = false;

	SetFocusable(true);
}

CheckButton::~CheckButton()
{
}

void CheckButton::SetText(std::string text)
{
	_text = text;

	Repaint();
}

std::string CheckButton::GetText()
{
	return _text;
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

		DispatchCheckButtonEvent(new CheckButtonEvent(this, _checked));

		Repaint();
	}
}

void CheckButton::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t CheckButton::GetHorizontalAlign()
{
	return _halign;
}

void CheckButton::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t CheckButton::GetVerticalAlign()
{
	return _valign;
}
		
bool CheckButton::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	bool catched = false;

	if (event->GetType() == JMT_PRESSED && event->GetButton() == JMB_BUTTON1) {
		catched = true;

		int x1 = event->GetX(),
				y1 = event->GetY(),
				size = _size.height;

		if (_size.height > _size.width) {
			size = _size.width;
		}

		if ((x1 > 0 && x1 < (size)) && (y1 > 0 && y1 < (size))) {
			if (_type == JCBT_CHECK) {
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
	if (IsEnabled() == false) {
		return false;
	}

	bool catched = false;

	jkeyevent_symbol_t action = event->GetSymbol();

	if (action == JKS_ENTER) {
		if (_type == JCBT_CHECK) {
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

	return catched || Component::ProcessEvent(event);
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

	int cw = _size.height/2,
			ch = _size.height/2,
			cx = 2*_horizontal_gap+_border_size,
			cy = (_size.height-ch)/2,
			csize = ch/2;

	if (csize < 0) {
		csize = 0;
	}

	if (cw > ch) {
		cw = ch;
	}
	
	/*
	if (_has_focus == true) {
		g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
		g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	}
	*/

	g->SetColor(0xf0, 0xf0, 0xf0, 0xff);

	if (_type == JCBT_CHECK) {
		g->FillRectangle(cx, cy, cw, ch);
	} else {
		g->FillCircle(cx+csize, _size.height/2, csize);
	}

	if (_checked == true) {
		g->SetColor(0x00, 0x00, 0x00, 0xff);

		if (_type == JCBT_CHECK) {
			g->FillRectangle(cx+4, cy+4, cw-8, ch-8);
		} else {
			g->FillCircle(cx+csize, _size.height/2, csize/2);
		}
	}

	if (IsFontSet() == true) {
		if (_is_enabled == true) {
			if (_has_focus == true) {
				g->SetColor(_focus_fgcolor);
			} else {
				g->SetColor(_fgcolor);
			}
		} else {
			g->SetColor(_disabled_fgcolor);
		}

		int x = _horizontal_gap+_border_size,
				y = _vertical_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				gapx = cx+cw+5,
				gapy = 0;
		int px = x+gapx,
				py = y+gapy,
				pw = w-gapx,
				ph = h-gapy;

		x = (x < 0)?0:x;
		y = (y < 0)?0:y;
		w = (w < 0)?0:w;
		h = (h < 0)?0:h;

		px = (px < 0)?0:px;
		py = (py < 0)?0:py;
		pw = (pw < 0)?0:pw;
		ph = (ph < 0)?0:ph;

		std::string text = GetText();

		if (_wrap == false) {
			text = _font->TruncateString(text, "...", pw);
		}

		g->DrawString(text, px, py, pw, ph, _halign, _valign);
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

void CheckButton::DispatchCheckButtonEvent(CheckButtonEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_check_listeners.size();

	while (k++ < (int)_check_listeners.size() && event->IsConsumed() == false) {
		_check_listeners[k-1]->ButtonSelected(event);

		if (size != (int)_check_listeners.size()) {
			size = (int)_check_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<CheckButtonListener *>::iterator i=_check_listeners.begin(); i!=_check_listeners.end(); i++) {
		(*i)->ButtonSelected(event);
	}
	*/

	delete event;
}

std::vector<CheckButtonListener *> & CheckButton::GetCheckButtonListeners()
{
	return _check_listeners;
}

}
