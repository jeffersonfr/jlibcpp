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
#include "jgui/jtext.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jstringutils.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

Text::Text(std::string text):
   Component()
{
  jcommon::Object::SetClassName("jgui::Text");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _text = text;
  _is_wrap = false;
}

Text::~Text()
{
}

void Text::SetWrap(bool b)
{
  if (_is_wrap == b) {
    return;
  }

  _is_wrap = b;

  Repaint();
}

bool Text::IsWrap()
{
  return _is_wrap;
}

std::string Text::GetText()
{
  return _text;
}

void Text::SetText(std::string text)
{
  _text = text;

  Repaint();
}

void Text::SetHorizontalAlign(jhorizontal_align_t align)
{
  if (_halign != align) {
    _halign = align;

    Repaint();
  }
}

jhorizontal_align_t Text::GetHorizontalAlign()
{
  return _halign;
}

void Text::SetVerticalAlign(jvertical_align_t align)
{
  if (_valign != align) {
    _valign = align;

    Repaint();
  }
}

jvertical_align_t Text::GetVerticalAlign()
{
  return _valign;
}

jsize_t<int> Text::GetPreferredSize()
{
  jsize_t<int> 
    t = {
      0, 0
    };

  jgui::Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();
  int
    gx = GetTheme().GetIntegerParam("hgap") + GetTheme().GetIntegerParam("border.size"),
    gy = GetTheme().GetIntegerParam("vgap") + GetTheme().GetIntegerParam("border.size");

  if (font != nullptr) {
    int 
      wp = size.width - 2*gx,
      hp = font->GetSize();

    if (wp > 0) {
      std::vector<std::string> lines;

      font->GetStringBreak(&lines, _text, {wp, INT_MAX});

      t.height = lines.size()*hp + 2*gy;
    }
  }

  return t;
}

void Text::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();
  int
    x = GetTheme().GetIntegerParam("hgap") + GetTheme().GetIntegerParam("border.size"),
    y = GetTheme().GetIntegerParam("vgap") + GetTheme().GetIntegerParam("border.size"),
    w = size.width - 2*x,
    h = size.height - 2*y;

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

    g->DrawString(text, {x, y, w, h}, _halign, _valign);
  }
}

}
