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
	_is_wrap = false;

	SetFocusable(true);
}

CheckButton::~CheckButton()
{
}

void CheckButton::SetWrap(bool b)
{
	if (_is_wrap == b) {
		return;
	}

	_is_wrap = b;

	Repaint();
}

bool CheckButton::IsWrap()
{
	return _is_wrap;
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
	_type = type;
}

bool CheckButton::IsSelected()
{
	return _checked;
}

void CheckButton::SetSelected(bool b)
{
	if (_checked != b) {
		_checked = b;

		Repaint();

		DispatchToggleEvent(new ToggleEvent(this, _checked));
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
		
bool CheckButton::KeyPressed(KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
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

	return catched;
}

bool CheckButton::MousePressed(MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	if (event->GetButton() == JMB_BUTTON1) {
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

		return true;
	}

	return false;
}

bool CheckButton::MouseReleased(MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool CheckButton::MouseMoved(MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool CheckButton::MouseWheel(MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}

	return false;
}

jcheckbox_type_t CheckButton::GetType()
{
	return _type;
}

void CheckButton::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	Theme *theme = GetTheme();
	jgui::Font *font = theme->GetFont("component");
	Color bg = theme->GetColor("component.bg");
	Color fg = theme->GetColor("component.fg");
	Color fgfocus = theme->GetColor("component.fg.focus");
	Color fgdisable = theme->GetColor("component.fg.disable");
	int bordersize = theme->GetBorderSize("component");

	int cw = _size.height/2,
			ch = _size.height/2,
			cx = 2*_horizontal_gap+bordersize,
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

	if (_has_focus == true) {
		g->SetColor(fgfocus);
	} else {
		g->SetColor(fg);
	}

	if (_type == JCBT_CHECK) {
		g->FillRectangle(cx, cy, cw, ch);
	} else {
		g->FillCircle(cx+csize, _size.height/2, csize);
	}

	if (_checked == true) {
		if (_has_focus == true) {
			g->SetColor(fg);
		} else {
			g->SetColor(fgfocus);
		}

		if (_type == JCBT_CHECK) {
			g->FillRectangle(cx+4, cy+4, cw-8, ch-8);
		} else {
			g->FillCircle(cx+csize, _size.height/2, csize/2);
		}
	}

	if (font != NULL) {
		g->SetFont(font);

		if (_is_enabled == true) {
			if (_has_focus == true) {
				g->SetColor(fgfocus);
			} else {
				g->SetColor(fg);
			}
		} else {
			g->SetColor(fgdisable);
		}

		int x = _horizontal_gap+bordersize,
				y = _vertical_gap+bordersize,
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

		if (_is_wrap == false) {
			text = font->TruncateString(text, "...", pw);
		}

		g->DrawString(text, px, py, pw, ph, _halign, _valign);
	}
}

void CheckButton::RegisterToggleListener(ToggleListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_check_listener_mutex);

	if (std::find(_check_listeners.begin(), _check_listeners.end(), listener) == _check_listeners.end()) {
		_check_listeners.push_back(listener);
	}
}

void CheckButton::RemoveToggleListener(ToggleListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_check_listener_mutex);

	std::vector<ToggleListener *>::iterator i = std::find(_check_listeners.begin(), _check_listeners.end(), listener);
	
	if (i != _check_listeners.end()) {
		_check_listeners.erase(i);
	}
}

void CheckButton::DispatchToggleEvent(ToggleEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<ToggleListener *> listeners;
	
	_check_listener_mutex.Lock();

	listeners = _check_listeners;

	_check_listener_mutex.Unlock();

	for (std::vector<ToggleListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		ToggleListener *listener = (*i);

		listener->StateChanged(event);
	}

	delete event;
}

std::vector<ToggleListener *> & CheckButton::GetToggleListeners()
{
	return _check_listeners;
}

}
