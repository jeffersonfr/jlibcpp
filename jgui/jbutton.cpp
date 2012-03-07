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
#include "jbutton.h"
#include "jbuttonlistener.h"
#include "jdebug.h"

namespace jgui {

Button::Button(std::string label, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Button");

	_wrap = false;
	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_name = label;
	_label = label;

	SetFocusable(true);
}

Button::~Button()
{
}

void Button::SetLabel(std::string label)
{
	_label = label;
}

std::string Button::GetLabel()
{
	return _label;
}

void Button::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t Button::GetHorizontalAlign()
{
	return _halign;
}

void Button::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t Button::GetVerticalAlign()
{
	return _valign;
}

bool Button::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	bool catched = false;

	if (event->GetType() == JMT_PRESSED && event->GetButton() == JMB_BUTTON1) {
		catched = true;

		DispatchButtonEvent(new ButtonEvent(this));
	}

	return catched;
}

bool Button::ProcessEvent(KeyEvent *event)
{
	if (IsEnabled() == false) {
		return false;
	}

	bool catched = false;

	if (event->GetSymbol() == JKS_ENTER) {
		DispatchButtonEvent(new ButtonEvent(this));

		catched = true;
	}

	return catched || Component::ProcessEvent(event);
}

void Button::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	/*
	if (_has_focus == true) {
		g->FillGradientRectangle(0, 0, _width, _height/2+1, 
			_bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
		g->FillGradientRectangle(0, _height/2, _width, _height/2, 
			_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	}
	*/

	if (_font != NULL) {
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
				gapx = 0,
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

		std::string text = GetLabel();

		if (_wrap == false) {
			text = _font->TruncateString(text, "...", pw);
		}

		g->DrawString(text, px, py, pw, ph, _halign, _valign);
	}
}

void Button::RegisterButtonListener(ButtonListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_button_listeners.begin(), _button_listeners.end(), listener) == _button_listeners.end()) {
		_button_listeners.push_back(listener);
	}
}

void Button::RemoveButtonListener(ButtonListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ButtonListener *>::iterator i = std::find(_button_listeners.begin(), _button_listeners.end(), listener);
	
	if (i != _button_listeners.end()) {
		_button_listeners.erase(i);
	}
}

void Button::DispatchButtonEvent(ButtonEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k = 0,
			size = (int)_button_listeners.size();

	while (k++ < (int)_button_listeners.size()) {
		_button_listeners[k-1]->ActionPerformed(event);

		if (size != (int)_button_listeners.size()) {
			size = (int)_button_listeners.size();

			k--;
		}
	}

	/*
	for (std::vector<ButtonListener *>::iterator i=_button_listeners.begin(); i!=_button_listeners.end(); i++) {
		(*i)->ActionPerformed(event);
	}
	*/

	delete event;
}

std::vector<ButtonListener *> & Button::GetButtonListeners()
{
	return _button_listeners;
}

}
