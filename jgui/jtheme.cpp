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
#include "jgui/jitemcomponent.h"
#include "jgui/japplication.h"

namespace jgui {

Theme * Theme::_default_theme = new Theme();

Theme * Theme::GetDefaultTheme()
{
  return _default_theme;
}

Theme::Theme():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Theme");

  // INFO:: component
  SetIntegerParam("component.bg", 0xa0202020);
  SetIntegerParam("component.fg", 0xfff0f0f0);
  SetIntegerParam("component.border", 0xff808080);
  SetIntegerParam("component.scroll", 0xff404040);

  SetIntegerParam("component.bg.focus", 0xfff0f0f0);
  SetIntegerParam("component.fg.focus", 0xff000000);
  SetIntegerParam("component.border.focus", 0xfff0f0f0);
  SetIntegerParam("component.scroll.focus", 0xff404040);

  SetIntegerParam("component.bg.disable", 0xff000000);
  SetIntegerParam("component.fg.disable", 0xff808080);
  SetIntegerParam("component.border.disable", 0xff808080);
  SetIntegerParam("component.scroll.disable", 0xff404040);

  SetIntegerParam("component.border.style", JCB_LINE);
  SetIntegerParam("component.border.size", 1);
  
  SetIntegerParam("component.scroll.size", 16);
  SetIntegerParam("component.scroll.gap", 4);
  
  SetIntegerParam("component.hgap", 0);
  SetIntegerParam("component.vgap", 0);

  _fonts["component.font"] = nullptr;

  // INFO:: container
  SetIntegerParam("container.bg", GetIntegerParam("component.bg"));
  SetIntegerParam("container.fg", GetIntegerParam("component.fg"));
  SetIntegerParam("container.border", GetIntegerParam("component.border"));
  SetIntegerParam("container.scroll", GetIntegerParam("component.scroll"));

  SetIntegerParam("container.bg.focus", GetIntegerParam("component.bg.focus"));
  SetIntegerParam("container.fg.focus", GetIntegerParam("component.fg.focus"));
  SetIntegerParam("container.border.focus", GetIntegerParam("component.border.focus"));
  SetIntegerParam("container.scroll.focus", GetIntegerParam("component.scroll.focus"));

  SetIntegerParam("container.bg.disable", GetIntegerParam("component.bg.disable"));
  SetIntegerParam("container.fg.disable", GetIntegerParam("component.fg.disable"));
  SetIntegerParam("container.border.disable", GetIntegerParam("component.border.disable"));
  SetIntegerParam("container.scroll.disable", GetIntegerParam("component.scroll.disable"));

  SetIntegerParam("container.border.style", JCB_EMPTY);
  SetIntegerParam("container.border.size", 0);
  
  SetIntegerParam("container.scroll.size", GetIntegerParam("component.scroll.size"));
  SetIntegerParam("container.scroll.gap", GetIntegerParam("component.scroll.gap"));

  SetIntegerParam("container.hgap", 0);
  SetIntegerParam("container.vgap", 0);

  _fonts["container.font"] = nullptr;

  // INFO:: window
  SetIntegerParam("window.bg", GetIntegerParam("component.bg"));
  SetIntegerParam("window.fg", GetIntegerParam("component.fg"));
  SetIntegerParam("window.border", GetIntegerParam("component.border"));
  SetIntegerParam("window.scroll", GetIntegerParam("component.scroll"));

  SetIntegerParam("window.bg.focus", GetIntegerParam("component.bg.focus"));
  SetIntegerParam("window.fg.focus", GetIntegerParam("component.fg.focus"));
  SetIntegerParam("window.border.focus", GetIntegerParam("component.border.focus"));
  SetIntegerParam("window.scroll.focus", GetIntegerParam("component.scroll.focus"));

  SetIntegerParam("window.bg.disable", GetIntegerParam("component.bg.disable"));
  SetIntegerParam("window.fg.disable", GetIntegerParam("component.fg.disable"));
  SetIntegerParam("window.border.disable", GetIntegerParam("component.border.disable"));
  SetIntegerParam("window.scroll.disable", GetIntegerParam("component.scroll.disable"));

  SetIntegerParam("window.border.style", JCB_RAISED_GRADIENT);
  SetIntegerParam("window.border.size", 2);

  SetIntegerParam("window.scroll.size", GetIntegerParam("component.scroll.size"));
  SetIntegerParam("window.scroll.gap", GetIntegerParam("component.scroll.gap"));

  SetIntegerParam("window.hgap", 0);
  SetIntegerParam("window.vgap", 0);

  _fonts["window.font"] = nullptr;

  // INFO:: item
  SetIntegerParam("item.bg", 0xff000000);
  SetIntegerParam("item.fg", 0xfff0f0f0);
  
  SetIntegerParam("item.bg.focus", 0xff404040);
  SetIntegerParam("item.fg.focus", 0xfff0f0f0);
  
  SetIntegerParam("item.bg.select", 0xff202020);
  SetIntegerParam("item.fg.select", 0xff008080);
  
  SetIntegerParam("item.bg.disable", 0xff101010);
  SetIntegerParam("item.fg.disable", 0xfff0f0f0);
  
  SetIntegerParam("item.size", DEFAULT_COMPONENT_HEIGHT);
  SetIntegerParam("item.hgap", 4);
  SetIntegerParam("item.vgap", 4);

}

Theme::~Theme()
{
}

jgui::Font * Theme::GetFont(std::string id)
{
  return _fonts[id];
}

void Theme::SetFont(std::string id, jgui::Font *font)
{
  _fonts[id] = font;
}

}
