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
#include "jtooglebutton.h"
#include "jbuttonlistener.h"
#include "jcommonlib.h"

namespace jgui {

ToogleButton::ToogleButton(std::string label, int x, int y, int width, int height):
	ImageButton(label, "", x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ToogleButton");

	_halign = CENTER_HALIGN;

	_is_pressed = false;
	
	SetFocusable(true);
}

ToogleButton::ToogleButton(std::string image, std::string label, int x, int y, int width, int height):
	ImageButton(label, image, x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ToogleButton");

	_is_pressed = false;
}

ToogleButton::~ToogleButton()
{
}

void ToogleButton::SetButtonPressed(bool b)
{
	if (_is_pressed == b) {
		return;
	}

	_is_pressed = b;

	Repaint();
}

bool ToogleButton::IsPressed()
{
	return _is_pressed;
}

bool ToogleButton::ProcessEvent(MouseEvent *event)
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

		if (_is_pressed == true) {
			_is_pressed = false;
		} else {
			_is_pressed = true;
		}

		RequestFocus();
		Repaint();
		DispatchButtonEvent(new ButtonEvent(this, GetText()));
	}

	return catched;
}

bool ToogleButton::ProcessEvent(KeyEvent *event)
{
	if (Component::ProcessEvent(event) == true) {
		return true;
	}

	if (_enabled == false) {
		return false;
	}

	bool catched = false;

	jkey_symbol_t action = event->GetSymbol();

	if (action == JKEY_ENTER) {
		if (_is_pressed == true) {
			_is_pressed = false;
		} else {
			_is_pressed = true;
		}

		Repaint();

		DispatchButtonEvent(new ButtonEvent(this, GetText()));

		catched = true;
	}

	return catched;
}

void ToogleButton::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	ImageButton::Paint(g);

	if(_is_pressed == true) {
		jcolor_t color = _bgcolor;

		color.alpha = 0xa0;

		g->SetDrawingFlags(DF_BLEND);
		g->SetColor(color);
		g->FillRectangle(0, 0, _size.width, _size.height);
	}
}

}
