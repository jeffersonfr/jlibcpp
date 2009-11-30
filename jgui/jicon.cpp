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

Icon::Icon(std::string file, int x, int y, int width, int height, int scale_width, int scale_height):
	Component(x, y, width, height)
{
	jcommon::Object::SetClassName("jgui::Icon");

	int w = width,
		h = height;

	_image = NULL;
	_file = file;
	_text = "";
	_scale_width = scale_width;
	_scale_height = scale_height;

	if (_scale_width <= 0) {
		_scale_width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale_height <= 0) {
		_scale_height = DEFAULT_SCALE_HEIGHT;
	}

	if (w <= 0 || h <= 0) {
#ifdef DIRECTFB_UI
		IDirectFBImageProvider *provider = NULL;
		DFBSurfaceDescription desc;

		IDirectFB *engine = (IDirectFB *)GFXHandler::GetInstance()->GetGraphicEngine();

		if (engine->CreateImageProvider(engine, file.c_str(), &provider) != DFB_OK) {
			return;
		}

		if (provider->GetSurfaceDescription(provider, &desc) != DFB_OK) {
			return;
		}

		if (w <= 0) {
			w = SCREEN_TO_SCALE(desc.width, GFXHandler::GetInstance()->GetScreenWidth(), scale_width);
		}

		if (h <= 0) {
			h = SCREEN_TO_SCALE(desc.height, GFXHandler::GetInstance()->GetScreenHeight(), scale_height);
		}
#endif
	}

	_image = new jgui::OffScreenImage(w, h);

	if (_image->GetGraphics() != NULL) {
		if (_image->GetGraphics()->DrawImage(_file, 0, 0, _size.width, _size.height) == true) {
			Repaint();
		} else {
			delete _image;
			_image = NULL;
		}
	}

	SetSize(w, h);
}

Icon::~Icon()
{
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

	if (_image->GetGraphics() != NULL) {
		if (_image->GetGraphics()->DrawImage(_file, 0, 0, _size.width, _size.height) == true) {
			Repaint();
		} else {
			delete _image;
			_image = NULL;
		}
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

void Icon::SetImage(std::string file)
{
	jthread::AutoLock lock(&_component_mutex);

	_file = file;

	if (_image != NULL) {
		delete _image;
	}

	_image = new OffScreenImage(_size.width, _size.height);

	if (_image->GetGraphics() != NULL) {
		if (_image->GetGraphics()->DrawImage(_file, 0, 0, _size.width, _size.height) == true) {
			Repaint();
		} else {
			delete _image;
			_image = NULL;
		}
	}
}

void Icon::Paint(Graphics *g)
{
	jthread::AutoLock lock(&_component_mutex);

	JDEBUG(JINFO, "paint\n");

	Component::Paint(g);

	if (_image != NULL) {
		g->DrawImage(_image, 0, 0, _size.width, _size.height);
	} else {
		g->SetColor(_fg_color);

		int gap = _horizontal_gap+_border_size;

		if (gap < 0) {
			gap = 0;
		}

		g->DrawString(TruncateString(_text, _size.width-2*gap), gap/2, (CENTER_VERTICAL_TEXT), _size.width-gap, _size.height, CENTER_ALIGN);
		// CHANGED:: g->DrawString(TruncateString(_text, _size.width-10), 5, 5, _size.width-10, _height, CENTER_ALIGN);
	}

	PaintBorder(g);
	
	if (_enabled == false) {
		g->SetColor(0x00, 0x00, 0x00, 0x80);
		FillRectangle(g, 0, 0, _size.width, _size.height);
	}
}

}
