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
	ImageButton("", label, x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ToogleButton");

	_is_pressed = false;
	
	SetFocusable(true);
}

ToogleButton::ToogleButton(std::string image, std::string label, int x, int y, int width, int height):
	ImageButton(image, label, x, y, width, height)
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
		DispatchEvent(new ButtonEvent(this, GetName()));
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

		DispatchEvent(new ButtonEvent(this, GetName()));

		catched = true;
	}

	return catched;
}

void ToogleButton::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	int font_height = DEFAULT_COMPONENT_HEIGHT;
	
	if (_font != NULL) {
		font_height = _font->GetHeight();
	}

	g->SetFont(_font);

	{
		/*
		if (_has_focus == true) {
			if (_is_pressed == true) {
				g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
				g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
			} else {
				g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
			}
		} else {
			if (_is_pressed == true) {
				g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
				g->FillRectangle(0, 0, _width, _height);
			}
		}
		*/

		int align_dx = 10;

		if (_align == 1) {
			align_dx = 0;
		}

		g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

		/*
		if (_is_pressed == true) {
			g->SetColor(_bg_red, _bg_green, _bg_blue, _bg_alpha);
		} else {
			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);
		}
		*/
		
		if (_image != "") {
			if (GetName() == "") {
				if (_has_focus == true) {
					if (prefetch2 != NULL) {
						g->DrawImage(prefetch2, 0, 0, _width, _height);
					} else {
						g->DrawImage(prefetch1, 0, 0, _width, _height);
					}
				} else {
					g->DrawImage(prefetch1, 0, 0, _width, _height);
				}
			} else {
				int ts = _width-(15+font_height+2*(align_dx+_border_size));

				if (ts < 0) {
					ts = 0;
				}

				g->DrawString(TruncateString(GetName(), ts), align_dx, (CENTER_VERTICAL_TEXT), _width-align_dx, font_height, LEFT_ALIGN);
				g->DrawImage(prefetch1, 5, 0, _height, _height);
			}
		} else {
			int ts = _width-2*(align_dx+_border_size);

			if (ts < 0) {
				ts = 0;
			}

			g->DrawString(TruncateString(GetName(), ts), align_dx, (CENTER_VERTICAL_TEXT), _width-align_dx, _height, _align);
		}
	}

	if(_is_pressed == true) {
		g->SetDrawingFlags(DF_BLEND);
		g->SetColor(_bg_red, _bg_green, _bg_blue, 0x40);
		g->FillRectangle(0, 0, _width, _height);
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetDrawingFlags(DF_BLEND);
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		g->FillRectangle(0, 0, _width, _height);
	}

}

}
