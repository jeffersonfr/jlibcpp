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
#include "jdfbfont.h"
#include "jdfbhandler.h"

namespace jgui {

DFBFont::DFBFont(std::string name, jfont_attributes_t attributes, int size, int scale_width, int scale_height):
	jgui::Font(name, attributes, size, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::DFBFont");

	_font = NULL;

	_size = 0;
	_ascender = 0;
	_descender = 0;
	_virtual_size = size;
	
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->CreateFont(name, size, &_font, _scale.width, _scale.height);

	if (_font != NULL) {
		_font->GetHeight(_font, &_size);
		_font->GetAscender(_font, &_ascender);
		_font->GetDescender(_font, &_descender);
	}

	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Add(this);
}

DFBFont::~DFBFont()
{
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_font != NULL) {
		_font->Release(_font);
	}
}

jfont_attributes_t DFBFont::GetFontAttributes()
{
	return _attributes;
}

void * DFBFont::GetNativeFont()
{
	return _font;
}

bool DFBFont::SetEncoding(std::string code)
{
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
}

std::string DFBFont::GetName()
{
	return _name;
}

int DFBFont::GetVirtualSize()
{
	return _virtual_size;
}

int DFBFont::GetSize()
{
	return SCREEN_TO_SCALE(_size, _screen.width, _scale.width);
}

int DFBFont::GetAscender()
{
	return SCREEN_TO_SCALE(_ascender, _screen.width, _scale.width);
}

int DFBFont::GetDescender()
{
	return SCREEN_TO_SCALE(abs(_descender), _screen.width, _scale.width);
}

int DFBFont::GetMaxAdvance()
{
	return SCREEN_TO_SCALE(_max_advance, _screen.width, _scale.width);
}

int DFBFont::GetLeading()
{
	return SCREEN_TO_SCALE(_size/2.0, _screen.width, _scale.width);
}

int DFBFont::GetStringWidth(std::string text)
{
	int size = 0;

	if (_font == NULL) {
		return 0;
	}

	_font->GetStringWidth(_font, text.c_str(), -1, &size);
	
	return SCREEN_TO_SCALE(size, _screen.width, _scale.width);
}

jregion_t DFBFont::GetStringExtends(std::string text)
{
	jregion_t region;

	region.x = 0;
	region.y = 0;
	region.width = 0;
	region.height = 0;

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
	
	return region;
}

jregion_t DFBFont::GetGlyphExtends(int symbol)
{
	jregion_t region;

	region.x = 0;
	region.y = 0;
	region.width = 0;
	region.height = 0;

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
	
	return region;
}

void DFBFont::Release()
{
	if (_font != NULL) {
		_font->Dispose(_font);
		_font->Release(_font);
		_font = NULL;
	}
}

void DFBFont::Restore()
{
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	if (_font == NULL) {
		dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->CreateFont(_name, _virtual_size, &_font, _scale.width, _scale.height);
	}
}

}
