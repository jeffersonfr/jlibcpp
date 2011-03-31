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
#include "jimagebutton.h"
#include "jdebug.h"

namespace jgui {

ImageButton::ImageButton(std::string label, std::string image, int x, int y, int width, int height):
	Button(label, x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ImageButton");

	_image = image;
	_image_focus = "";

	_image_icon = NULL;
	_image_focus_icon = NULL;

	_image_icon = Image::CreateImage(_image);
}

ImageButton::~ImageButton()
{
	if ((void *)_image_icon != NULL) {
		delete _image_icon;
	}

	if ((void *)_image_focus_icon != NULL) {
		delete _image_focus_icon;
	}
}

void ImageButton::SetImage(std::string image)
{
	_image = image;

	if (_image_icon != NULL) {
		delete _image_icon;
	}

	_image_icon = Image::CreateImage(_image);
}

void ImageButton::SetImageFocus(std::string image)
{
	_image_focus = image;

	if (_image_focus_icon != NULL) {
		delete _image_focus_icon;
	}

	_image_focus_icon = Image::CreateImage(_image_focus);
}

void ImageButton::Paint(Graphics *g)
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

	PaintBorderEdges(g);
}

}
