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

namespace jgui {

Item::Item():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_enabled = true;
	_prefetch = NULL;
	_is_checked = false;
	_is_visible = true;
	_type = EMPTY_MENU_ITEM;
}

Item::Item(std::string value):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_enabled = true;
	_prefetch = NULL;
	_value = value;
	_is_checked = false;
	_is_visible = true;
	_type = TEXT_MENU_ITEM;
}

Item::Item(std::string value, std::string image):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_enabled = true;
	_prefetch = NULL;
	_image = image;
	_value = value;
	_is_checked = false;
	_is_visible = true;
	_type = IMAGE_MENU_ITEM;
	
	int w = 128,
			h = 128;

	_prefetch = new OffScreenImage(w, h);

	if (_prefetch->GetGraphics() != NULL) {
		if (_prefetch->GetGraphics()->DrawImage(_image, 0, 0, w, h) == false) {
			delete _prefetch;
			_prefetch = NULL;
		}
	}
}

Item::Item(std::string value, bool checked):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Item");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_enabled = true;
	_prefetch = NULL;
	_value = value;
	_is_checked = checked;
	_is_visible = true;
	_type = CHECK_MENU_ITEM;
}

Item::~Item()
{
	if (_prefetch != NULL) {
		delete _prefetch;
		_prefetch = NULL;
	}
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
	_enabled = b;
}

bool Item::GetEnabled()
{
	return _enabled;
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
	if (_type == jgui::CHECK_MENU_ITEM) {
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

jgui::OffScreenImage * Item::GetImage()
{
	return _prefetch;
}

jmenuitem_type_t Item::GetType()
{
	return _type;
}

ItemComponent::ItemComponent(int x, int y, int width, int height):
  jgui::Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ItemComponent");

	_index = 0;
	_loop = false;
	
	Theme *theme = ThemeManager::GetInstance()->GetTheme();

	theme->Update((Component *)this);
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

jcolor_t ItemComponent::GetItemColor()
{
	return _item_color;
}

jcolor_t ItemComponent::GetItemFocusColor()
{
	return _focus_item_color;
}

jcolor_t ItemComponent::GetItemForegroundColor()
{
	return _item_fgcolor;
}

jcolor_t ItemComponent::GetSelectedItemColor()
{
	return _selected_item_color;
}

jcolor_t ItemComponent::GetSelectedItemForegroundColor()
{
	return _selected_item_fgcolor;
}

jcolor_t ItemComponent::GetItemForegroundFocusColor()
{
	return _focus_item_fgcolor;
}

void ItemComponent::SetItemColor(jcolor_t color)
{
	SetItemColor(color.red, color.green, color.blue, color.alpha);
}

void ItemComponent::SetItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_color.red = red;
	_item_color.green = green;
	_item_color.blue = blue;
	_item_color.alpha = alpha;
}

void ItemComponent::SetItemFocusColor(jcolor_t color)
{
	SetItemFocusColor(color.red, color.green, color.blue, color.alpha);
}

void ItemComponent::SetItemFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_focus_item_color.red = red;
	_focus_item_color.green = green;
	_focus_item_color.blue = blue;
	_focus_item_color.alpha = alpha;
}

void ItemComponent::SetSelectedItemColor(jcolor_t color)
{
	SetSelectedItemColor(color.red, color.green, color.blue, color.alpha);
}

void ItemComponent::SetSelectedItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_selected_item_color.red = red;
	_selected_item_color.green = green;
	_selected_item_color.blue = blue;
	_selected_item_color.alpha = alpha;
}

void ItemComponent::SetSelectedItemForegroundColor(jcolor_t color)
{
	SetSelectedItemForegroundColor(color.red, color.green, color.blue, color.alpha);
}

void ItemComponent::SetSelectedItemForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_selected_item_fgcolor.red = red;
	_selected_item_fgcolor.green = green;
	_selected_item_fgcolor.blue = blue;
	_selected_item_fgcolor.alpha = alpha;
}

void ItemComponent::SetItemForegroundColor(jcolor_t color)
{
	SetItemForegroundColor(color.red, color.green, color.blue, color.alpha);
}

void ItemComponent::SetItemForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_fgcolor.red = red;
	_item_fgcolor.green = green;
	_item_fgcolor.blue = blue;
	_item_fgcolor.alpha = alpha;
}

void ItemComponent::SetItemForegroundFocusColor(jcolor_t color)
{
	SetItemForegroundFocusColor(color.red, color.green, color.blue, color.alpha);
}

void ItemComponent::SetItemForegroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_focus_item_fgcolor.red = red;
	_focus_item_fgcolor.green = green;
	_focus_item_fgcolor.blue = blue;
	_focus_item_fgcolor.alpha = alpha;
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
		throw jcommon::RuntimeException("Index out of range");
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
		throw jcommon::RuntimeException("Index out of range");
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
		throw jcommon::RuntimeException("Index out of range");
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

	if (std::find(_select_listeners.begin(), _select_listeners.end(), listener) == _select_listeners.end()) {
		_select_listeners.push_back(listener);
	}
}

void ItemComponent::RemoveSelectListener(SelectListener *listener)
{
	if (listener == NULL) {
		return;
	}

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

	int k=0;

	while (k++ < (int)_select_listeners.size()) {
		SelectListener *listener = _select_listeners[k-1];

		if (event->GetType() == ACTION_ITEM) {
			listener->ItemSelected(event);
		} else {
			listener->ItemChanged(event);
		}
	}

	/*
	for (std::vector<SelectListener *>::iterator i=_select_listeners.begin(); i!=_select_listeners.end(); i++) {
		if (event->GetType() == ACTION_ITEM) {
			(*i)->ItemSelected(event);
		} else {
			(*i)->ItemChanged(event);
		}
	}
	*/

	delete event;
}

std::vector<SelectListener *> & ItemComponent::GetSelectListeners()
{
	return _select_listeners;
}

}

