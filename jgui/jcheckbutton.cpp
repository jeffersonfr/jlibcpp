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
#include "jgui/jcheckbutton.h"
#include "jlogger/jloggerlib.h"

#include <algorithm>

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

		DispatchToggleEvent(new jevent::ToggleEvent(this, _checked));
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
		
bool CheckButton::KeyPressed(jevent::KeyEvent *event)
{
	if (Component::KeyPressed(event) == true) {
		return true;
	}

	bool catched = false;

  jevent::jkeyevent_symbol_t action = event->GetSymbol();

	if (action == jevent::JKS_ENTER) {
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

bool CheckButton::MousePressed(jevent::MouseEvent *event)
{
	if (Component::MousePressed(event) == true) {
		return true;
	}

	if (event->GetButton() == jevent::JMB_BUTTON1) {
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

bool CheckButton::MouseReleased(jevent::MouseEvent *event)
{
	if (Component::MouseReleased(event) == true) {
		return true;
	}

	return false;
}

bool CheckButton::MouseMoved(jevent::MouseEvent *event)
{
	if (Component::MouseMoved(event) == true) {
		return true;
	}

	return false;
}

bool CheckButton::MouseWheel(jevent::MouseEvent *event)
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
  
  if (theme == NULL) {
    return;
  }

	Font 
    *font = theme->GetFont("component.font");
	Color 
    bg = theme->GetIntegerParam("component.bg"),
	  fg = theme->GetIntegerParam("component.fg"),
	  fgfocus = theme->GetIntegerParam("component.fg.focus"),
	  fgdisable = theme->GetIntegerParam("component.fg.disable");
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
		y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
		w = _size.width - 2*x,
		h = _size.height - 2*y;
	int
    major = 16,
    minor = 4,
    cs = (std::min(_size.width, _size.height) - major)/2;

	if (_has_focus == true) {
		g->SetColor(fgfocus);
	} else {
		g->SetColor(fg);
	}

	if (_type == JCBT_CHECK) {
		g->FillRectangle(x, y + (_size.height - cs)/2, cs, cs);
  } else if (_type == JCBT_RADIO) {
		g->FillCircle(x + cs/2, _size.height/2, cs);
	}

	if (_checked == true) {
		if (_has_focus == true) {
			g->SetColor(fg);
		} else {
			g->SetColor(fgfocus);
		}

		if (_type == JCBT_CHECK) {
		  g->FillRectangle(x + minor, y + (_size.height - cs)/2 + minor, cs - 2*minor, cs - 2*minor);
		} else {
		  g->FillCircle(x + cs/2, _size.height/2, minor);
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

		std::string text = GetText();

		if (_is_wrap == false) {
			text = font->TruncateString(text, "...", w);
		}

		g->DrawString(text, x, y, w, h, _halign, _valign);
	}
}

void CheckButton::RegisterToggleListener(jevent::ToggleListener *listener)
{
	if (listener == NULL) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_check_listener_mutex);

	if (std::find(_check_listeners.begin(), _check_listeners.end(), listener) == _check_listeners.end()) {
		_check_listeners.push_back(listener);
	}
}

void CheckButton::RemoveToggleListener(jevent::ToggleListener *listener)
{
	if (listener == NULL) {
		return;
	}

 	std::lock_guard<std::mutex> guard(_check_listener_mutex);

  _check_listeners.erase(std::remove(_check_listeners.begin(), _check_listeners.end(), listener), _check_listeners.end());
}

void CheckButton::DispatchToggleEvent(jevent::ToggleEvent *event)
{
	if (event == NULL) {
		return;
	}

	_check_listener_mutex.lock();

	std::vector<jevent::ToggleListener *> listeners = _check_listeners;

	_check_listener_mutex.unlock();

	for (std::vector<jevent::ToggleListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		jevent::ToggleListener *listener = (*i);

		listener->StateChanged(event);
	}

	delete event;
}

std::vector<jevent::ToggleListener *> & CheckButton::GetToggleListeners()
{
	return _check_listeners;
}

}
