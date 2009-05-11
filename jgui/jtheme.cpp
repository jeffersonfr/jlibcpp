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

namespace jgui {

Theme::Theme():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Theme");

	_component_font = NULL;
	_component_bgcolor = 0xff153555;
	_component_fgcolor = 0xfff0f0f0;
	_component_focus_bgcolor = 0xff0665aa;
	_component_focus_fgcolor = 0xfff0f0f0;
	_component_border_color = 0xff606080; // 0xff355575;
	_component_border_size = 1;
	_component_border = LINE_BORDER;

	_gradient_level = 0x40;

	_insets.left = 30;
	_insets.top = 60;
	_insets.right = 30;
	_insets.bottom = 30;
	
	_window_font = NULL;
	_window_bgcolor = 0xff355575;
	_window_fgcolor = 0xfff0f0f0;
	_window_border_color = 0xff355575;
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
	
	parent->SetIgnoreRepaint(true);

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
			c->SetBackgroundFocusColor(_component_focus_bgcolor);
			c->SetForegroundFocusColor(_component_focus_fgcolor);
			c->SetGradientLevel(_gradient_level);
			c->SetBorderSize(_component_border_size);
			c->SetBorder(_component_border);

			if (_component_font != NULL) {
				c->SetFont(_component_font);
			}
		}

		stack.erase(stack.begin());
	}

	parent->SetIgnoreRepaint(false);
	parent->Repaint();
}

void Theme::SetWindowBackgroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_window_bgcolor = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetWindowForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_window_fgcolor = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetWindowBorderColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_window_border_color = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetComponentBackgroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_bgcolor = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetComponentForegroundColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_fgcolor = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetComponentBorderColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_border_color = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetComponentBackgroungFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_focus_bgcolor = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
}

void Theme::SetComponentForegroundFocusColor(int red, int green, int blue, int alpha)
{
	TRUNC_COLOR(red, green, blue, alpha);

	_component_focus_fgcolor = (alpha << 24) | (red << 16) | (green << 8) | (blue << 0);
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

void Theme::SetGradientLevel(int level)
{
	_gradient_level = level;
}

}
