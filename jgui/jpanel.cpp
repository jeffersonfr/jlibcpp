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
#include "jgui/jpanel.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

Panel::Panel(jgui::jrect_t<int> bounds):
  Container(bounds)
{
  jcommon::Object::SetClassName("jgui::Panel");
  
  jgui::jinsets_t<int>
    insets;

  insets.left = 8;
  insets.right = 8;
  insets.top = 64;
  insets.bottom = 8;

  SetInsets(insets);
  SetFocusable(true);
  SetBackgroundVisible(true);
}

Panel::Panel(std::string title, jgui::jrect_t<int> bounds):
  Container(bounds)
{
  jcommon::Object::SetClassName("jgui::Panel");
  
  _title = title;

  jgui::jinsets_t<int>
    insets;

  insets.left = 8;
  insets.right = 8;
  insets.top = 64;
  insets.bottom = 8;

  SetInsets(insets);
  SetFocusable(true);
  SetBackgroundVisible(true);
}

Panel::~Panel()
{
}

void Panel::SetTitle(std::string title)
{
  _title = title;
}

std::string Panel::GetTitle()
{
  return _title;
}
    
void Panel::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Container::Paint(g);
}

void Panel::PaintGlassPane(Graphics *g)
{
  jgui::Font 
    *font = GetTheme().GetFont();
  jgui::jsize_t<int>
    size = GetSize();
  jinsets_t<int>
    insets = GetInsets();
  int 
    bs = GetTheme().GetIntegerParam("border.size");

  if (_title != "") {
    g->SetGradientStop(0.0, GetTheme().GetIntegerParam("bg"));
    g->SetGradientStop(1.0, GetTheme().GetIntegerParam("scroll"));
    g->FillLinearGradient({bs, bs, size.width - 2*bs, insets.top - 2*bs}, {0, 0}, {0, insets.top - 2*bs});
    g->ResetGradientStop();

    if (font != nullptr) {
      int y = (insets.top-font->GetSize())/2;

      if (y < 0) {
        y = 0;
      }

      std::string text = _title;
      
      // if (_wrap == false) {
        text = font->TruncateString(text, "...", (size.width-insets.left-insets.right));
      // }

      g->SetFont(font);
      g->SetColor(GetTheme().GetIntegerParam("fg"));
      g->DrawString(text, jpoint_t<int>{insets.left + (size.width - insets.left - insets.right - font->GetStringWidth(text))/2, y});
    }
  }
}

}
