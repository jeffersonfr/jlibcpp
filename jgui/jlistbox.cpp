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
#include "jgui/jlistbox.h"
#include "jlogger/jloggerlib.h"
#include "jexception/joutofboundsexception.h"

namespace jgui {

ListBox::ListBox():
  Component(),
  ItemComponent()
{
  jcommon::Object::SetClassName("jgui::ListBox");

  _pressed = false;
  _selected_index = -1;
  _mode = JLBM_NONE_SELECTION;

  SetFocusable(true);
}

ListBox::~ListBox() 
{
}

void ListBox::UpdatePreferredSize()
{
  jgui::jsize_t<int> 
    t = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();

  // TODO:: list all items to see the largest width [+ image.width + gap] 

  t.width = t.width + GetHorizontalPadding() + 2*border.GetSize();
  t.height = _items.size()*(GetItemSize() + GetItemGap()) - GetItemGap() + GetVerticalPadding() + 2*border.GetSize();

  SetPreferredSize(t);
}

void ListBox::SetSelectionType(jlistbox_mode_t type)
{
  if (_mode == type) {
    return;
  }

  _mode = type;
  _selected_index = -1;

  for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
    (*i)->SetSelected(false);
  }
}

jlistbox_mode_t ListBox::GetSelectionType()
{
  return _mode;
}

void ListBox::AddEmptyItem()
{
  Item *item = new Item();

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);

  UpdatePreferredSize();
}

void ListBox::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
  
  UpdatePreferredSize();
}

void ListBox::AddImageItem(std::string text, jgui::Image *image)
{
  Item *item = new Item(text, image);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);

  UpdatePreferredSize();
}

void ListBox::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);

  UpdatePreferredSize();
}

void ListBox::SetCurrentIndex(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    throw jexception::OutOfBoundsException("Index out of bounds exception");
  }

  _index = i;

  jgui::jpoint_t<int>
    slocation = GetScrollLocation();

  SetScrollLocation(slocation.x, _index*(GetItemSize() + GetItemGap()));
}

bool ListBox::IsSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return false;
  }

  if (_mode == JLBM_SINGLE_SELECTION) {
    if (_selected_index == i) {
      return true;
    }
  } else if (_mode == JLBM_MULTI_SELECTION) {
    return _items[i]->IsSelected();
  }

  return false;
}

void ListBox::SetSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_mode == JLBM_SINGLE_SELECTION) {
    if (_selected_index == i) {
      _selected_index = -1;
    } else {
      _selected_index = i;
    }

    Repaint();
  } else if (_mode == JLBM_MULTI_SELECTION) {
    if (item->IsSelected()) {
      item->SetSelected(false);
    } else {
      item->SetSelected(true);
    }

    Repaint();
  }
}

void ListBox::Select(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_mode == JLBM_SINGLE_SELECTION) {
    _selected_index = i;

    Repaint();
  } else if (_mode == JLBM_MULTI_SELECTION) {
    item->SetSelected(true);

    Repaint();
  }
}

void ListBox::Deselect(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_mode == JLBM_SINGLE_SELECTION) {
    _selected_index = -1;

    Repaint();
  } else if (_mode == JLBM_MULTI_SELECTION) {
    item->SetSelected(false);

    Repaint();
  }
}

int ListBox::GetSelectedIndex()
{
  return _selected_index;
}

bool ListBox::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEnabled() == false) {
    return false;
  }

  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  jevent::jkeyevent_symbol_t 
    action = event->GetSymbol();
  bool 
    catched = false;

  if (action == jevent::JKS_CURSOR_UP) {
    IncrementLines(1);
    
    catched = true;
  } else if (action == jevent::JKS_PAGE_UP) {
    IncrementLines((size.height-GetVerticalPadding())/(GetItemSize() + GetItemGap()));
    
    catched = true;
  } else if (action == jevent::JKS_CURSOR_DOWN) {
    DecrementLines(1);

    catched = true;
  } else if (action == jevent::JKS_PAGE_DOWN) {
    DecrementLines((size.height-GetVerticalPadding())/(GetItemSize() + GetItemGap()));

    catched = true;
  } else if (action == jevent::JKS_HOME) {
    IncrementLines(_items.size());
    
    catched = true;
  } else if (action == jevent::JKS_END) {
    DecrementLines(_items.size());
    
    catched = true;
  } else if (action == jevent::JKS_ENTER) {
    SetSelected(_index);

    if (_items.size() > 0) { 
      DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_ACTION));
    }

    catched = true;
  }

  return catched;
}

bool ListBox::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return true;
}

bool ListBox::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}
  
bool ListBox::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool ListBox::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  jgui::jpoint_t<int>
    slocation = GetScrollLocation();

  SetScrollLocation(slocation.x, slocation.y + GetItemSize()*event->GetClicks());

  Repaint();

  return true;
}

void ListBox::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  jgui::Font 
    *font = GetTheme().GetFont();
  jpoint_t<int> 
    scroll_location = GetScrollLocation();
  jgui::jsize_t<int>
    size = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();
  jgui::jinsets_t
    padding = GetPadding();
  int
    w = size.width - GetHorizontalPadding();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;
    // scrollw = (IsScrollableY() == true)?(ss + sg):0;
  int 
    offset = 4;

  for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
    if ((*i)->GetType() == JIT_IMAGE) {
      offset += GetItemSize() + 8;

      break;
    }
  }

  padding.left = padding.left - scrollx;
  padding.top = padding.top - scrolly;

  for (int i=0; i<(int)_items.size(); i++) {
    int dy = padding.top + (GetItemSize() + GetItemGap())*i;

    if ((dy + GetItemSize()) < 0 || dy > size.height) {
      continue;
    }

    Item *item = _items[i];

    if (item->IsEnabled() == true) {
      g->SetColor(GetTheme().GetIntegerParam("bg"));
    } else {
      g->SetColor(GetTheme().GetIntegerParam("bg.disable"));
    }

    if (_index != i) {
      if (_mode == JLBM_SINGLE_SELECTION) {  
        if (_selected_index == i) {  
          g->SetColor(GetTheme().GetIntegerParam("bg.select"));
        }
      } else if (_mode == JLBM_MULTI_SELECTION) {  
        if (item->IsSelected() == true) {  
          g->SetColor(GetTheme().GetIntegerParam("bg.select"));
        }
      }
    } else {
      g->SetColor(GetTheme().GetIntegerParam("bg.focus"));
    }

    g->FillRectangle({padding.left, padding.top + (GetItemSize() + GetItemGap())*i, w, GetItemSize()});

    if (font != nullptr) {
      g->SetFont(font);

      if (item->IsEnabled() == true) {
        g->SetColor(GetTheme().GetIntegerParam("fg"));
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg.disable"));
      }

      if (_index != i) {
        if (_mode == JLBM_SINGLE_SELECTION) {  
          if (_selected_index == i) {  
            g->SetColor(GetTheme().GetIntegerParam("fg.select"));
          }
        } else if (_mode == JLBM_MULTI_SELECTION) {  
          if (item->IsSelected() == true) {  
            g->SetColor(GetTheme().GetIntegerParam("fg.select"));
          }
        }
      } else {
        g->SetColor(GetTheme().GetIntegerParam("fg.focus"));
      }

      std::string text = _items[i]->GetValue();

      // if (_wrap == false) {
        text = font->TruncateString(text, "...", w - offset);
      // }

      g->DrawString(text, {padding.left + offset, padding.top + (GetItemSize() + GetItemGap())*i, w - offset, GetItemSize()}, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
    }
    
    if (_items[i]->GetType() == JIT_EMPTY) {
    } else if (_items[i]->GetType() == JIT_TEXT) {
    } else if (_items[i]->GetType() == JIT_IMAGE) {
      if (_items[i]->GetImage() != nullptr) {
        g->DrawImage(_items[i]->GetImage(), {GetHorizontalPadding(), padding.top + (GetItemSize() + GetItemGap())*i, GetItemSize(), GetItemSize()});
      }
    }

  }
}

void ListBox::IncrementLines(int lines)
{
  if (_items.size() == 0) {
    return;
  }

  int 
    old_index = _index;

  _index = _index - lines;

  if (_index < 0) {
    if (_loop == false) {
      _index = 0;
    } else {
      _index = (int)(_items.size()-1);
    }
  }

  jgui::jpoint_t<int> 
    scroll_location = GetScrollLocation();
  jgui::jsize_t<int>
    size = GetSize();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;

  if ((GetItemSize() + GetItemGap())*_index < scrolly) {
    ScrollToVisibleArea({scrollx, (std::max)(0, (GetItemSize() + GetItemGap())*_index), size.width, size.height}, this);
  } else if ((scrolly+size.height) < (GetItemSize() + GetItemGap())*(int)_index) {
    ScrollToVisibleArea({scrollx, (GetItemSize() + GetItemGap())*(_index + 1) - size.height + 2*GetItemGap(), size.width, size.height}, this);
  }

  if (_index != old_index) {
    Repaint();

    DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_UP)); 
  }
}

void ListBox::DecrementLines(int lines)
{
  if (_items.size() == 0) { 
    return;
  }

  int 
    old_index = _index;

  _index = _index + lines;

  if (_index >= (int)_items.size()) {
    if (_loop == false) {
      if (_items.size() > 0) {
        _index = _items.size()-1;
      } else {
        _index = 0;
      }
    } else {
      _index = 0;
    }
  }

  jgui::jpoint_t<int> 
    scroll_location = GetScrollLocation();
  jgui::jsize_t<int>
    size = GetSize();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;

  if ((scrolly + size.height) < (GetItemSize() + GetItemGap())*(int)(_index + 1)) {
    ScrollToVisibleArea({scrollx, (GetItemSize() + GetItemGap())*(_index + 1)-size.height+2*GetItemGap(), size.width, size.height}, this);
  } else if ((GetItemSize() + GetItemGap())*_index < scrolly) {
    ScrollToVisibleArea({scrollx, (std::max)(0, (GetItemSize() + GetItemGap())*_index), size.width, size.height}, this);
  }

  if (_index != old_index) {
    Repaint();

    DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_DOWN)); 
  }
}

jsize_t<int> ListBox::GetScrollDimension()
{
  jsize_t<int> 
    t = GetSize();
  jgui::Border
    border = GetTheme().GetBorder();

  // t.width = t.width;
  t.height = _items.size()*(GetItemSize() + GetItemGap()) + GetVerticalPadding();

  return t;
}

}
