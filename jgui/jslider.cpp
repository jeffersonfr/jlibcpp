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
#include "jgui/jslider.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

Slider::Slider():
  SliderComponent()
{
  jcommon::Object::SetClassName("jgui::Slider");

  _pressed = false;
  _stone_size = 24;
  _inverted = false;

  SetFocusable(true);
}

Slider::~Slider()
{
}

int Slider::GetStoneSize()
{
  return _stone_size;
}
    
void Slider::SetStoneSize(int size)
{
  _stone_size = size;

  Repaint();
}

void Slider::SetInverted(bool b)
{
  if (_inverted == b) {
    return;
  }

  _inverted = b;

  Repaint();
}

bool Slider::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEnabled() == false) {
    return false;
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
      SetValue(_value+_maximum_tick);

      catched = true;
    } else if (action == jevent::JKS_PAGE_UP) {
      SetValue(_value-_maximum_tick);

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
    
bool Slider::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  jgui::jpoint_t<int>
    elocation = event->GetLocation();
  jgui::jsize_t<int>
    size = GetSize();
  int
    dx = GetTheme().GetIntegerParam("hgap") + GetTheme().GetIntegerParam("border.size"),
    dy = GetTheme().GetIntegerParam("vgap") + GetTheme().GetIntegerParam("border.size"),
    dw = size.width - 2*dx - _stone_size,
    dh = size.height - 2*dy - _stone_size;
  bool 
    catched = false;

  if (event->GetButton() == jevent::JMB_BUTTON1) {
    catched = true;

    if (_type == JSO_HORIZONTAL) {
      if (elocation.y > 0 && elocation.y < (size.height)) {
        int d = (int)((_value*dw)/(GetMaximum()-GetMinimum()));

        _pressed = false;

        if (elocation.x > (dx) && elocation.x < (dx+d)) {
          SetValue(_value-_maximum_tick);
        } else if (elocation.x > (dx+d+_stone_size) && elocation.x < (size.width)) {
          SetValue(_value+_maximum_tick);
        } else if (elocation.x > (dx+d) && elocation.x < (dx+d+_stone_size)) {
          _pressed = true;
        }
      }
    } else if (_type == JSO_VERTICAL) {
      if (elocation.x > 0 && elocation.x < (size.width)) {
        int d = (int)((_value*dh)/(GetMaximum()-GetMinimum()));

        _pressed = false;

        if (elocation.y > (dy) && elocation.y < (dy+d)) {
          SetValue(_value-_maximum_tick);
        } else if (elocation.y > (dy+d+_stone_size) && elocation.y < (size.height)) {
          SetValue(_value+_maximum_tick);
        }
      }
    }
  } 

  return catched;
}

    
bool Slider::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  _pressed = false;

  return false;
}
    
bool Slider::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  jgui::jpoint_t<int>
    elocation = event->GetLocation();
  jgui::jsize_t<int>
    size = GetSize();
  int
    dx = GetTheme().GetIntegerParam("hgap") + GetTheme().GetIntegerParam("border.size"),
    dy = GetTheme().GetIntegerParam("vgap") + GetTheme().GetIntegerParam("border.size"),
    dw = size.width - 2*dx - _stone_size,
    dh = size.height - 2*dy - _stone_size;

  if (_pressed == true) {
    int diff = GetMaximum()-GetMinimum();

    if (_type == JSO_HORIZONTAL) {
      SetValue(diff*(elocation.x - _stone_size/2)/dw);
    } else if (_type == JSO_VERTICAL) {
      SetValue(diff*(elocation.y - _stone_size/2)/dh);
    }

    return true;
  }

  return false;
}
    
bool Slider::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }

  _pressed = false;

  SetValue(GetValue()+_minimum_tick*event->GetClicks());

  return true;
}
    
void Slider::Paint(Graphics *g)
{
  // JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::jsize_t<int>
    size = GetSize();
  int
    x = GetTheme().GetIntegerParam("hgap") + GetTheme().GetIntegerParam("border.size"),
    y = GetTheme().GetIntegerParam("vgap") + GetTheme().GetIntegerParam("border.size"),
    w = size.width - 2*x,
    h = size.height - 2*y;

  if (_type == JSO_HORIZONTAL) {
    int 
      d = (int)((_value*(w - _stone_size))/(GetMaximum() - GetMinimum()));

    if (d > (w - _stone_size)) {
      d = w - _stone_size;
    }

    if (HasFocus() == true) {
      g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
    } else {
      g->SetColor(GetTheme().GetIntegerParam("scroll"));
    }
    
    g->FillRectangle({x, (h-4)/2+y, w, 4});

    if (_inverted == false) {
      std::vector<jgui::jpoint_t<int>> p = {
        {0, 0},
        {_stone_size, 0},
        {_stone_size, (int)(h*0.4)},
        {_stone_size/2, h},
        {0, (int)(h*0.4)}
      };

      g->FillPolygon({(int)d+x, y}, p, 5);
    } else {
      std::vector<jgui::jpoint_t<int>> p = {
        {_stone_size/2, 0},
        {_stone_size, (int)(h*0.6)},
        {_stone_size, h},
        {0, h},
        {0, (int)(h*0.6)}
      };

      g->FillPolygon({(int)d+x, y}, p);
    }
  } else if (_type == JSO_VERTICAL) {
    int 
      d = (int)((_value*(h-_stone_size))/(GetMaximum()-GetMinimum()));

    if (d > (h - _stone_size)) {
      d = h - _stone_size;
    }

    if (HasFocus() == true) {
      g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
    } else {
      g->SetColor(GetTheme().GetIntegerParam("scroll"));
    }
    
    g->FillRectangle({(w-10)/2+x, y, 10, h});

    if (_inverted == false) {
      std::vector<jgui::jpoint_t<int>> p = {
        {0, 0},
        {(int)(size.width*0.4), 0},
        {w, _stone_size/2},
        {(int)(size.width*0.4), _stone_size},
        {0, _stone_size}
      };

      g->FillPolygon({x, (int)d+y}, p);
    } else {
      std::vector<jgui::jpoint_t<int>> p = {
        {0, _stone_size/2},
        {(int)(size.width*0.6), 0},
        {w, 0},
        {w, _stone_size},
        {(int)(size.width*0.6), _stone_size}
      };

      g->FillPolygon({x, (int)d+y}, p);
    }
  }
}

}
