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

  _text = text;
  _halign = JHA_CENTER;
  _valign = JVA_CENTER;
  _is_wrap = false;
}

Text::~Text()
{
}

void Text::UpdatePreferredSize()
{
  jsize_t<int> 
    t = {
      0, 0
    };

  jgui::Border
    border = GetTheme().GetBorder();
  jgui::Font 
    *font = GetTheme().GetFont();

  if (font != nullptr) {
    if (IsWrap() == false) {
      jgui::jfont_extends_t 
        extends = font->GetStringExtends(GetText());

      t.width = int(extends.size.width - extends.bearing.x);
      t.height = int(extends.size.height - extends.bearing.y);
    } else {
      jgui::jsize_t<int>
        size = GetSize();

      size.width = size.width;
      size.height = 0;

      if (size.width < 0) {
        size.width = 0;
      }

      if (size.width > 0) {
        std::vector<std::string> lines;

        font->GetStringBreak(&lines, GetText(), {size.width, INT_MAX});

        t.height = lines.size()*font->GetSize();
      }
    }
  }

  SetPreferredSize(t + jgui::jsize_t<int>{GetHorizontalPadding() + 2*border.GetSize(), GetVerticalPadding() + 2*border.GetSize()});
}

void Text::SetWrap(bool wrap)
{
  if (_is_wrap == wrap) {
    _is_wrap = wrap;

    UpdatePreferredSize();
    Repaint();
  }
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
  if (_text != text) {
    _text = text;

    UpdatePreferredSize();
    Repaint();
  }
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

void Text::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();

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

    g->DrawString(text, {GetPadding().left, GetPadding().top, w, h}, _halign, _valign);
  }
}

}
