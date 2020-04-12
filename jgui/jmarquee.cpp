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

#define STEP 16

namespace jgui {

Marquee::Marquee(std::string text, std::chrono::milliseconds interval):
   Animation(std::chrono::milliseconds(0), interval)
{
  jcommon::Object::SetClassName("jgui::Marquee");

  _text = text;
  _position = 0;
}

Marquee::~Marquee()
{
}

void Marquee::SetText(std::string text)
{
  _text = text;
}

std::string Marquee::GetText()
{
  return _text;
}

void Marquee::Update(std::chrono::milliseconds tick)
{
  jgui::Font 
    *font = GetTheme().GetFont();

  if (font == nullptr) {
    return;
  }

  jgui::jsize_t<int>
    size = GetSize();
  int 
    width = font->GetStringWidth(_text.c_str());

  _position = _position - STEP;

  if (_position <= -width) {
    _position = size.width;
  }
}

void Marquee::Render(Graphics *g)
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

}
