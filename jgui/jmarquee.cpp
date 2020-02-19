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
#include "jgui/jmarquee.h"
#include "jlogger/jloggerlib.h"

#include <thread>

#include <unistd.h>

namespace jgui {

Marquee::Marquee(std::string text):
   Component()
{
  jcommon::Object::SetClassName("jgui::Marquee");

  _text = text;
  _step = 10;
  _position = 0;
  _running = false;

  SetInterval(100);
  SetType(JMM_LOOP);
}

Marquee::~Marquee()
{
  Stop();
}

void Marquee::SetStep(int i)
{
  _step = i;
}

void Marquee::SetInterval(int i)
{
  _interval = i;
}

void Marquee::SetType(jmarquee_mode_t type)
{
  _type = type;
  _position = 0;
}

void Marquee::Start()
{
   std::lock_guard<std::mutex> guard(_marquee_mutex);

  if (_running == true) {
    return;
  }

  _running = true;

  _thread = std::thread(&Marquee::Run, this);
}

void Marquee::Stop()
{
   std::lock_guard<std::mutex> guard(_marquee_mutex);

  if (_running == false) {
    return;
  }

  _running = false;

  try {
    _thread.join();
  } catch (...) {
  }
}

void Marquee::SetText(std::string text)
{
  _text = text;
}

std::string Marquee::GetText()
{
  return _text;
}

void Marquee::Paint(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();

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

    g->DrawString(GetText(), jpoint_t<int>{_position, (size.height - font->GetSize())/2});
  }
}

void Marquee::Run()
{
  jgui::Font 
    *font = GetTheme().GetFont();
  
  if (font == nullptr) {
    return;
  }

  jgui::jsize_t<int>
    size = GetSize();

  while (_running == true) {
    if (_type == JMM_BOUNCE) {
      int 
        tw = font->GetStringWidth(_text.c_str()),
        td = (size.width - GetHorizontalPadding() - tw);

      if (td > 0) {
        td = GetHorizontalPadding();
      } else {
        td = -td + GetHorizontalPadding();
      }

      _position = _position - _step;

      if (_position < -td) {
        _step = -_step;
      } else if (_position > (size.width - GetHorizontalPadding() - tw + td)) {
        _step = -_step;
      }
    } else {
      int width = font->GetStringWidth(_text.c_str());

      if (_step < 0) {
        _step = -_step;
      }

      _position = _position - _step;

      if (_position <= -width) {
        _position = size.width - GetHorizontalPadding();
      }
    }

    Repaint();

    std::this_thread::sleep_for(std::chrono::milliseconds((_interval)));
  }
}

}
