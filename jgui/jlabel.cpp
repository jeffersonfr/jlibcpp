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
#include "jlabel.h"
#include "jcommonlib.h"

namespace jgui {

Label::Label(std::string text, int x, int y, int width, int height):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Label");

	_align = LEFT_ALIGN;
	_vertical_gap = 5;
	_horizontal_gap = 5;
	_text = text;
	_wrap = false;

	SetAlign(LEFT_ALIGN);
}

Label::~Label()
{
}

void Label::SetWrap(bool b)
{
	if (_wrap == b) {
		return;
	}

	_wrap = b;

	Repaint();
}

std::string Label::GetText()
{
	return _text;
}

void Label::SetText(std::string text)
{
	_text = text;

	Repaint();
}

void Label::SetAlign(jalign_t align)
{
	if (_align != align) {
		jthread::AutoLock lock(&_component_mutex);

		_align = align;

		Repaint();
	}
}

jalign_t Label::GetAlign()
{
	return _align;
}

void Label::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (IsFontSet() == true) {
		g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

		if (_wrap == false && _truncate_string == true) {
			g->DrawString(TruncateString(_text, _width-2*_horizontal_gap), _horizontal_gap, (CENTER_VERTICAL_TEXT), _width-2*_horizontal_gap, _height, _align);
		} else {
			// g->DrawString(TruncateString(_text, _width-2*_horizontal_gap), _horizontal_gap, _vertical_gap, _width-2*_horizontal_gap, _height, _align);
			g->DrawString(_text, _horizontal_gap, _vertical_gap, _width-2*_horizontal_gap, _height, _align);
		}
	}

	PaintBorder(g);
	
	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _width, _height);
	}
}

}
