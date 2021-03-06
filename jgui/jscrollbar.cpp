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
#include "jgui/jscrollbar.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

ScrollBar::ScrollBar():
  SliderComponent()
{
  jcommon::Object::SetClassName("jgui::ScrollBar");

  _pressed = false;
  _stone_size = 32;
  _label_visible = true;
  
  SetFocusable(true);
}

ScrollBar::~ScrollBar()
{
}

void ScrollBar::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;

  Repaint();
}

jscroll_orientation_t ScrollBar::GetScrollOrientation()
{
  return _type;
}

void ScrollBar::SetStoneSize(int size)
{
  _stone_size = size;

  Repaint();
}

int ScrollBar::GetStoneSize()
{
  return _stone_size;
}

bool ScrollBar::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  jevent::jkeyevent_symbol_t action = event->GetSymbol();

  bool catched = false;

  if (_type == JSO_HORIZONTAL) {
    if (action == jevent::JKS_CURSOR_LEFT) {
      SetValue(_value-_minimum_tick);

      catched = true;
    } else if (action == jevent::JKS_CURSOR_RIGHT) {
      SetValue(_value+_minimum_tick);

      catched = true;
    } else if (action == jevent::JKS_PAGE_DOWN) {
      SetValue(_value-_maximum_tick);

      catched = true;
    } else if (action == jevent::JKS_PAGE_UP) {
      SetValue(_value+_maximum_tick);

      catched = true;
    }
  } else if (_type == JSO_VERTICAL) {
    if (action == jevent::JKS_CURSOR_UP) {
      SetValue(_value-_minimum_tick);

      catched = true;
    } else if (action == jevent::JKS_CURSOR_DOWN) {
      SetValue(_value+_minimum_tick);

      catched = true;
    } else if (action == jevent::JKS_PAGE_DOWN) {
      SetValue(_value-_maximum_tick);

      catched = true;
    } else if (action == jevent::JKS_PAGE_UP) {
      SetValue(_value+_maximum_tick);

      catched = true;
    }
  }

  return catched;
}

bool ScrollBar::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  jgui::jpoint_t
    elocation = event->GetLocation();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();
  int 
    arrow_size,
    dx = GetPadding().left,
    dy = GetPadding().right,
    dw = size.width - 2*dx-_stone_size,
    dh = size.height - 2*dy-_stone_size;
  bool 
    catched = false;

  if (_type == JSO_HORIZONTAL) {
    arrow_size = h/2;
  } else {
    arrow_size = w/2;
  }

  if (event->GetButton() != jevent::JMB_BUTTON1) {
    return false;
  }

  catched = true;

  if (_type == JSO_HORIZONTAL) {
    if (elocation.y > 0 && elocation.y < (size.height)) {
      int d = (int)((_value*(dw-2*arrow_size))/(GetMaximum()-GetMinimum()));

      _pressed = false;

      if (elocation.x > (dx) && elocation.x < (arrow_size + dx)) {
        SetValue(_value - _minimum_tick);
      } else if (elocation.x > (size.width - arrow_size - dx) && elocation.x < (size.width - dx)) {
        SetValue(_value + _minimum_tick);
      } else if (elocation.x > (arrow_size + dx) && elocation.x < (arrow_size + dx + d)) {
        SetValue(_value - _maximum_tick);
      } else if (elocation.x > (arrow_size + dx + d + _stone_size) && elocation.x < (size.width - arrow_size)) {
        SetValue(_value + _maximum_tick);
      } else if (elocation.x > (arrow_size + dx + d) && elocation.x < (arrow_size + dx + d + _stone_size)) {
        _pressed = true;
      }
    }
  } else if (_type == JSO_VERTICAL) {
    if (elocation.x > 0 && elocation.x < (size.width)) {
      int d = (int)((_value*(dh-2*arrow_size))/(GetMaximum()-GetMinimum()));

      _pressed = false;

      if (elocation.y > (dy) && elocation.y < (arrow_size + dy)) {
        SetValue(_value - _minimum_tick);
      } else if (elocation.y > (size.height - arrow_size - dy) && elocation.y < (size.height - dy)) {
        SetValue(_value + _minimum_tick);
      } else if (elocation.y > (arrow_size + dy) && elocation.y < (arrow_size + dy + d)) {
        SetValue(_value - _maximum_tick);
      } else if (elocation.y > (arrow_size + dy + d + _stone_size) && elocation.y < (size.height - arrow_size)) {
        SetValue(_value + _maximum_tick);
      } else if (elocation.y > (arrow_size + dy + d) && elocation.y < (arrow_size + dy + d + _stone_size)) {
        _pressed = true;
      }
    }
  }


  return catched;
}

bool ScrollBar::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }
  
  _pressed = false;

  return true;
}

bool ScrollBar::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }
  
  jgui::jpoint_t
    elocation = event->GetLocation();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetHorizontalPadding();
  int 
    arrow_size,
    dx = GetPadding().left,
    dy = GetPadding().right,
    dw = size.width - 2*dx - _stone_size,
    dh = size.height - 2*dy - _stone_size;

  if (_type == JSO_HORIZONTAL) {
    arrow_size = h/2;
  } else {
    arrow_size = w/2;
  }

  if (_pressed == true) {
    int 
      diff = GetMaximum()-GetMinimum();

    if (_type == JSO_HORIZONTAL) {
      SetValue(diff*(elocation.x - _stone_size/2 - arrow_size)/(dw - 2*arrow_size));
    } else if (_type == JSO_VERTICAL) {
      SetValue(diff*(elocation.y - _stone_size/2 - arrow_size)/(dh - 2*arrow_size));
    }

    return true;
  }

  return false;
}

bool ScrollBar::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  _pressed = false;

  SetValue(GetValue()+_minimum_tick*event->GetClicks());

  return true;
}

void ScrollBar::Paint(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    w = size.width - GetHorizontalPadding(),
    h = size.height - GetVerticalPadding();

  if (_type == JSO_HORIZONTAL) {
    int
      arrow_size = h/2,
      limit = w - _stone_size - 2*arrow_size;
    double 
      d = (_value*limit)/(GetMaximum() - GetMinimum());

    if (d > limit) {
      d = limit;
    }

    if (HasFocus() == true) {
      g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
    } else {
      g->SetColor(GetTheme().GetIntegerParam("scroll"));
    }

    g->FillRectangle({(int)d + arrow_size + GetPadding().left, GetPadding().top, _stone_size, h});

    g->FillTriangle({GetPadding().left+w, GetPadding().top+arrow_size}, {GetPadding().left+w-arrow_size, GetPadding().top}, {GetPadding().left+w-arrow_size, GetPadding().top+2*arrow_size});
    g->FillTriangle({GetPadding().left, GetPadding().top+arrow_size}, {GetPadding().left+arrow_size, GetPadding().top}, {GetPadding().left+arrow_size, GetPadding().top+2*arrow_size});
  } else if (_type == JSO_VERTICAL) {
    int 
      arrow_size = w/2,
      limit = h - _stone_size - 2*arrow_size - GetVerticalPadding();
    double 
      d = (_value*limit)/(GetMaximum()-GetMinimum());

    if (d > limit) {
      d = limit;
    }

    if (HasFocus() == true) {
      g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
    } else {
      g->SetColor(GetTheme().GetIntegerParam("scroll"));
    }

    g->FillRectangle({GetPadding().left, (int)d + arrow_size + GetPadding().top + GetVerticalPadding(), w, _stone_size});
    g->FillTriangle({GetPadding().left, GetPadding().top+arrow_size}, {GetPadding().left+w/2, GetPadding().top}, {GetPadding().left+w, GetPadding().top+arrow_size});
    g->FillTriangle({GetPadding().left, GetPadding().top+h-arrow_size}, {GetPadding().left+w/2, GetPadding().top+h}, {GetPadding().left+w, GetPadding().top+h-arrow_size});
  }
}

}
