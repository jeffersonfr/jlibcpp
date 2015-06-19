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
#include "jlabel.h"
#include "jstringtokenizer.h"
#include "jstringutils.h"
#include "jdebug.h"

namespace jgui {

Label::Label(std::string text, int x, int y, int width, int height):
   	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Label");

	_is_wrap = false;
	_halign = JHA_CENTER;
	_valign = JVA_CENTER;

	_vertical_gap = 5;
	_horizontal_gap = 5;
	_text = text;
}

Label::~Label()
{
}

void Label::SetWrap(bool b)
{
	if (_is_wrap == b) {
		return;
	}

	_is_wrap = b;

	Repaint();
}

bool Label::IsWrap()
{
	return _is_wrap;
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

void Label::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t Label::GetHorizontalAlign()
{
	return _halign;
}

void Label::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t Label::GetVerticalAlign()
{
	return _valign;
}

jsize_t Label::GetPreferredSize()
{
	jsize_t t = _size;

	if (IsFontSet() == true) {
		int wp = _size.width-2*(_horizontal_gap-_border_size),
				hp = _font->GetSize();

		if (wp > 0) {
			std::vector<std::string> lines;

			_font->GetStringBreak(&lines, _text, wp, INT_MAX, _halign);

			t.height = lines.size()*hp+2*(_vertical_gap+_border_size);
		}
	}

	return t;
}

void Label::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (IsFontSet() == true) {
		if (_is_enabled == true) {
			if (_has_focus == true) {
				g->SetColor(_focus_fgcolor);
			} else {
				g->SetColor(_fgcolor);
			}
		} else {
			g->SetColor(_disabled_fgcolor);
		}

		int x = _horizontal_gap+_border_size,
				y = _vertical_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				gapx = 0,
				gapy = 0;
		int px = x+gapx,
				py = y+gapy,//(h-_font->GetSize())/2+gapy,
				pw = w-gapx,
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

		if (_is_wrap == false) {
			text = _font->TruncateString(text, "...", w);
		}

		g->DrawString(text, px, py, pw, ph, _halign, _valign);
	}
}

}
