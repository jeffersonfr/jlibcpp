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
#include "jgui/jitemcomponent.h"
#include "jevent/jselectlistener.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jillegalargumentexception.h"

#include <algorithm>

namespace jgui {

Item::Item():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Item");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;
  _image = nullptr;
  _is_enabled = true;
  _is_checked = false;
  _is_visible = true;
  _type = JIT_EMPTY;
}

Item::Item(std::string value):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Item");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _is_enabled = true;
  _value = value;
  _image = nullptr;
  _is_checked = false;
  _is_visible = true;
  _type = JIT_TEXT;
}

Item::Item(std::string value, jgui::Image *image):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Item");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _is_enabled = true;
  _image = image;
  _value = value;
  _is_checked = false;
  _is_visible = true;
  _type = JIT_IMAGE;
}

Item::Item(std::string value, bool checked):
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::Item");

  _halign = JHA_CENTER;
  _valign = JVA_CENTER;

  _is_enabled = true;
  _value = value;
  _image = nullptr;
  _is_checked = checked;
  _is_visible = true;
  _type = JIT_CHECK;
}

Item::~Item()
{
  if (_image != nullptr) {
    delete _image;
    _image = nullptr;
  }
}

jcommon::Object * Item::Clone()
{
  Item *item = new Item(*this);

  for (std::vector<Item *>::iterator i=_childs.begin(); i!=_childs.end(); i++) {
    item->_childs.push_back(dynamic_cast<Item *>((*i)->Clone()));
  }

  return (jcommon::Object *)item;
}

const std::vector<Item *> & Item::GetChilds()
{
  return _childs;
}

void Item::SetHorizontalAlign(jhorizontal_align_t align)
{
  _halign = align;
}

jhorizontal_align_t Item::GetHorizontalAlign()
{
  return _halign;
}

void Item::SetVerticalAlign(jvertical_align_t align)
{
  _valign = align;
}

jvertical_align_t Item::GetVerticalAlign()
{
  return _valign;
}

void Item::SetEnabled(bool b)
{
  _is_enabled = b;
}

bool Item::IsEnabled()
{
  return _is_enabled;
}

void Item::SetVisible(bool b)
{
  _is_visible = b;
}

bool Item::IsVisible()
{
  return _is_visible;
}

bool Item::IsSelected()
{
  return _is_checked;
}

void Item::SetSelected(bool b)
{
  if (_is_checked == b) {
    return;
  }

  _is_checked = b;
}

void Item::AddChild(Item *item)
{
  if (_type == jgui::JIT_CHECK) {
    throw jexception::IllegalArgumentException("Item cannot accept childs");
  }

  _childs.push_back(item);
}

void Item::AddChild(Item *item, int index)
{
  if (index > (int)_childs.size()) {
    index = _childs.size();
  }

  _childs.insert(_childs.begin()+index, item);
}

void Item::AddChilds(std::vector<Item *> &items)
{
  for (std::vector<Item *>::iterator i=items.begin(); i!=items.end(); i++) {
    _childs.push_back((*i));
  }
}

void Item::RemoveChild(int index)
{
  if (_childs.size() > 0 && index > 0 && index < (int)_childs.size()) {
    _childs.erase(_childs.begin()+index);
  }
}

void Item::RemoveChild(Item *item)
{
  std::vector<Item *>::iterator i = std::find(_childs.begin(), _childs.end(), item);

  if (i != _childs.end()) {
    _childs.erase(i);
  }
}

int Item::GetChildsSize()
{
  return _childs.size();
}

std::string Item::GetValue()
{
  return _value;
}

jgui::Image * Item::GetImage()
{
  return _image;
}

jitem_type_t Item::GetType()
{
  return _type;
}

ItemComponent::ItemComponent():
  jcommon::Object()
{
  jcommon::Object::SetClassName("jgui::ItemComponent");

  _index = 0;
  _loop = false;
  _item_size = 32;
  _item_gap = 2;
}

ItemComponent::~ItemComponent()
{
  for (std::vector<Item *>::iterator i=_internal.begin(); i!=_internal.end(); i++) {
    delete (*i);
  }
}

void ItemComponent::AddInternalItem(Item *item)
{
  std::vector<Item *>::iterator i=std::find(_internal.begin(), _internal.end(), item);

  if (i == _internal.end()) {
    _internal.push_back(item);
  }
}

int ItemComponent::GetItemSize()
{
  return _item_size;
}

int ItemComponent::GetItemGap()
{
  return _item_gap;
}

void ItemComponent::SetItemSize(int size)
{
  _item_size = size;
}

void ItemComponent::SetItemGap(int gap)
{
  _item_gap = gap;
}

int ItemComponent::GetItemsCount()
{
  return _items.size();
}

void ItemComponent::SetLoop(bool loop)
{
  _loop = loop;
}

void ItemComponent::SetCurrentIndex(int i)
{
  if (i < 0 || i >= (int)_items.size()) {
    throw jexception::OutOfBoundsException("Index out of range");
  }

  _index = i;
}

void ItemComponent::AddEmptyItem()
{
  Item *item = new Item();

  AddInternalItem(item);
  AddItem(item);
}

void ItemComponent::AddTextItem(std::string text)
{
  Item *item = new Item(text);

  AddInternalItem(item);
  AddItem(item);
}

void ItemComponent::AddImageItem(std::string text, jgui::Image *image)
{
  Item *item = new Item(text, image);

  AddInternalItem(item);
  AddItem(item);
}

void ItemComponent::AddCheckedItem(std::string text, bool checked)
{
  Item *item = new Item(text, checked);

  AddInternalItem(item);
  AddItem(item);
}

void ItemComponent::AddItem(Item *item)
{
  AddItem(item, _items.size());
}

void ItemComponent::AddItem(Item *item, int index)
{
  if (item == nullptr) {
    return;
  }

  if (index < 0 || index > (int)_items.size()) {
    throw jexception::OutOfBoundsException("Index out of range");
  }

  {
     std::lock_guard<std::mutex> guard(_itemcomponent_mutex);

    _items.insert(_items.begin()+index, item);
  }
}

void ItemComponent::AddItems(std::vector<Item *> &items)
{
  for (std::vector<Item *>::iterator i=items.begin(); i!=items.end(); i++) {
    AddItem((*i));
  }
}

void ItemComponent::RemoveItem(Item *item)
{
  if (item == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_itemcomponent_mutex);
    
  _items.erase(std::remove(_items.begin(), _items.end(), item), _items.end());
}

void ItemComponent::RemoveItem(int index)
{
  if (index < 0 || index >= (int)_items.size()) {
    throw jexception::OutOfBoundsException("Index out of range");
  }

   std::lock_guard<std::mutex> guard(_itemcomponent_mutex);
  
  _items.erase(_items.begin()+index);
}

void ItemComponent::RemoveItems()
{
   std::lock_guard<std::mutex> guard(_itemcomponent_mutex);

  _items.clear();
}

Item * ItemComponent::GetItem(int index)
{
  if (index < 0 || index >= (int)_items.size()) {
    return nullptr;
  }

  return *(_items.begin()+index);
}

const std::vector<Item *> & ItemComponent::GetItems()
{
  return _items;
}

Item * ItemComponent::GetCurrentItem()
{
  if (_items.size() == 0) {
    return nullptr;
  }

  return *(_items.begin()+_index);
}

int ItemComponent::GetCurrentIndex()
{
  return _index;
}

void ItemComponent::RegisterSelectListener(jevent::SelectListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_select_listener_mutex);

  if (std::find(_select_listeners.begin(), _select_listeners.end(), listener) == _select_listeners.end()) {
    _select_listeners.push_back(listener);
  }
}

void ItemComponent::RemoveSelectListener(jevent::SelectListener *listener)
{
  if (listener == nullptr) {
    return;
  }

   std::lock_guard<std::mutex> guard(_select_listener_mutex);

  _select_listeners.erase(std::remove(_select_listeners.begin(), _select_listeners.end(), listener), _select_listeners.end());
}

void ItemComponent::DispatchSelectEvent(jevent::SelectEvent *event)
{
  if (event == nullptr) {
    return;
  }

  _select_listener_mutex.lock();

  std::vector<jevent::SelectListener *> listeners = _select_listeners;

  _select_listener_mutex.unlock();

  for (std::vector<jevent::SelectListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
    jevent::SelectListener *listener = (*i);

    if (event->GetType() == jevent::JSET_ACTION) {
      listener->ItemSelected(event);
    } else {
      listener->ItemChanged(event);
    }
  }

  delete event;
}

const std::vector<jevent::SelectListener *> & ItemComponent::GetSelectListeners()
{
  return _select_listeners;
}

}

