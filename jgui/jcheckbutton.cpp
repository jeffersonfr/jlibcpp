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

CheckButton::CheckButton(jcheckbox_type_t type, std::string text):
  Component()
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

  jgui::jsize_t<int>
    size = GetSize();

  if (event->GetButton() == jevent::JMB_BUTTON1) {
    jgui::jpoint_t
      elocation = event->GetLocation();
    int
      ms = size.height;

    if (size.height > size.width) {
      ms = size.width;
    }

    if ((elocation.x > 0 && elocation.x < ms) && (elocation.y > 0 && elocation.y < ms)) {
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

  Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();
  int
    major = 16,
    minor = 4,
    cs = (std::min(size.width, size.height) - major)/2;
  int
    offset = GetPadding().left;

  offset = offset + 8;

  g->SetColor(GetTheme().GetIntegerParam("border.select"));

  if (_type == JCBT_CHECK) {
    g->FillRectangle({offset, GetPadding().top + (size.height - cs)/2, major, major});
  } else if (_type == JCBT_RADIO) {
    g->FillCircle({offset + major/2, size.height/2}, major/2);
  }

  if (IsSelected() == true) {
    g->SetColor(GetTheme().GetIntegerParam("border"));

    if (_type == JCBT_CHECK) {
      g->FillRectangle({offset + minor, GetPadding().top + (size.height - cs)/2 + minor, 2*minor, 2*minor});
    } else {
      g->FillCircle({offset + major/2, size.height/2}, minor);
    }
  }

  offset = offset + major + 8;

  if (font != nullptr) {
    g->SetFont(font);

    if (IsEnabled() == true) {
      if (HasFocus() == true) {
        g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg"));
      }
    } else {
      g->SetColor(GetTheme().GetIntegerParam("fg.disable"));
    }

    std::string text = GetText();

    if (_is_wrap == false) {
      text = font->TruncateString(text, "...", w);
    }

    g->DrawString(text, {offset, GetPadding().top, w, h}, _halign, _valign);
  }
}

void CheckButton::RegisterToggleListener(jevent::ToggleListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_check_listener_mutex);

  if (std::find(_check_listeners.begin(), _check_listeners.end(), listener) == _check_listeners.end()) {
    _check_listeners.push_back(listener);
  }
}

void CheckButton::RemoveToggleListener(jevent::ToggleListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_check_listener_mutex);

  _check_listeners.erase(std::remove(_check_listeners.begin(), _check_listeners.end(), listener), _check_listeners.end());
}

void CheckButton::DispatchToggleEvent(jevent::ToggleEvent *event)
{
  if (event == nullptr) {
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

const std::vector<jevent::ToggleListener *> & CheckButton::GetToggleListeners()
{
  return _check_listeners;
}

}
