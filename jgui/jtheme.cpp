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
#include "jtheme.h"
#include "jwindow.h"
#include "jitemcomponent.h"

namespace jgui {

Theme::Theme():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Theme");

	SetWindowBackgroundColor(0x35, 0x55, 0x75, 0xff);
	SetWindowForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetWindowBorderColor(0x35, 0x55, 0x75, 0xff);

	SetComponentBackgroundColor(0x15, 0x35, 0x55, 0xff);
	SetComponentForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetComponentBackgroundFocusColor(0x06, 0x65, 0xaa, 0xff);
	SetComponentForegroundFocusColor(0xf0, 0xf0, 0xf0, 0xff);
	SetComponentBorderColor(0x60, 0x60, 0x80, 0xff);
	SetComponentBorderFocusColor(0xf0, 0xf0, 0xf0, 0xff);
	SetComponentScrollbarColor(0x80, 0x80, 0xe0, 0xff);
	SetComponentDisabledBackgroundColor(0x05, 0x25, 0x35, 0xff);
	SetComponentDisabledForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetComponentDisabledBorderColor(0x06, 0x65, 0xaa, 0xff);
	
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
	SetItemForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetItemFocusColor(0x57, 0x67, 0x7e, 0xff);
	SetItemForegroundFocusColor(0xf0, 0xf0, 0xf0, 0xff);
	SetSelectedItemColor(0x80, 0x80, 0x80, 0xff);
	SetSelectedItemForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetDisabledItemColor(0x60, 0x60, 0x60, 0xff);
	SetDisabledItemForegroundColor(0xf0, 0xf0, 0xf0, 0xff);

	_component_border_size = 1;
	_component_border = JCB_LINE;

	_component_font = NULL;

	_insets.left = 8;
	_insets.top = 42;
	_insets.right = 8;
	_insets.bottom = 8;
	
	_window_font = NULL;
	_window_border_size = 4;
	_window_border = JCB_RAISED_GRADIENT;
}

Theme::~Theme()
{
}

void Theme::Update(Component *parent)
{
	if (parent == NULL) {
		return;
	}

	if (parent->IsThemeEnabled() == false) {
		return;
	}

	std::vector<Component *> stack;

	stack.push_back(parent);

	while (stack.size() > 0) {
		Container *container = dynamic_cast<jgui::Container *>(*stack.begin());

		if (container != NULL) {
			for (std::vector<Component *>::iterator i=container->GetComponents().begin(); i!=container->GetComponents().end(); i++) {
				Container *internal_container = dynamic_cast<jgui::Container *>(*i);

				if (internal_container != NULL) {
					stack.push_back(internal_container);
				}
			}
		} 
		
		Component *c = *stack.begin();

		c->SetBackgroundColor(_component_bgcolor);
		c->SetForegroundColor(_component_fgcolor);
		c->SetBorderColor(_component_border_color);
		c->SetBorderFocusColor(_component_border_focus_color);
		c->SetBackgroundFocusColor(_component_focus_bgcolor);
		c->SetForegroundFocusColor(_component_focus_fgcolor);
		c->SetScrollbarColor(_component_scrollbar_color);
		c->SetDisabledBackgroundColor(_component_disabled_bgcolor);
		c->SetDisabledForegroundColor(_component_disabled_fgcolor);
		c->SetDisabledBorderColor(_component_disabled_border_color);

		c->SetBorderSize(_component_border_size);
		c->SetBorder(_component_border);

		if (_component_font != NULL) {
			c->SetFont(_component_font);
		}

		ItemComponent *ic = dynamic_cast<ItemComponent *>(c);

		if (ic != NULL) {
			ic->SetItemColor(_item_color);
			ic->SetItemForegroundColor(_item_fgcolor);
			ic->SetItemFocusColor(_item_focus_color);
			ic->SetItemForegroundFocusColor(_item_focus_fgcolor);
			ic->SetSelectedItemColor(_item_selected_color);
			ic->SetSelectedItemForegroundColor(_item_selected_fgcolor);
			ic->SetDisabledItemColor(_item_disabled_color);
			ic->SetDisabledItemForegroundColor(_item_disabled_fgcolor);
		}

		stack.erase(stack.begin());
	}
	
	Window *w = dynamic_cast<Window *>(parent);

	if (w != NULL) {
		w->SetBackgroundColor(_window_bgcolor);
		w->SetForegroundColor(_window_fgcolor);
		w->SetBorderColor(_window_border_color);
		w->SetBorderSize(_window_border_size);
		w->SetBorder(_window_border);

		w->SetInsets(_insets.left, _insets.top, _insets.right, _insets.bottom);

		if (_window_font != NULL) {
			w->SetFont(_window_font);
		}
	}

}

void Theme::SetWindowBackgroundColor(int red, int green, int blue, int alpha)
{
	_window_bgcolor = Color(red, green, blue, alpha);
}

void Theme::SetWindowForegroundColor(int red, int green, int blue, int alpha)
{
	_window_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetWindowBorderColor(int red, int green, int blue, int alpha)
{
	_window_border_color = Color(red, green, blue, alpha);
}

void Theme::SetComponentBackgroundColor(int red, int green, int blue, int alpha)
{
	_component_bgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentForegroundColor(int red, int green, int blue, int alpha)
{
	_component_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentBorderColor(int red, int green, int blue, int alpha)
{
	_component_border_color = Color(red, green, blue, alpha);
}

void Theme::SetComponentBorderFocusColor(int red, int green, int blue, int alpha)
{
	_component_border_focus_color = Color(red, green, blue, alpha);
}

void Theme::SetComponentBackgroundFocusColor(int red, int green, int blue, int alpha)
{
	_component_focus_bgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentForegroundFocusColor(int red, int green, int blue, int alpha)
{
	_component_focus_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentScrollbarColor(int red, int green, int blue, int alpha)
{
	_component_scrollbar_color = Color(red, green, blue, alpha);
}

void Theme::SetComponentDisabledBackgroundColor(int red, int green, int blue, int alpha)
{
	_component_disabled_bgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentDisabledForegroundColor(int red, int green, int blue, int alpha)
{
	_component_disabled_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentDisabledBorderColor(int red, int green, int blue, int alpha)
{
	_component_disabled_border_color = Color(red, green, blue, alpha);
}

void Theme::SetItemColor(int red, int green, int blue, int alpha)
{
	_item_color = Color(red, green, blue, alpha);
}

void Theme::SetItemForegroundColor(int red, int green, int blue, int alpha)
{
	_item_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetItemFocusColor(int red, int green, int blue, int alpha)
{
	_item_focus_color = Color(red, green, blue, alpha);
}

void Theme::SetItemForegroundFocusColor(int red, int green, int blue, int alpha)
{
	_item_focus_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetSelectedItemColor(int red, int green, int blue, int alpha)
{
	_item_selected_color = Color(red, green, blue, alpha);
}

void Theme::SetSelectedItemForegroundColor(int red, int green, int blue, int alpha)
{
	_item_selected_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetDisabledItemColor(int red, int green, int blue, int alpha)
{
	_item_disabled_color = Color(red, green, blue, alpha);
}

void Theme::SetDisabledItemForegroundColor(int red, int green, int blue, int alpha)
{
	_item_disabled_fgcolor = Color(red, green, blue, alpha);
}

void Theme::SetComponentBorder(jcomponent_border_t border)
{
	_component_border = border;
}

void Theme::SetWindowBorder(jcomponent_border_t border)
{
	_window_border = border;
}

void Theme::SetWindowInsets(int left, int top, int right, int bottom)
{
	_insets.left = left;
	_insets.top = top;
	_insets.right = right ;
	_insets.bottom = bottom;
}

void Theme::SetWindowBorderSize(int size)
{
	_window_border_size = size;
}

void Theme::SetComponentBorderSize(int size)
{
	_component_border_size = size;
}

void Theme::SetWindowFont(Font *font)
{
	_window_font = font;
}

void Theme::SetComponentFont(Font *font)
{
	_component_font = font;
}

Color & Theme::GetWindowBackgroundColor()
{
	return _window_bgcolor;
}

Color & Theme::GetWindowForegroundColor()
{
	return _window_fgcolor;
}

Color & Theme::GetWindowBorderColor()
{
	return _window_border_color;
}

Color & Theme::GetComponentBackgroundColor()
{
	return _component_bgcolor;
}

Color & Theme::GetComponentForegroundColor()
{
	return _component_fgcolor;
}

Color & Theme::GetComponentBorderColor()
{
	return _component_border_color;
}

Color & Theme::GetComponentBorderFocusColor()
{
	return _component_border_focus_color;
}

Color & Theme::GetComponentBackgroundFocusColor()
{
	return _component_focus_bgcolor;
}

Color & Theme::GetComponentForegroundFocusColor()
{
	return _component_focus_fgcolor;
}

Color & Theme::GetComponentScrollbarColor()
{
	return _component_scrollbar_color;
}

Color & Theme::GetComponentDisabledBackgroundColor()
{
	return _component_disabled_bgcolor;
}

Color & Theme::GetComponentDisabledForegroundColor()
{
	return _component_disabled_fgcolor;
}

Color & Theme::GetComponentDisabledBorderColor()
{
	return _component_disabled_border_color;
}

Color & Theme::GetItemColor()
{
	return _item_color;
}

Color & Theme::GetItemForegroundColor()
{
	return _item_fgcolor;
}

Color & Theme::GetItemFocusColor()
{
	return _item_focus_color;
}

Color & Theme::GetItemForegroundFocusColor()
{
	return _item_focus_fgcolor;
}

Color & Theme::GetSelectedItemColor()
{
	return _item_selected_color;
}

Color & Theme::GetSelectedItemForegroundColor()
{
	return _item_selected_fgcolor;
}

Color & Theme::GetDisabledItemColor()
{
	return _item_disabled_color;
}

Color & Theme::GetDisabledItemForegroundColor()
{
	return _item_disabled_fgcolor;
}

jcomponent_border_t Theme::GetComponentBorder()
{
	return _component_border;
}

jcomponent_border_t Theme::GetWindowBorder()
{
	return _window_border;
}

jinsets_t Theme::GetWindowInsets()
{
	return _insets;
}

int Theme::GetWindowBorderSize()
{
	return _window_border_size;
}

int Theme::GetComponentBorderSize()
{
	return _component_border_size;
}

Font * Theme::GetWindowFont()
{
	return _window_font;
}

Font * Theme::GetComponentFont()
{
	return _component_font;
}

}
