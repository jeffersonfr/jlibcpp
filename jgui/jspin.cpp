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
#include "jgui/jspin.h"
#include "jlogger/jloggerlib.h"

namespace jgui {

Spin::Spin():
  Component(),
  ItemComponent()
{
  jcommon::Object::SetClassName("jgui::Spin");

  _type = JSO_HORIZONTAL;
  // _type = JSO_VERTICAL;
  
  SetFocusable(true);
}

Spin::~Spin()
{
}

void Spin::SetScrollOrientation(jscroll_orientation_t type)
{
  if (_type == type) {
    return;
  }

  _type = type;

  Repaint();
}

jscroll_orientation_t Spin::GetScrollOrientation()
{
  return _type;
}

void Spin::NextItem()
{
  _index++;

  if (_index >= (int)_items.size()) {
    _index = _items.size()-1;

    if (_loop == true) {
      _index = 0;
    }
  }

  Repaint();

  DispatchSelectEvent(new jevent::SelectEvent(this, _index, jevent::JSET_RIGHT));
}

void Spin::PreviousItem()
{
  _index--;

  if (_index < 0) {
    _index = 0;

    if (_loop == true) {
      _index = _items.size() - 1;
    }
  }

  Repaint();

  DispatchSelectEvent(new jevent::SelectEvent(this, _index, jevent::JSET_LEFT));
}

bool Spin::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (_items.size() == 0) {
    return true;
  }

  jevent::jkeyevent_symbol_t action = event->GetSymbol();

  bool catched = false;

  if (action == jevent::JKS_CURSOR_LEFT) {
    if (_type == JSO_HORIZONTAL) {
      PreviousItem();

      catched = true;
    }
  } else if (action == jevent::JKS_CURSOR_RIGHT) {
    if (_type == JSO_HORIZONTAL) {
      NextItem();

      catched = true;
    }
  } else if (action == jevent::JKS_CURSOR_UP) {
    if (_type == JSO_VERTICAL) {
      PreviousItem();

      catched = true;
    }
  } else if (action == jevent::JKS_CURSOR_DOWN) {
    if (_type == JSO_VERTICAL) {
      NextItem();

      catched = true;
    }
  } else if (action == jevent::JKS_ENTER) {
    DispatchSelectEvent(new jevent::SelectEvent(this, _index, jevent::JSET_ACTION));

    catched = true;
  }

  return catched;
}

bool Spin::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  if (_items.size() == 0) {
    return true;
  }

  jgui::jpoint_t<int>
    elocation = event->GetLocation();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  int
    h = size.height - GetVerticalPadding();
  bool 
    catched = false;

  if (event->GetButton() == jevent::JMB_BUTTON1) {
    int 
      arrow_size;

    catched = true;

    if (_type == JSO_HORIZONTAL) {
      arrow_size = h/2;
    } else {
      arrow_size = (h-8)/2;
    }

    RequestFocus();

    if (_type == JSO_HORIZONTAL) {
      if (elocation.y > GetPadding().top && elocation.y < (GetPadding().top+size.height)) {
        if (elocation.x > (size.width-arrow_size-GetPadding().left) && elocation.x < (size.width-GetPadding().left)) {
          NextItem();
        } else if (elocation.x > GetPadding().left && elocation.x < (GetPadding().left+arrow_size)) {
          PreviousItem();
        }
      }
    } else if (_type == JSO_VERTICAL) {
      if (elocation.x > (size.width-2*arrow_size-GetPadding().left) && elocation.x < (size.width-GetPadding().left)) {
        if (elocation.y > GetPadding().top && elocation.y < (h/2)) {
          PreviousItem();
        } else if (elocation.y > (GetPadding().top+h/2) && elocation.y < (GetPadding().top+h)) {
          NextItem();
        }
      }
    }
  }

  return catched;
}


bool Spin::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }
  
  return false;
}

bool Spin::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }
  
  return false;
}

bool Spin::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  return false;
}

void Spin::AddEmptyItem()
{
  Item *item = new Item();

  if (_type == JSO_HORIZONTAL) {
    item->SetHorizontalAlign(JHA_CENTER);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  if (_type == JSO_HORIZONTAL) {
    item->SetHorizontalAlign(JHA_CENTER);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::AddImageItem(std::string text, jgui::Image *image)
{
  Item *item = new Item(text, image);

  if (_type == JSO_HORIZONTAL) {
    item->SetHorizontalAlign(JHA_CENTER);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  if (_type == JSO_HORIZONTAL) {
    item->SetHorizontalAlign(JHA_CENTER);
  }
    
  AddInternalItem(item);
  AddItem(item);
}

void Spin::Paint(Graphics *g)
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
  int
    arrow_size = 8;

  if (_type == JSO_HORIZONTAL) {
    int offset = (size.height - 2*arrow_size)/2;

    if (_loop == true || (_index < ((int)_items.size()-1))) {
      if (HasFocus() == true) {
        g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg"));
      }

      g->FillTriangle({w, GetPadding().top + arrow_size + offset}, {GetPadding().left + w - arrow_size, GetPadding().top + offset}, {GetPadding().left + w - arrow_size, GetPadding().top + 2*arrow_size + offset});
    }

    if (_loop == true || (_index > 0 && _items.size() > 0)) {
      if (HasFocus() == true) {
        g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg"));
      }

      g->FillTriangle({GetPadding().left, GetPadding().top + arrow_size + offset}, {GetPadding().left + arrow_size, GetPadding().top + offset}, {GetPadding().left + arrow_size, GetPadding().top + 2*arrow_size + offset});
    }

    if (_items.size() > 0) {
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

        std::string text = _items[_index]->GetValue();

        // if (_wrap == false) {
          text = font->TruncateString(text, "...", w);
        // }

        g->DrawString(text, {GetPadding().left, GetPadding().top, w, h}, _items[_index]->GetHorizontalAlign(), _items[_index]->GetVerticalAlign());
      }
    }
  } else if (_type == JSO_VERTICAL) {
    int offset = (size.height/2 - arrow_size)/2;

    if (_loop == true || (_index < ((int)_items.size()-1))) {
      if (HasFocus() == true) {
        g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg"));
      }
    }

    g->FillTriangle({size.width - 2*arrow_size - GetPadding().left, GetPadding().top + arrow_size + offset}, {size.width - GetPadding().left, GetPadding().top + arrow_size + offset}, {size.width - arrow_size - GetPadding().left, GetPadding().top + offset});

    if (_loop == true || (_index > 0 && _items.size() > 0)) {
      if (HasFocus() == true) {
        g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg"));
      }
    }

    g->FillTriangle({size.width - 2*arrow_size - GetPadding().left, GetPadding().top + arrow_size + size.height/2}, {size.width - GetPadding().left, GetPadding().top + arrow_size + size.height/2}, {size.width - arrow_size - GetPadding().left, GetPadding().top + size.height - offset});


    if (_items.size() > 0) {
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

        std::string text = _items[_index]->GetValue();

        // if (_wrap == false) {
          text = font->TruncateString(text, "...", w);
        // }

        g->DrawString(text, {GetPadding().left, GetPadding().top, w, h}, _items[_index]->GetHorizontalAlign(), _items[_index]->GetVerticalAlign());
      }
    }
  }
}

}

