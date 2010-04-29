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
#include "jicon.h"
#include "jcommonlib.h"

namespace jgui {

Icon::Icon(std::string file, int x, int y, int width, int height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Icon");

	_halign = CENTER_HALIGN;
	_valign = CENTER_VALIGN;

	_image = NULL;
	_file = file;
	_text = file;

	_image = new jgui::OffScreenImage(width, height);

	if (_image->GetGraphics() == NULL || _image->GetGraphics()->DrawImage(_file, 0, 0, _size.width, _size.height) == false) {
		delete _image;
		_image = NULL;
	}
}

Icon::~Icon()
{
	if (_image != NULL) {
		delete _image;
		_image = NULL;
	}
}

void Icon::SetSize(int w, int h)
{
	if (_size.width == w && _size.height == h) {
		return;
	}

	_size.width = w;
	_size.height = h;

	if (_size.width < _minimum_size.width) {
		_size.width = _minimum_size.width;
	}

	if (_size.height < _minimum_size.height) {
		_size.height = _minimum_size.height;
	}

	if (_size.width > _maximum_size.width) {
		_size.width = _maximum_size.width;
	}

	if (_size.height > _maximum_size.height) {
		_size.height = _maximum_size.height;
	}

	if (_image != NULL) {
		delete _image;
	}

	_image = new OffScreenImage(_size.width, _size.height);

	if (_image->GetGraphics() == NULL || _image->GetGraphics()->DrawImage(_file, 0, 0, _size.width, _size.height) == false) {
		delete _image;
		_image = NULL;
	}

	Repaint(true);
}

void Icon::SetText(std::string text)
{
	_text = text;

	if (_image == NULL) {
		Repaint();
	}
}

std::string Icon::GetText()
{
	return _text;
}

void Icon::SetImage(std::string file)
{
	jthread::AutoLock lock(&_component_mutex);

	_file = file;

	if (_text == "") {
		_text = file;
	}

	if (_image != NULL) {
		delete _image;
		_image = NULL;
	}

	_image = new OffScreenImage(_size.width, _size.height);

	if (_image->GetGraphics() == NULL || _image->GetGraphics()->DrawImage(_file, 0, 0, _size.width, _size.height) == false) {
		delete _image;
		_image = NULL;
	}

	Repaint();
}

void Icon::SetHorizontalAlign(jhorizontal_align_t align)
{
	if (_halign != align) {
		_halign = align;

		Repaint();
	}
}

jhorizontal_align_t Icon::GetHorizontalAlign()
{
	return _halign;
}

void Icon::SetVerticalAlign(jvertical_align_t align)
{
	if (_valign != align) {
		_valign = align;

		Repaint();
	}
}

jvertical_align_t Icon::GetVerticalAlign()
{
	return _valign;
}
		
void Icon::Paint(Graphics *g)
{
	jthread::AutoLock lock(&_component_mutex);

	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (_image != NULL) {
		int x = _horizontal_gap+_border_size,
				y = _vertical_gap+_border_size,
				w = _size.width-2*x,
				h = _size.height-2*y,
				gapx = 0,
				gapy = 0;
		int px = x+gapx,
				py = y+gapy,
				pw = w-2*gapx,
				ph = h-2*gapy;

		g->DrawImage(_image, px, py, pw, ph);
	} else {
		if (_font != NULL) {
			if (_has_focus == true) {
				g->SetColor(_focus_fgcolor);
			} else {
				g->SetColor(_fgcolor);
			}

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

			g->SetClip(0, 0, x+w, y+h);
			g->DrawString(text, px, py, pw, ph, _halign, _valign);
			g->SetClip(0, 0, _size.width, _size.height);
		}
	}

	PaintEdges(g);
}

}
