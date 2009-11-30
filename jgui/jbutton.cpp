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
#include "jbutton.h"
#include "jbuttonlistener.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

#include <algorithm>

namespace jgui {

Button::Button(std::string label, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Button");

	_index = 0;
	_align = CENTER_ALIGN;

	_name_list.push_back(label);

	SetFocusable(true);
}

Button::~Button()
{
}

void Button::SetText(std::string name)
{
	_name_list.clear();
	_name_list.push_back(name);
}

void Button::AddName(std::string name)
{
	_name_list.push_back(name);
}

void Button::RemoveName(int index)
{
	if (index >= (int)_name_list.size()) {
		return;
	}

	_name_list.erase(_name_list.begin()+index);
}

void Button::SetCurrentNameIndex(int index)
{
	int size = _name_list.size();

	if (size == 0) {
		_index = 0;

		return;
	}

	_index = index;

	if (_index < 0) {
		_index = size-1;
	}

	if (_index > (int)(size-1)) {
		_index = 0;
	}

	Repaint();
}

void Button::NextName()
{
	_index++;

	SetCurrentNameIndex(_index);
}

void Button::PreviousName()
{
	_index--;

	SetCurrentNameIndex(_index);
}

void Button::SetAlign(jalign_t align)
{
	if (_align != align) {
		jthread::AutoLock lock(&_component_mutex);

		_align = align;

		Repaint();
	}
}

jalign_t Button::GetAlign()
{
	return _align;
}

bool Button::ProcessEvent(MouseEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	if (event->GetType() == JMOUSE_PRESSED_EVENT) {
		catched = true;

		RequestFocus();
		DispatchEvent(new ButtonEvent(this, GetName()));
	}

	return catched;
}

bool Button::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	if (event->GetSymbol() == JKEY_ENTER) {
		DispatchEvent(new ButtonEvent(this, GetName()));

		catched = true;
	}

	return catched;
}

std::string Button::GetName()
{
	if (_name_list.size() == 0) {
		return "";
	}

	if (_index < 0) {
		_index = 0;
	}

	if (_index >= (int)_name_list.size()) {
		_index = _name_list.size()-1;
	}

	return _name_list[_index];
}

void Button::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	/*
 if (_has_focus == true) {
	 g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
 	g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
 }
 */

	if (_font != NULL) {
		g->SetFont(_font);

		if (_has_focus == true) {
			g->SetColor(_fgfocus_color);
		} else {
			g->SetColor(_fg_color);
		}

		int gap = _horizontal_gap+_border_size;

		if (gap < 0) {
			gap = 0;
		}

		g->DrawString(TruncateString(GetName(), _size.width-2*gap), gap/2, (CENTER_VERTICAL_TEXT), _size.width-gap, _size.height, _align);
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _size.width, _size.height);
	}
}

void Button::RegisterButtonListener(ButtonListener *listener)
{
	if (listener == NULL) {
		return;
	}

	if (std::find(_button_listeners.begin(), _button_listeners.end(), listener) == _button_listeners.end()) {
		_button_listeners.push_back(listener);
	}
}

void Button::RemoveButtonListener(ButtonListener *listener)
{
	if (listener == NULL) {
		return;
	}

	std::vector<ButtonListener *>::iterator i = std::find(_button_listeners.begin(), _button_listeners.end(), listener);
	
	if (i != _button_listeners.end()) {
		_button_listeners.erase(i);
	}
}

void Button::DispatchEvent(ButtonEvent *event)
{
	if (event == NULL) {
		return;
	}

	int k=0;

	while (k++ < (int)_button_listeners.size()) {
		_button_listeners[k-1]->ActionPerformed(event);
	}

	/*
	for (std::vector<ButtonListener *>::iterator i=_button_listeners.begin(); i!=_button_listeners.end(); i++) {
		(*i)->ActionPerformed(event);
	}
	*/

	delete event;
}

std::vector<ButtonListener *> & Button::GetButtonListeners()
{
	return _button_listeners;
}

}
