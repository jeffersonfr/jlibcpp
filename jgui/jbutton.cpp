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

Button::Button(std::string text, int x, int y, int width, int height):
  Component(x, y, width, height)
{
  jcommon::Object::SetClassName("jgui::Button");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _text = text;
  _image = nullptr;

  SetFocusable(true);
}

Button::Button(std::string text, jgui::Image *image, int x, int y, int width, int height):
  Component(x, y, width, height)
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

void Button::SetText(std::string text)
{
  _text = text;

  Repaint();
}

void Button::SetImage(jgui::Image *image)
{
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }

  _image = image;

  Repaint();
}

jgui::Image * Button::GetImage()
{
  return _image;
}

std::string Button::GetText()
{
  return _text;
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

  Theme *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  Font 
    *font = theme->GetFont("component.font");
  Color 
    bg = theme->GetIntegerParam("component.bg"),
    fg = theme->GetIntegerParam("component.fg"),
    fgfocus = theme->GetIntegerParam("component.fg.focus"),
    fgdisable = theme->GetIntegerParam("component.fg.disable");
  jgui::jsize_t
    size = GetSize();
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
    y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
    w = size.width - 2*x,
    h = size.height - 2*y;

  if (_image != nullptr) {
    jgui::jsize_t
      size = _image->GetSize();

    x = x + size.width + 8;;

    g->DrawImage(_image, 8, 8);
  }

  if (font != nullptr) {
    g->SetFont(font);

    if (IsEnabled() == true) {
      if (HasFocus() == true) {
        g->SetColor(fgfocus);
      } else {
        g->SetColor(fg);
      }
    } else {
      g->SetColor(fgdisable);
    }

    std::string text = font->TruncateString(GetText(), "...", w);

    g->DrawString(text, x, y, w, h, _halign, _valign);
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
