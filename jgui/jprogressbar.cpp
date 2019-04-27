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
#include "jgui/jprogressbar.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

ProgressBar::ProgressBar(int x, int y, int width, int height, jscroll_orientation_t type):
     Component(x, y, width, height)
{
  jcommon::Object::SetClassName("jgui::ProgressBar");

  _type = type;
  _value = 0;
  _fixe_delta = 10;
  _delta = _fixe_delta;
  _stone_size = 32;
}

ProgressBar::~ProgressBar()
{
}

void ProgressBar::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;

  Repaint();
}

jscroll_orientation_t ProgressBar::GetScrollOrientation()
{
  return _type;
}

int ProgressBar::GetStoneSize()
{
  return _stone_size;
}

void ProgressBar::SetStoneSize(int size)
{
  _stone_size = size;

  Repaint();
}
    
double ProgressBar::GetValue()
{
  return _value;
}

void ProgressBar::SetValue(double i)
{
  _value = (int)i;

  if (_value < 0.0) {
    _value = 0;
  }

  if (_value > 100) {
    _value = 100;
  }

  Repaint();
}

void ProgressBar::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  Theme 
    *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  jgui::Color 
    bg = theme->GetIntegerParam("component.bg"),
    fg = theme->GetIntegerParam("component.fg"),
    fgfocus = theme->GetIntegerParam("component.fg.focus"),
    fgdisable = theme->GetIntegerParam("component.fg.disable"),
    scroll = theme->GetIntegerParam("component.scroll");
  jgui::jsize_t
    size = GetSize();
  std::string 
    text;
  int
    x = theme->GetIntegerParam("component.hgap") + theme->GetIntegerParam("component.border.size"),
    y = theme->GetIntegerParam("component.vgap") + theme->GetIntegerParam("component.border.size"),
    w = size.width - 2*x,
    h = size.height - 2*y;

  if (_type == JSO_HORIZONTAL) {
    double 
      d = (_value*w)/100.0;
    char 
      t[255];

    if (d > w) {
      d = w;
    }

    g->SetColor(scroll);
    g->FillRectangle(x, y, (int)d, h);

    snprintf(t, 255-1, "%d %%", _value);

    text = (char *)t;
  } else if (_type == JSO_VERTICAL) {
    double 
      d = (_value*h)/100.0;
    char 
      t[255];

    if (d > h) {
      d = h;
    }

    g->SetColor(scroll);
    g->FillRectangle(x, y, w, (int)d);

    snprintf(t, 255-1, "%d %%", _value);

    text = (char *)t;
  }

  if (IsEnabled() == true) {
    if (HasFocus() == true) {
      g->SetColor(fgfocus);
    } else {
      g->SetColor(fg);
    }
  } else {
    g->SetColor(fgdisable);
  }

  // if (_wrap == false) {
  text = font->TruncateString(text, "...", w);
  x = x + (w - font->GetStringWidth(text))/2;
  // }

  g->DrawString(text, x, y, w, h);
}

}
