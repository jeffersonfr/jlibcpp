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
#include "jimagebutton.h"
#include "jbuttonlistener.h"
#include "jfocusevent.h"
#include "jcommonlib.h"

namespace jgui {

ImageButton::ImageButton(std::string image, std::string label, int x, int y, int width, int height):
		Button(label, x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::ImageButton");

	_image = image;
	_image_focus = "";
	prefetch1 = NULL;
	prefetch2 = NULL;

	if (_image != "") {
		prefetch1 = new OffScreenImage(_width, _height);
		
		if (prefetch1->GetGraphics() != NULL) {
			prefetch1->GetGraphics()->DrawImage(_image, 0, 0, _width, _height);
		}
	}
}

ImageButton::~ImageButton()
{
	if ((void *)prefetch1 != NULL) {
		delete prefetch1;
	}

	if ((void *)prefetch2 != NULL) {
		delete prefetch2;
	}
}

void ImageButton::SetSize(int w, int h)
{
	if (_width == w && _height == h) {
		return;
	}

	_width = w;
	_height = h;

	if (_width < _minimum_width) {
		_width = _minimum_width;
	}

	if (_height < _minimum_height) {
		_height = _minimum_height;
	}

	if (_width > _maximum_width) {
		_width = _maximum_width;
	}

	if (_height > _maximum_height) {
		_height = _maximum_height;
	}

	if (_image != "") {
		if (prefetch1 != NULL) {
			delete prefetch1;
		}

		prefetch1 = new OffScreenImage(_width, _height);
		
		if (prefetch1->GetGraphics() != NULL) {
			prefetch1->GetGraphics()->Clear();
			prefetch1->GetGraphics()->DrawImage(_image, 0, 0, _width, _height);
		}
	}
	
	if (_image_focus != "") {
		if (prefetch2 != NULL) {
			if (prefetch2 != NULL) {
				delete prefetch2;
			}

			prefetch2 = new OffScreenImage(_width, _height);

			if (prefetch2->GetGraphics() != NULL) {
				prefetch2->GetGraphics()->DrawImage(_image_focus, 0, 0, _width, _height);
			}
		}
	}

	Repaint(true);
}

void ImageButton::SetImage(std::string image)
{
	_image = image;

	if (_image != "") {
		if (prefetch1 != NULL) {
			delete prefetch1;
		}

		prefetch1 = new OffScreenImage(_width, _height);
		
		if (prefetch1->GetGraphics() != NULL) {
			prefetch1->GetGraphics()->Clear();
			prefetch1->GetGraphics()->DrawImage(_image, 0, 0, _width, _height);
		}
	}
}

void ImageButton::SetImageFocus(std::string image)
{
	_image_focus = image;

	if (prefetch2 != NULL) {
		if (prefetch2 != NULL) {
			delete prefetch2;
		}

		prefetch2 = new OffScreenImage(_width, _height);
		
		if (prefetch2->GetGraphics() != NULL) {
			prefetch2->GetGraphics()->DrawImage(_image_focus, 0, 0, _width, _height);
		}
	}
}

void ImageButton::Paint(Graphics *g)
{
	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	{
		/*
		if (_has_focus == true) {
				g->FillGradientRectangle(0, 0, _width, _height/2+1, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha);
				g->FillGradientRectangle(0, _height/2, _width, _height/2, _bgfocus_red, _bgfocus_green, _bgfocus_blue, _bgfocus_alpha, _bgfocus_red-_gradient_level, _bgfocus_green-_gradient_level, _bgfocus_blue-_gradient_level, _bgfocus_alpha);
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
				g->DrawImage(prefetch1, 0, 0, _height, _height);
				g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

				int gap = _horizontal_gap+_border_size;

				if (IsFontSet() == true) {
					gap = gap + _font->GetHeight();
				}

				if (gap < 0) {
					gap = 0;
				}

				g->DrawString(TruncateString(GetName(), _width-2*gap), _height+gap/2, (CENTER_VERTICAL_TEXT), _width-gap, _height, LEFT_ALIGN);
			}
		} else {
			g->SetColor(_fg_red, _fg_green, _fg_blue, _fg_alpha);

			int gap = _horizontal_gap+_border_size;

			if (gap < 0) {
				gap = 0;
			}

			g->DrawString(TruncateString(GetName(), _width-2*gap), gap/2, (CENTER_VERTICAL_TEXT), _width-gap, _height, _align);
		}
	}

	PaintBorder(g);

	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _width, _height);
	}
}

}
