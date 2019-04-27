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

Marquee::Marquee(std::string text, int x, int y, int width, int height):
   Component(x, y, width, height)
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

  Theme *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  jgui::Color 
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

    std::string text = _text;

    if (_type == JMM_BOUNCE && size.width >= font->GetStringWidth(_text.c_str())) {
      text = (char *)(_text.c_str());
    }

    jregion_t 
      clip = g->GetClip();
    int 
      cx = x,
      cy = y,
      cw = w,
      ch = h;

    if (cx > clip.width) {
      cx = clip.width;
    }

    if (cy > clip.height) {
      cy = clip.height;
    }

    if (cw > (clip.width-cx)) {
      cw = clip.width - cx;
    }

    if (ch > (clip.height-cy)) {
      ch = clip.height - cy;
    }

    g->ClipRect(cx, cy, cw - 1, ch - 1);
    g->DrawString(text, x + _position, y);
    g->SetClip(clip.x, clip.y, clip.width, clip.height);
  }
}

void Marquee::Run()
{
  do {
    if (_running == false) {
      return;
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  } while (GetTheme() == nullptr);

  Theme 
    *theme = GetTheme();

  if (theme == nullptr) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  
  if (font == nullptr) {
    return;
  }

  jgui::jsize_t
    size = GetSize();
  int 
    hg = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size");

  while (_running == true) {
    if (_type == JMM_BOUNCE) {
      int 
        tw = font->GetStringWidth(_text.c_str()),
        td = (size.width - 2*hg - tw);

      if (td > 0) {
        td = hg;
      } else {
        td = -td + hg;
      }

      _position = _position - _step;

      if (_position < -td) {
        _step = -_step;
      } else if (_position > (size.width - 2*hg - tw + td)) {
        _step = -_step;
      }
    } else {
      int width = font->GetStringWidth(_text.c_str());

      if (_step < 0) {
        _step = -_step;
      }

      _position = _position - _step;

      if (_position <= -width) {
        _position = size.width - 2*hg;
      }
    }

    Repaint();

    std::this_thread::sleep_for(std::chrono::milliseconds((_interval)));
  }
}

}
