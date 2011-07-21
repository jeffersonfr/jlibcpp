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
#include "jfont.h"
#include "jgfxhandler.h"

namespace jgui {

Font *Font::_default_font = NULL;

Font::Font(std::string name, jfont_attributes_t attributes, int height, int scale_width, int scale_height):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Font");

#ifdef DIRECTFB_UI
	_font = NULL;
#endif

	_attributes = attributes;
	_name = name;
	_height = 0;
	_ascender = 0;
	_descender = 0;
	_virtual_height = height;
	
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = scale_width;
	_scale.height = scale_height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}

#ifdef DIRECTFB_UI
	((GFXHandler *)GFXHandler::GetInstance())->CreateFont(name, height, &_font, _scale.width, _scale.height);

	if (_font != NULL) {
		_font->GetHeight(_font, &_height);
		_font->GetAscender(_font, &_ascender);
		_font->GetDescender(_font, &_descender);
	}
#endif

	GFXHandler::GetInstance()->Add(this);
}

Font::~Font()
{
	GFXHandler::GetInstance()->Add(this);

#ifdef DIRECTFB_UI
	if (_font != NULL) {
		_font->Release(_font);
	}
#endif
}

Font * Font::GetDefaultFont()
{
	if (_default_font == NULL) {
		_default_font = new jgui::Font(_DATA_PREFIX"/fonts/font.ttf", JFA_NONE, DEFAULT_FONT_SIZE);
	}

	return _default_font;
}

void Font::SetWorkingScreenSize(int width, int height)
{
	_scale.width = width;
	_scale.height = height;
}

jsize_t Font::GetWorkingScreenSize()
{
	return _scale;
}

jfont_attributes_t Font::GetFontAttributes()
{
	return _attributes;
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
#endif

	return false;
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
	return SCREEN_TO_SCALE(_height, _screen.width, _scale.width);
}

int Font::GetAscender()
{
	return SCREEN_TO_SCALE(_ascender, _screen.width, _scale.width);
}

int Font::GetDescender()
{
	return SCREEN_TO_SCALE(abs(_descender), _screen.width, _scale.width);
}

int Font::GetMaxAdvanced()
{
	return SCREEN_TO_SCALE(_max_advance, _screen.width, _scale.width);
}

int Font::GetLeading()
{
	return SCREEN_TO_SCALE(_height/2.0, _screen.width, _scale.width);
}

int Font::GetStringWidth(std::string text)
{
	int size = 0;

#ifdef DIRECTFB_UI
	if (_font == NULL) {
		return 0;
	}

	_font->GetStringWidth(_font, text.c_str(), -1, &size);
#endif
	
	return SCREEN_TO_SCALE(size, _screen.width, _scale.width);
}

jregion_t Font::GetStringExtends(std::string text)
{
	jregion_t region;

	region.x = 0;
	region.y = 0;
	region.width = 0;
	region.height = 0;

#ifdef DIRECTFB_UI
	if (_font == NULL) {
		return region;
	}

	DFBRectangle lrect;
							 // irect;

	_font->GetStringExtents(_font, text.c_str(), -1, &lrect, NULL); // &irect);

	region.x = SCREEN_TO_SCALE(lrect.x, _screen.width, _scale.width);
	region.y = SCREEN_TO_SCALE(lrect.y, _screen.width, _scale.width);
	region.width = SCREEN_TO_SCALE(lrect.w, _screen.width, _scale.width);
	region.height = SCREEN_TO_SCALE(lrect.h, _screen.width, _scale.width);
#endif
	
	return region;
}

jregion_t Font::GetGlyphExtends(int symbol)
{
	jregion_t region;

	region.x = 0;
	region.y = 0;
	region.width = 0;
	region.height = 0;

#ifdef DIRECTFB_UI
	if (_font == NULL) {
		return region;
	}

	DFBRectangle lrect;
	int advance;

	_font->GetGlyphExtents(_font, symbol, &lrect, &advance);

	region.x = SCREEN_TO_SCALE(lrect.x, _screen.width, _scale.width);
	region.y = SCREEN_TO_SCALE(lrect.y, _screen.width, _scale.width);
	region.width = SCREEN_TO_SCALE(lrect.w, _screen.width, _scale.width);
	region.height = SCREEN_TO_SCALE(lrect.h, _screen.width, _scale.width);
#endif
	
	return region;
}

std::string Font::TruncateString(std::string text, std::string extension, int width)
{
	if (text.size() <= 1 || width <= 0) {
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
#ifdef DIRECTFB_UI
	if (_font != NULL) {
		_font->Dispose(_font);
		_font->Release(_font);
		_font = NULL;
	}
#endif
}

void Font::Restore()
{
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

#ifdef DIRECTFB_UI
	if (_font == NULL) {
		((GFXHandler *)GFXHandler::GetInstance())->CreateFont(_name, _virtual_height, &_font, _scale.width, _scale.height);
	}
#endif
}

}
