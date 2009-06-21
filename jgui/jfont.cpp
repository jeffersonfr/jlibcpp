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
#include "jfont.h"
#include "jgfxhandler.h"
#include "jgraphics.h"

namespace jgui {

Font::Font(std::string name, int attr, int height, int scale_width, int scale_height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Font");

	_font = NULL;
	_name = name;
	_height = 0;
	_ascender = 0;
	_descender = 0;
	_virtual_height = height;
	_scale_width = scale_width;
	_scale_height = scale_height;

	if (_scale_width <= 0) {
		_scale_width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale_height <= 0) {
		_scale_height = DEFAULT_SCALE_HEIGHT;
	}

#ifdef DIRECTFB_UI
	((GFXHandler *)GFXHandler::GetInstance())->CreateFont(name, height, &_font, _scale_width, _scale_height);

	if (_font != NULL) {
		_font->GetHeight(_font, &_height);
		_font->GetAscender(_font, &_ascender);
		_font->GetDescender(_font, &_descender);
	}
#endif
}

Font::~Font()
{
#ifdef DIRECTFB_UI
	_font->Release(_font);
#endif
}

void Font::SetWorkingScreenSize(int width, int height)
{
	_scale_width = width;
	_scale_height = height;
}

void * Font::GetFont()
{
#ifdef DIRECTFB_UI
	return _font;
#endif

	return NULL;
}

bool Font::SetEncoding(std::string code)
{
#ifdef DIRECTFB_UI
	DFBTextEncodingID enc_id;
	
	if (_font == NULL) {
		return false;
	}

	if (_font->FindEncoding(_font, code.c_str(), &enc_id) != DFB_OK) {
		return false;
	}

	if (_font->SetEncoding(_font, enc_id) != DFB_OK) {
		return false;
	}

	return true;
#else
	return false;
#endif
}

std::string Font::GetName()
{
	return _name;
}

int Font::GetVirtualHeight()
{
	return _virtual_height;
}

int Font::GetHeight()
{
	return (int)ceil(((double)_height*(double)_scale_height)/(double)GFXHandler::GetInstance()->GetScreenHeight());
}

int Font::GetAscender()
{
	return (int)ceil(((double)_ascender*(double)_scale_width)/(double)GFXHandler::GetInstance()->GetScreenWidth());
}

int Font::GetDescender()
{
	return (int)ceil(((double)abs(_descender)*(double)_scale_width)/(double)GFXHandler::GetInstance()->GetScreenWidth());
}

int Font::GetMaxAdvanced()
{
	return (int)ceil(((double)_max_advance*(double)_scale_width)/(double)GFXHandler::GetInstance()->GetScreenWidth());
}

int Font::GetLeading()
{
	return (int)ceil(((double)(_height/2)*(double)_scale_width)/(double)GFXHandler::GetInstance()->GetScreenWidth());
}

int Font::GetStringWidth(std::string text)
{
#ifdef DIRECTFB_UI
	if (_font == NULL) {
		return 0;
	}

	int size;

	_font->GetStringWidth(_font, text.c_str(), -1, &size);

	return (int)round(((double)size*(double)_scale_width)/(double)GFXHandler::GetInstance()->GetScreenWidth());
#else
	return 0;
#endif
}

std::string Font::TruncateString(std::string text, std::string extension, int width)
{
	if (text.size() <= 1) {
		return text;
	}

	if (GetStringWidth(text) < width) {
		return text;
	}

	bool flag = false;

	while (GetStringWidth(text + extension) > width) {
		flag = true;

		text = text.substr(0, text.size()-1);

		if (text.size() <= 1) {
			break;
		}
	}

	if (flag == true) {
		return text + extension;
	}

	return text;
}

void Font::Release()
{
}

void Font::Restore()
{
}

}
