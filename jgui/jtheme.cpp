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
#include "jtheme.h"
#include "jcomponent.h"
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
	
	SetItemColor(0x17, 0x27, 0x3e, 0xff);
	SetItemFocusColor(0x57, 0x67, 0x7e, 0xff);
	SetSelectedItemColor(0x80, 0x80, 0x80, 0xff);
	SetSelectedItemForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetItemForegroundColor(0xf0, 0xf0, 0xf0, 0xff);
	SetItemForegroundFocusColor(0xf0, 0xf0, 0xf0, 0xff);

	_component_border_size = 1;
	_component_border = LINE_BORDER;

	_component_font = NULL;

	_insets.left = 30;
	_insets.top = 60;
	_insets.right = 30;
	_insets.bottom = 30;
	
	_window_font = NULL;
	_window_border_size = 4;
	_window_border = GRADIENT_BORDER;
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

	// WARN:: esse metodo pode causar problemas pela falta de sincronizacao com a Window (em caso de remocao ou adicao de componentes)
	
	if (parent->InstanceOf("jgui::Window") == true) {
		Window *w = (Window *)parent;

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

	std::vector<Component *> stack;

	stack.push_back(parent);

	while (stack.size() > 0) {
		Component *c = *stack.begin();

		if (c->InstanceOf("jgui::Container") == true) {
			for (std::vector<Component *>::iterator i=((Container *)c)->GetComponents().begin(); i!=((Container *)c)->GetComponents().end(); i++) {
				Component *component = (*i);

				if (((Container *)c)->InstanceOf("jgui::Container") == true) {
					stack.push_back(component);
				}
			}
		} else {
			c->SetBackgroundColor(_component_bgcolor);
			c->SetForegroundColor(_component_fgcolor);
			c->SetBorderColor(_component_border_color);
			c->SetBorderFocusColor(_component_border_focus_color);
			c->SetBackgroundFocusColor(_component_focus_bgcolor);
			c->SetForegroundFocusColor(_component_focus_fgcolor);
			c->SetBorderSize(_component_border_size);
			c->SetBorder(_component_border);

			if (_component_font != NULL) {
				c->SetFont(_component_font);
			}
		
			if (c->InstanceOf("jgui::ItemComponent") == true) {
				ItemComponent *ic = (ItemComponent *)c;

				ic->SetItemColor(_item_color);
				ic->SetItemFocusColor(_item_focus_color);
				ic->SetSelectedItemColor(_item_selected_color);
				ic->SetSelectedItemForegroundColor(_item_selected_fgcolor);
				ic->SetItemForegroundColor(_item_fgcolor);
				ic->SetItemForegroundFocusColor(_item_focus_fgcolor);
			}
		}

		stack.erase(stack.begin());
	}
}

void Theme::SetWindowBackgroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_window_bgcolor.red = red;
	_window_bgcolor.green = green;
	_window_bgcolor.blue = blue;
	_window_bgcolor.alpha = alpha;
}

void Theme::SetWindowForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_window_fgcolor.red = red;
	_window_fgcolor.green = green;
	_window_fgcolor.blue = blue;
	_window_fgcolor.alpha = alpha;
}

void Theme::SetWindowBorderColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_window_border_color.red = red;
	_window_border_color.green = green;
	_window_border_color.blue = blue;
	_window_border_color.alpha = alpha;
}

void Theme::SetComponentBackgroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_bgcolor.red = red;
	_component_bgcolor.green = green;
	_component_bgcolor.blue = blue;
	_component_bgcolor.alpha = alpha;
}

void Theme::SetComponentForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_fgcolor.red = red;
	_component_fgcolor.green = green;
	_component_fgcolor.blue = blue;
	_component_fgcolor.alpha = alpha;
}

void Theme::SetComponentBorderColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_border_color.red = red;
	_component_border_color.green = green;
	_component_border_color.blue = blue;
	_component_border_color.alpha = alpha;
}

void Theme::SetComponentBorderFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_border_focus_color.red = red;
	_component_border_focus_color.green = green;
	_component_border_focus_color.blue = blue;
	_component_border_focus_color.alpha = alpha;
}

void Theme::SetComponentBackgroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_focus_bgcolor.red = red;
	_component_focus_bgcolor.green = green;
	_component_focus_bgcolor.blue = blue;
	_component_focus_bgcolor.alpha = alpha;
}

void Theme::SetComponentForegroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_focus_fgcolor.red = red;
	_component_focus_fgcolor.green = green;
	_component_focus_fgcolor.blue = blue;
	_component_focus_fgcolor.alpha = alpha;
}

void Theme::SetItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_color.red = red;
	_item_color.green = green;
	_item_color.blue = blue;
	_item_color.alpha = alpha;
}

void Theme::SetItemFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_focus_color.red = red;
	_item_focus_color.green = green;
	_item_focus_color.blue = blue;
	_item_focus_color.alpha = alpha;
}

void Theme::SetSelectedItemColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_selected_color.red = red;
	_item_selected_color.green = green;
	_item_selected_color.blue = blue;
	_item_selected_color.alpha = alpha;
}

void Theme::SetSelectedItemForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_selected_fgcolor.red = red;
	_item_selected_fgcolor.green = green;
	_item_selected_fgcolor.blue = blue;
	_item_selected_fgcolor.alpha = alpha;
}

void Theme::SetItemForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_fgcolor.red = red;
	_item_fgcolor.green = green;
	_item_fgcolor.blue = blue;
	_item_fgcolor.alpha = alpha;
}

void Theme::SetItemForegroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_item_focus_fgcolor.red = red;
	_item_focus_fgcolor.green = green;
	_item_focus_fgcolor.blue = blue;
	_item_focus_fgcolor.alpha = alpha;
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

jcolor_t Theme::GetWindowBackgroundColor()
{
	return _window_bgcolor;
}

jcolor_t Theme::GetWindowForegroundColor()
{
	return _window_fgcolor;
}

jcolor_t Theme::GetWindowBorderColor()
{
	return _window_border_color;
}

jcolor_t Theme::GetComponentBackgroundColor()
{
	return _component_bgcolor;
}

jcolor_t Theme::GetComponentForegroundColor()
{
	return _component_fgcolor;
}

jcolor_t Theme::GetComponentBorderColor()
{
	return _component_border_color;
}

jcolor_t Theme::GetComponentBorderFocusColor()
{
	return _component_border_focus_color;
}

jcolor_t Theme::GetComponentBackgroundFocusColor()
{
	return _component_focus_bgcolor;
}

jcolor_t Theme::GetComponentForegroundFocusColor()
{
	return _component_focus_fgcolor;
}

jcolor_t Theme::GetItemColor()
{
	return _item_color;
}

jcolor_t Theme::GetItemFocusColor()
{
	return _item_focus_color;
}

jcolor_t Theme::GetSelectedItemColor()
{
	return _item_selected_color;
}

jcolor_t Theme::GetSelectedItemForegroundColor()
{
	return _item_selected_fgcolor;
}

jcolor_t Theme::GetItemForegroundColor()
{
	return _item_fgcolor;
}

jcolor_t Theme::GetItemForegroundFocusColor()
{
	return _item_focus_fgcolor;
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
