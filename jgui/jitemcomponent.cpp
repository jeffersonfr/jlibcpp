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
#include "Stdafx.h"
#include "jitemcomponent.h"
#include "jruntimeexception.h"
#include "jthememanager.h"
#include "jselectlistener.h"
#include "joutofboundsexception.h"

namespace jgui {

Item::Item():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_is_enabled = true;
	_prefetch = NULL;
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
	_prefetch = NULL;
	_value = value;
	_is_checked = false;
	_is_visible = true;
	_type = JIT_TEXT;
}

Item::Item(std::string value, std::string image):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_is_enabled = true;
	_prefetch = NULL;
	_image = image;
	_value = value;
	_is_checked = false;
	_is_visible = true;
	_type = JIT_IMAGE;
	
	_prefetch = Image::CreateImage(_image);
}

Item::Item(std::string value, bool checked):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_is_enabled = true;
	_prefetch = NULL;
	_value = value;
	_is_checked = checked;
	_is_visible = true;
	_type = JIT_CHECK;
}

Item::~Item()
{
	if (_prefetch != NULL) {
		delete _prefetch;
		_prefetch = NULL;
	}
}

jcommon::Object * Item::Clone()
{
	Item *item = new Item(*this);

	if (_prefetch != NULL) {
		item->_prefetch = jgui::Image::CreateImage(item->_prefetch);
	}

	for (std::vector<Item *>::iterator i=_childs.begin(); i!=_childs.end(); i++) {
		item->_childs.push_back(dynamic_cast<Item *>((*i)->Clone()));
	}

	return (jcommon::Object *)item;
}

std::vector<Item *> & Item::GetChilds()
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
		throw jcommon::RuntimeException("Item cannot accept childs");
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
	return _prefetch;
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

Color & ItemComponent::GetItemColor()
{
	return _item_color;
}

Color & ItemComponent::GetItemForegroundColor()
{
	return _item_fgcolor;
}

Color & ItemComponent::GetItemFocusColor()
{
	return _focus_item_color;
}

Color & ItemComponent::GetItemForegroundFocusColor()
{
	return _focus_item_fgcolor;
}

Color & ItemComponent::GetSelectedItemColor()
{
	return _selected_item_color;
}

Color & ItemComponent::GetSelectedItemForegroundColor()
{
	return _selected_item_fgcolor;
}

Color & ItemComponent::GetDisabledItemColor()
{
	return _disabled_item_color;
}

Color & ItemComponent::GetDisabledItemForegroundColor()
{
	return _disabled_item_fgcolor;
}

void ItemComponent::SetItemColor(int red, int green, int blue, int alpha)
{
	_item_color = Color(red, green, blue, alpha);
}

void ItemComponent::SetItemForegroundColor(int red, int green, int blue, int alpha)
{
	_item_fgcolor = Color(red, green, blue, alpha);
}

void ItemComponent::SetItemFocusColor(int red, int green, int blue, int alpha)
{
	_focus_item_color = Color(red, green, blue, alpha);
}

void ItemComponent::SetItemForegroundFocusColor(int red, int green, int blue, int alpha)
{
	_focus_item_fgcolor = Color(red, green, blue, alpha);
}

void ItemComponent::SetSelectedItemColor(int red, int green, int blue, int alpha)
{
	_selected_item_color = Color(red, green, blue, alpha);
}

void ItemComponent::SetSelectedItemForegroundColor(int red, int green, int blue, int alpha)
{
	_selected_item_fgcolor = Color(red, green, blue, alpha);
}

void ItemComponent::SetDisabledItemColor(int red, int green, int blue, int alpha)
{
	_disabled_item_color = Color(red, green, blue, alpha);
}

void ItemComponent::SetDisabledItemForegroundColor(int red, int green, int blue, int alpha)
{
	_disabled_item_fgcolor = Color(red, green, blue, alpha);
}

void ItemComponent::SetItemColor(const Color &color)
{
	_item_color = color;
}

void ItemComponent::SetItemForegroundColor(const Color &color)
{
	_item_fgcolor = color;
}

void ItemComponent::SetItemFocusColor(const Color &color)
{
	_focus_item_color = color;
}

void ItemComponent::SetItemForegroundFocusColor(const Color &color)
{
	_focus_item_fgcolor = color;
}

void ItemComponent::SetSelectedItemColor(const Color &color)
{
	_selected_item_color = color;
}

void ItemComponent::SetSelectedItemForegroundColor(const Color &color)
{
	_selected_item_fgcolor = color;
}

void ItemComponent::SetDisabledItemColor(const Color &color)
{
	_disabled_item_color = color;
}

void ItemComponent::SetDisabledItemForegroundColor(const Color &color)
{
	_disabled_item_fgcolor = color;
}

int ItemComponent::GetItemsSize()
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
		throw jcommon::OutOfBoundsException("Index out of range");
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

void ItemComponent::AddImageItem(std::string text, std::string image)
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
	if (index < 0 || index > (int)_items.size()) {
		throw jcommon::OutOfBoundsException("Index out of range");
	}

	if (item == NULL) {
		return;
	}

	{
		// jthread::AutoLock lock(&_component_mutex);

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
	if (item == NULL) {
		return;
	}

	// jthread::AutoLock lock(&_component_mutex);
		
	std::vector<Item *>::iterator i = std::find(_items.begin(), _items.end(), item);
	
	if (i != _items.end()) {
		_items.erase(i);
	}
}

void ItemComponent::RemoveItem(int index)
{
	if (index < 0 || index >= (int)_items.size()) {
		throw jcommon::OutOfBoundsException("Index out of range");
	}

	// jthread::AutoLock lock(&_component_mutex);
	
	_items.erase(_items.begin()+index);
}

void ItemComponent::RemoveItems()
{
	// jthread::AutoLock lock(&_component_mutex);

	_items.clear();
}

Item * ItemComponent::GetItem(int index)
{
	if (index < 0 || index >= (int)_items.size()) {
		return NULL;
	}

	return *(_items.begin()+index);
}

std::vector<Item *> & ItemComponent::GetItems()
{
	return _items;
}

Item * ItemComponent::GetCurrentItem()
{
	if (_items.size() == 0) {
		return NULL;
	}

	return *(_items.begin()+_index);
}

int ItemComponent::GetCurrentIndex()
{
	return _index;
}

void ItemComponent::RegisterSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_select_listener_mutex);

	if (std::find(_select_listeners.begin(), _select_listeners.end(), listener) == _select_listeners.end()) {
		_select_listeners.push_back(listener);
	}
}

void ItemComponent::RemoveSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

	jthread::AutoLock lock(&_select_listener_mutex);

	std::vector<SelectListener *>::iterator i = std::find(_select_listeners.begin(), _select_listeners.end(), listener);

	if (i != _select_listeners.end()) {
		_select_listeners.erase(i);
	}
}

void ItemComponent::DispatchSelectEvent(SelectEvent *event)
{
	if (event == NULL) {
		return;
	}

	std::vector<SelectListener *> listeners;
	
	_select_listener_mutex.Lock();

	listeners = _select_listeners;

	_select_listener_mutex.Unlock();

	for (std::vector<SelectListener *>::iterator i=listeners.begin(); i!=listeners.end() && event->IsConsumed() == false; i++) {
		SelectListener *listener = (*i);

		if (event->GetType() == JSET_ACTION) {
			listener->ItemSelected(event);
		} else {
			listener->ItemChanged(event);
		}
	}

	delete event;
}

std::vector<SelectListener *> & ItemComponent::GetSelectListeners()
{
	return _select_listeners;
}

}

