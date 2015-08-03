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
#include "jsdlfont.h"
#include "jsdlhandler.h"
#include "jnullpointerexception.h"

namespace jgui {

SDLFont::SDLFont(std::string name, jfont_attributes_t attributes, int size):
	jgui::Font(name, attributes, size)
{
	jcommon::Object::SetClassName("jgui::SDLFont");

	// CHANGE:: force initialization ... sync with SDLHandler::InitEngine()
	TTF_Init();

	_encoding = JFE_UTF8;

	jio::File file(name);

	if (file.Exists() == false) {
		if (attributes & JFA_BOLD) {
			name = name + "-bold";
		}
		
		if (attributes & JFA_ITALIC) {
			name = name + "-italic";
		}

		name = std::string(_DATA_PREFIX"/fonts/") + name + ".ttf";

		jio::File file(name);

		if (file.Exists() == false) {
			throw jcommon::NullPointerException("Cannot create a native font");
		}
	}

	_font = TTF_OpenFont(name.c_str(), size);

	if (_font == NULL) { 
		throw jcommon::NullPointerException("Cannot create a native font");
	}

	int style = TTF_STYLE_NORMAL;

	if ((attributes & JFA_BOLD) != 0) {
		style = style | TTF_STYLE_BOLD;
	}
	
	if ((attributes & JFA_ITALIC) != 0) {
		style = style | TTF_STYLE_ITALIC;
	}
	
	TTF_SetFontStyle(_font, style);
	TTF_SetFontOutline(_font, 0);
	TTF_SetFontHinting(_font, TTF_HINTING_NORMAL);
	TTF_SetFontKerning(_font, 1); 

	_ascender = abs(TTF_FontAscent(_font));
	_descender = abs(TTF_FontDescent(_font));
	_leading = TTF_FontLineSkip(_font) - _ascender - _descender;
	_max_advance_width = 0;
	_max_advance_height = 0;
	
	for (int i=0; i<256; i++) {
		jregion_t bounds;

		bounds = SDLFont::GetGlyphExtends(i);

		_widths[i] = bounds.x+bounds.width;
	}

	dynamic_cast<SDLHandler *>(GFXHandler::GetInstance())->Add(this);
}

SDLFont::~SDLFont()
{
	dynamic_cast<SDLHandler *>(GFXHandler::GetInstance())->Remove(this);

	TTF_CloseFont(_font);

	_font = NULL;
}

void SDLFont::ApplyContext(void *ctx)
{
}

void * SDLFont::GetNativeFont()
{
	return _font;
}

std::string SDLFont::GetName()
{
	return _name;
}

int SDLFont::GetAscender()
{
	return _ascender;
}

int SDLFont::GetDescender()
{
	return abs(_descender);
}

int SDLFont::GetMaxAdvanceWidth()
{
	return _max_advance_width;
}

int SDLFont::GetMaxAdvanceHeight()
{
	return _max_advance_height;
}

int SDLFont::GetLeading()
{
	return _leading;
}

int SDLFont::GetStringWidth(std::string text)
{
	return GetStringExtends(text).width;
}

jregion_t SDLFont::GetStringExtends(std::string text)
{
	jregion_t r;
	int w, h;

	r.x = 0;
	r.y = 0;
	r.width = 0;
	r.height = 0;

	if (_encoding == JFE_UTF8) {
		if (TTF_SizeUTF8(_font, text.c_str(), &w, &h) != 0) {
			return r;
		}
	} else if (_encoding == JFE_ISO_8859_1) {
		if (TTF_SizeText(_font, text.c_str(), &w, &h) != 0) {
			return r;
		}
	}

	r.width = w;
	r.height = h;

	return r;
}

jregion_t SDLFont::GetGlyphExtends(int symbol)
{
	jregion_t r;
	int minx;
	int maxx;
	int miny;
	int maxy;
	int advance;

	r.x = 0;
	r.y = 0;
	r.width = 0;
	r.height = 0;

	if (TTF_GlyphMetrics(_font, symbol, &minx, &maxx, &miny, &maxy, &advance) != 0) {
		return r;
	}

	r.x = advance;
	r.y = 0;
	r.width = maxx-minx;
	r.height = maxy-miny;

	return r;
}

bool SDLFont::CanDisplay(int ch)
{
	return true;
}

int SDLFont::GetCharWidth(char ch)
{
	return _widths[(int)ch];
}

const int * SDLFont::GetCharWidths()
{
	return (int *)_widths;
}

void SDLFont::Release()
{
}

void SDLFont::Restore()
{
}

}
