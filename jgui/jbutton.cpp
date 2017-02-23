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
#include "jdebug.h"

namespace jgui {

Button::Button(std::string label, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Button");

	_is_wrap = false;
	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_name = label;
	_label = label;

	SetFocusable(true);
}

Button::~Button()
{
}

void Button::SetWrap(bool b)
{
	if (_is_wrap == b) {
		return;
	}

	_is_wrap = b;

	Repaint();
}

bool Button::IsWrap()
{
	return _is_wrap;
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

bool Button::KeyPressed(KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	bool catched = false;

	if (event->GetSymbol() == JKS_ENTER) {
		DispatchActionEvent(new ActionEvent(this));

		catched = true;
	}

	return catched;
}

bool Button::MousePressed(MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	if (event->GetButton() == JMB_BUTTON1) {
		DispatchActionEvent(new ActionEvent(this));

		return true;
	}

	return false;
}

bool Button::MouseReleased(MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool Button::MouseMoved(MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool Button::MouseWheel(MouseEvent *event)
{
	if (Component::MouseWheel(event) == true) {
		return true;
	}

	return false;
}

void Button::Paint(Graphics *g)
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

	/*
	if (_has_focus == true) {
		g->FillGradientRectangle(0, 0, _width, _height/2+1, 
			_bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
		g->FillGradientRectangle(0, _height/2, _width, _height/2, 
			_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	}
	*/

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

		if (_is_wrap == false) {
			text = font->TruncateString(text, "...", pw);
		}

		g->DrawString(text, px, py, pw, ph, _halign, _valign);
	}
}

void Button::RegisterActionListener(ActionListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_action_listener_mutex);

	if (std::find(_action_listeners.begin(), _action_listeners.end(), listener) == _action_listeners.end()) {
		_action_listeners.push_back(listener);
	}
}

void Button::RemoveActionListener(ActionListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_action_listener_mutex);

	std::vector<ActionListener *>::iterator i = std::find(_action_listeners.begin(), _action_listeners.end(), listener);
	
	if (i != _action_listeners.end()) {
		_action_listeners.erase(i);
	}
}

void Button::DispatchActionEvent(ActionEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<ActionListener *> listeners;
	
	_action_listener_mutex.Lock();

	listeners = _action_listeners;

	_action_listener_mutex.Unlock();

	for (std::vector<ActionListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		ActionListener *listener = (*i);

		listener->ActionPerformed(event);
	}

	delete event;
}

std::vector<ActionListener *> & Button::GetActionListeners()
{
	return _action_listeners;
}

}
