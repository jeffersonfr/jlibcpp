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
#include "jgui/jbutton.h"
#include "jgui/jimage.h"
#include "jlogger/jloggerlib.h"

#include <algorithm>

namespace jgui {

Button::Button(std::string text):
  Button(text, nullptr)
{
}

Button::Button(std::string text, jgui::Image *image):
  Component()
{
  jcommon::Object::SetClassName("jgui::Button");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _text = text;
  _image = image;

  SetFocusable(true);
}

Button::~Button()
{
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }
}

void Button::UpdatePreferredSize()
{
  jsize_t<int> 
    t = {
      0, 0
    };

  jgui::Border
    border = GetTheme().GetBorder();
  jgui::Font 
    *font = GetTheme().GetFont();

  if (_image != nullptr) {
    jgui::jsize_t<int>
      size = _image->GetSize();

    t.width = t.width + size.width;
    t.height = t.height + size.height;

    if (font != nullptr and GetText().empty() == false) {
      t.width = t.width + 4;
    }
  }

  if (font != nullptr) {
    jgui::jfont_extends_t 
      extends = font->GetStringExtends(GetText());

    t.width = t.width + int(extends.size.width - extends.bearing.x);
    t.height = t.height + int(extends.size.height - extends.bearing.y);
  }

  SetPreferredSize(t + jgui::jsize_t<int>{GetHorizontalPadding() + 2*border.GetSize(), GetVerticalPadding() + 2*border.GetSize()});
}

void Button::SetText(std::string text)
{
  if (_text != text) {
    _text = text;

    UpdatePreferredSize();
    Repaint();
  }
}

std::string Button::GetText()
{
  return _text;
}

void Button::SetImage(jgui::Image *image)
{
  if (_image != image) {
    _image = image;

    UpdatePreferredSize();
    Repaint();
  }
}

jgui::Image * Button::GetImage()
{
  return _image;
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

bool Button::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  bool catched = false;

  if (event->GetSymbol() == jevent::JKS_ENTER) {
    DispatchActionEvent(new jevent::ActionEvent(this));

    catched = true;
  }

  return catched;
}

bool Button::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  if (event->GetButton() == jevent::JMB_BUTTON1) {
    DispatchActionEvent(new jevent::ActionEvent(this));

    return true;
  }

  return false;
}

bool Button::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}

bool Button::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool Button::MouseWheel(jevent::MouseEvent *event)
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

  Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  jgui::jinsets_t
    padding = GetPadding();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();

  if (_image != nullptr) {
    jgui::jsize_t<int>
      size = _image->GetSize();

    g->DrawImage(_image, jpoint_t<int>{padding.left, padding.top});

    padding.left = padding.left + size.width + 4;
  }

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

    std::string text = font->TruncateString(GetText(), "...", w);

    g->DrawString(text, {padding.left, padding.top, w, h}, _halign, _valign);
  }
}

void Button::RegisterActionListener(jevent::ActionListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_action_listener_mutex);

  if (std::find(_action_listeners.begin(), _action_listeners.end(), listener) == _action_listeners.end()) {
    _action_listeners.push_back(listener);
  }
}

void Button::RemoveActionListener(jevent::ActionListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_action_listener_mutex);

  _action_listeners.erase(std::remove(_action_listeners.begin(), _action_listeners.end(), listener), _action_listeners.end());
}

void Button::DispatchActionEvent(jevent::ActionEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _action_listener_mutex.lock();

  std::vector<jevent::ActionListener *> listeners = _action_listeners;

  _action_listener_mutex.unlock();

  for (std::vector<jevent::ActionListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    jevent::ActionListener *listener = (*i);

    listener->ActionPerformed(event);
  }

  delete event;
}

const std::vector<jevent::ActionListener *> & Button::GetActionListeners()
{
  return _action_listeners;
}

}
