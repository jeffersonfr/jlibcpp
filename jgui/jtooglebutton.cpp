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
#include "jtooglebutton.h"
#include "jbuttonlistener.h"
#include "jdebug.h"

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

	Component::Paint(g);

	/*
	if (_has_focus == true) {
		g->FillGradientRectangle(0, 0, _width, _height/2+1, 
			_bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
		g->FillGradientRectangle(0, _height/2, _width, _height/2, 
			_bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
	}
	*/

	int x = _horizontal_gap+_border_size,
			y = _vertical_gap+_border_size,
			w = _size.width-2*x,
			h = _size.height-2*y,
			gapx = 0,
			gapy = 0;
	int px = x+gapx,
			py = y+gapy,
			pw = w-gapx,
			ph = h-gapy;

	if (GetText() == "") {
		g->DrawImage((_has_focus == true && _image_focus_icon != NULL)?_image_focus_icon:_image_icon, px, py, pw, ph);
	} else {
		g->DrawImage((_has_focus == true && _image_focus_icon != NULL)?_image_focus_icon:_image_icon, px, py, ph, ph);

		if (_font != NULL) {
			if (_has_focus == true) {
				g->SetColor(_focus_fgcolor);
			} else {
				g->SetColor(_fgcolor);
			}

			if (_image != "") {
				gapx = x+ph;
			}

			px = x+gapx;
			py = y+gapy;
			pw = w-gapx;
			ph = h-gapy;

			x = (x < 0)?0:x;
			y = (y < 0)?0:y;
			w = (w < 0)?0:w;
			h = (h < 0)?0:h;

			px = (px < 0)?0:px;
			py = (py < 0)?0:py;
			pw = (pw < 0)?0:pw;
			ph = (ph < 0)?0:ph;

			std::string text = GetText();

			if (_wrap == false) {
				text = _font->TruncateString(text, "...", pw);
			}

			g->DrawString(text, px, py, pw, ph, _halign, _valign);
		}
	}

	if (_is_pressed == true && _enabled == true) {
		jcolor_t color = _bgcolor;

		color.alpha = 0xa0;

		g->SetDrawingFlags(DF_BLEND);
		g->SetColor(color);
		g->FillRectangle(0, 0, _size.width, _size.height);
	}
	
	PaintEdges(g);
}

}
