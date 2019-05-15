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
#include "jgui/jtreelistview.h"
#include "jlogger/jloggerlib.h"
#include "jexception/joutofboundsexception.h"

namespace jgui {

TreeListView::TreeListView(int x, int y, int width, int height):
  Component(x, y, width, height),
  ItemComponent()
{
  jcommon::Object::SetClassName("jgui::TreeListView");

  _item_gap = 4;
  _pressed = false;
  _item_size = DEFAULT_COMPONENT_HEIGHT;
  _selected_index = -1;

  SetFocusable(true);
}

TreeListView::~TreeListView() 
{
}

void TreeListView::AddEmptyItem()
{
  Item *item = new Item();

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

void TreeListView::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

void TreeListView::AddImageItem(std::string text, jgui::Image *image)
{
  Item *item = new Item(text, image);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

void TreeListView::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  item->SetHorizontalAlign(JHA_LEFT);
    
  AddInternalItem(item);
  AddItem(item);
}

int TreeListView::GetItemSize()
{
  return _item_size;
}

void TreeListView::SetItemSize(int size)
{
  if (size <= 0) {
    return;
  }

  _item_size = size;
  
  Repaint();
}

void TreeListView::SetCurrentIndex(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    throw jexception::OutOfBoundsException("Index out of bounds exception");
  }

  jgui::jpoint_t
    slocation = GetScrollLocation();

  _index = i;

  SetScrollLocation(slocation.x, _index*(GetItemSize()+GetItemGap()));
}

bool TreeListView::IsSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return false;
  }

  if (_selected_index == i) {
    return true;
  }

  return false;
}

void TreeListView::SetSelected(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  if (_selected_index == i) {
    _selected_index = -1;
  } else {
    _selected_index = i;
  }

  Repaint();
}

void TreeListView::Select(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  _selected_index = i;

  Repaint();
}

void TreeListView::Deselect(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    return;
  }

  Item *item = _items[i];

  if (item->IsEnabled() == false) {
    return;
  }

  _selected_index = -1;

  Repaint();
}

void TreeListView::Expand(Item *item)
{
  _expanded_items[item] = true;

  Repaint();
}

void TreeListView::Collapse(Item *item)
{
  _expanded_items[item] = false;

  Repaint();
}

bool TreeListView::IsExpanded(Item *item)
{
  return _expanded_items[item];
}

void TreeListView::ExpandAll(Item *item)
{
  for (std::map<Item *, bool>::iterator i=_expanded_items.begin(); i!=_expanded_items.end(); i++) {
    _expanded_items[i->first] = true;
  }

  Repaint();
}

void TreeListView::CollapseAll(Item *item)
{
  for (std::map<Item *, bool>::iterator i=_expanded_items.begin(); i!=_expanded_items.end(); i++) {
    _expanded_items[i->first] = false;
  }

  Repaint();
}

int TreeListView::GetItemGap()
{
  return _item_gap;
}

void TreeListView::SetItemGap(int gap)
{
  _item_gap = gap;
}

int TreeListView::GetSelectedIndex()
{
  return _selected_index;
}

bool TreeListView::KeyPressed(jevent::KeyEvent *event)
{
  if (Component::KeyPressed(event) == true) {
    return true;
  }

  if (IsEnabled() == false) {
    return false;
  }

  jevent::jkeyevent_symbol_t action = event->GetSymbol();

  bool catched = false;

  if (action == jevent::JKS_CURSOR_UP) {
    IncrementLines(1);
    
    catched = true;
  } else if (action == jevent::JKS_PAGE_UP) {
    // TODO:: IncrementLines((_size.height-2*(bordersize+_vertical_gap))/(_item_size+_item_gap));
    
    catched = true;
  } else if (action == jevent::JKS_CURSOR_DOWN) {
    DecrementLines(1);

    catched = true;
  } else if (action == jevent::JKS_PAGE_DOWN) {
    // TODO:: DecrementLines((_size.height-2*(bordersize+_vertical_gap))/(_item_size+_item_gap));

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

bool TreeListView::MousePressed(jevent::MouseEvent *event)
{
  if (Component::MousePressed(event) == true) {
    return true;
  }

  return false;
}

bool TreeListView::MouseReleased(jevent::MouseEvent *event)
{
  if (Component::MouseReleased(event) == true) {
    return true;
  }

  return false;
}
  
bool TreeListView::MouseMoved(jevent::MouseEvent *event)
{
  if (Component::MouseMoved(event) == true) {
    return true;
  }

  return false;
}

bool TreeListView::MouseWheel(jevent::MouseEvent *event)
{
  if (Component::MouseWheel(event) == true) {
    return true;
  }
  
  jgui::jpoint_t
    slocation = GetScrollLocation();

  SetScrollLocation(slocation.x, slocation.y + _item_size*event->GetClicks());

  return true;
}

void TreeListView::Paint(Graphics *g)
{
  JDEBUG(JINFO, "paint\n");

  Component::Paint(g);

  Theme *theme = GetTheme();
  
  if (theme == nullptr) {
    return;
  }

  jgui::Font 
    *font = theme->GetFont("component.font");
  jgui::Color 
    bg = theme->GetIntegerParam("component.bg"),
    fg = theme->GetIntegerParam("component.fg"),
    fgfocus = theme->GetIntegerParam("component.fg.focus"),
    fgdisable = theme->GetIntegerParam("component.fg.disable");
  jgui::jsize_t<int>
    size = GetSize();
  int
    hg = theme->GetIntegerParam("component.hgap"),
    vg = theme->GetIntegerParam("component.vgap");
  int
    x = hg + theme->GetIntegerParam("component.border.size"),
    y = vg + theme->GetIntegerParam("component.border.size"),
    w = size.width - 2*x;
    // h = size.height - 2*y;
  int
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap");
  jpoint_t scroll_location = GetScrollLocation();
  int 
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;
    // scrollw = (IsScrollableY() == true)?(ss + sg):0;
  int offset = 4;

  for (std::vector<jgui::Item *>::iterator i=_items.begin(); i!=_items.end(); i++) {
    if ((*i)->GetType() == JIT_IMAGE) {
      offset += is + 8;

      break;
    }
  }

  // TODO:: tentar ajeitar
  x = x - scrollx;
  y = y - scrolly;

  for (int i=0; i<(int)_items.size(); i++) {
    int dy = y+(is + ig)*i;

    if ((dy + is) < 0 || dy > size.height) {
      continue;
    }

    Item *item = _items[i];

    if (item->IsEnabled() == true) {
      g->SetColor(theme->GetIntegerParam("item.fg"));
    } else {
      g->SetColor(theme->GetIntegerParam("item.fg.disable"));
    }

    if (_index != i) {
      if (_selected_index == i) {  
        g->SetColor(theme->GetIntegerParam("item.fg.select"));
      }
    } else {
      g->SetColor(theme->GetIntegerParam("item.fg.focus"));
    }

    g->FillRectangle({x, y+(is + ig)*i, w, is});

    if (_selected_index == i) {
      g->SetColor(theme->GetIntegerParam("item.fg.select"));
    }

    if (_items[i]->GetType() == JIT_EMPTY) {
    } else if (_items[i]->GetType() == JIT_TEXT) {
    } else if (_items[i]->GetType() == JIT_IMAGE) {
      if (_items[i]->GetImage() != nullptr) {
        g->DrawImage(_items[i]->GetImage(), {hg, y + (is + ig)*i, is, is});
      }
    }

    if (font != nullptr) {
      g->SetFont(font);

      if (IsEnabled() == true) {
        if (HasFocus() == true) {
          g->SetColor(fgfocus);
        } else {
          g->SetColor(fg);
        }
      } else {
        g->SetColor(fgdisable);
      }

      std::string text = _items[i]->GetValue();

      // if (_wrap == false) {
        text = font->TruncateString(text, "...", w - offset);
      // }

      g->DrawString(text, {x + offset, y + (is + ig)*i, w - offset, is}, _items[i]->GetHorizontalAlign(), _items[i]->GetVerticalAlign());
    }
  }
}

void TreeListView::IncrementLines(int lines)
{
  if (_items.size() == 0) {
    return;
  }

  jgui::jsize_t<int>
    size = GetSize();
  jpoint_t 
    scroll_location = GetScrollLocation();
  int
    scrollx = (IsScrollableX() == true)?scroll_location.x:0,
    scrolly = (IsScrollableY() == true)?scroll_location.y:0;
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

  if ((_item_size+_item_gap)*_index < scrolly) {
    ScrollToVisibleArea(scrollx, (std::max)(0, (_item_size+_item_gap)*_index), size.width, size.height, this);
  } else if ((scrolly+size.height) < (_item_size+_item_gap)*(int)_index) {
    ScrollToVisibleArea(scrollx, (_item_size+_item_gap)*(_index+1)-size.height+2*_item_gap, size.width, size.height, this);
  }

  if (_index != old_index) {
    Repaint();

    DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_UP)); 
  }
}

void TreeListView::DecrementLines(int lines)
{
  if (_items.size() == 0) { 
    return;
  }

  jgui::jsize_t<int>
    size = GetSize();
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

  jpoint_t scroll_location = GetScrollLocation();
  int scrollx = (IsScrollableX() == true)?scroll_location.x:0,
      scrolly = (IsScrollableY() == true)?scroll_location.y:0;

  if ((scrolly+size.height) < (_item_size+_item_gap)*(int)(_index+1)) {
    ScrollToVisibleArea(scrollx, (_item_size+_item_gap)*(_index+1)-size.height+2*_item_gap, size.width, size.height, this);
  } else if ((_item_size+_item_gap)*_index < scrolly) {
    ScrollToVisibleArea(scrollx, (std::max)(0, (_item_size+_item_gap)*_index), size.width, size.height, this);
  }

  if (_index != old_index) {
    Repaint();

    DispatchSelectEvent(new jevent::SelectEvent(_items[_index], _index, jevent::JSET_DOWN)); 
  }
}

jsize_t<int> TreeListView::GetScrollDimension()
{
  Theme *theme = GetTheme();

  jsize_t<int> t {0, 0};

  if (theme == nullptr) {
    return t;
  }

  int
    bs = theme->GetIntegerParam("component.border.size");
  int
    // hg = theme->GetIntegerParam("component.hgap"),
    vg = theme->GetIntegerParam("component.vgap");
  int
    is = theme->GetIntegerParam("item.size"),
    ig = theme->GetIntegerParam("item.gap");

  t = GetSize();

  // t.width = t.width;
  t.height = _items.size()*(is + ig) + 2*(vg + bs);

  return  t;
}

}
