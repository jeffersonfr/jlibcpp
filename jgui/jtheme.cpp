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
#include "jgui/jtheme.h"
#include "jgui/japplication.h"
#include "jgui/jcomponent.h"

namespace jgui {

Theme::Theme():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Theme");

  // INFO:: component
  SetIntegerParam("bg", 0xa0202020);
  SetIntegerParam("bg.focus", 0xff404040);
  SetIntegerParam("bg.disable", 0xff000000);
  SetIntegerParam("bg.select", 0xff404040);

  SetIntegerParam("fg", 0xfff0f0f0);
  SetIntegerParam("fg.focus", 0xfff0f0f0);
  SetIntegerParam("fg.disable", 0xff808080);
  SetIntegerParam("fg.select", 0xfff0f0f0);

  SetIntegerParam("border", 0xff808080);
  SetIntegerParam("border.focus", 0xfff0f0f0);
  SetIntegerParam("border.disable", 0xff808080);
  SetIntegerParam("border.select", 0xfff0f0f0);

  SetIntegerParam("scroll", 0xff404040);
  SetIntegerParam("scroll.focus", 0xff404040);
  SetIntegerParam("scroll.disable", 0xff404040);
  SetIntegerParam("scroll.select", 0xff404040);

  SetIntegerParam("border.style", JCB_LINE);
  SetIntegerParam("border.size", 1);
  
  SetIntegerParam("scroll.size", 16);
  SetIntegerParam("scroll.gap", 4);
  
  SetIntegerParam("hgap", 0);
  SetIntegerParam("vgap", 0);

  SetFont("primary", &Font::SMALL);
  SetFont("secondary", &Font::TINY);
}

Theme::~Theme()
{
}

jgui::Font * Theme::GetFont(std::string id)
{
  if (_fonts.find(id) == _fonts.end()) {
    return nullptr;
  }

  return _fonts[id];
}

void Theme::SetFont(std::string id, jgui::Font *font)
{
  _fonts[id] = font;
}

}
