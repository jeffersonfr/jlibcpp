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
#include "jnullpointerexception.h"

namespace jgui {

DFBFont::DFBFont(std::string name, jfont_attributes_t attributes, int size):
	jgui::Font(name, attributes, size)
{
	jcommon::Object::SetClassName("jgui::DFBFont");

	_font = NULL;

	DFBHandler *handler = dynamic_cast<DFBHandler *>(GFXHandler::GetInstance());

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

	handler->CreateFont(name, size, &_font);

	if (_font == NULL) {
		throw jcommon::NullPointerException("Cannot create a native font");
	}

	_font->GetAscender(_font, &_ascender);
	_font->GetDescender(_font, &_descender);
	_leading = size - _ascender - _descender;
	_max_advance_width = 0;
	_max_advance_height = 0;
	
	for (int i=0; i<256; i++) {
		jregion_t bounds;

		bounds = DFBFont::GetGlyphExtends(i);

		_widths[i] = bounds.x+bounds.width;
	}

	handler->Add(this);
}

DFBFont::~DFBFont()
{
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_font != NULL) {
		_font->Release(_font);
	}
}

void DFBFont::ApplyContext(void *ctx)
{
	IDirectFBSurface *context = (IDirectFBSurface *)ctx;

	context->SetFont(context, _font);
}

void * DFBFont::GetNativeFont()
{
	return _font;
}

std::string DFBFont::GetName()
{
	return _name;
}

int DFBFont::GetAscender()
{
	return _ascender;
}

int DFBFont::GetDescender()
{
	return abs(_descender);
}

int DFBFont::GetMaxAdvanceWidth()
{
	return _max_advance_width;
}

int DFBFont::GetMaxAdvanceHeight()
{
	return _max_advance_height;
}

int DFBFont::GetLeading()
{
	return _leading;
}

int DFBFont::GetStringWidth(std::string text)
{
	if (_font == NULL) {
		return 0;
	}

	int size = 0;

	_font->GetStringWidth(_font, text.c_str(), text.size(), &size);
	
	return size;
}

jregion_t DFBFont::GetStringExtends(std::string text)
{
	jregion_t r;

	r.x = 0;
	r.y = 0;
	r.width = 0;
	r.height = 0;

	if (_font == NULL) {
		return r;
	}

	DFBRectangle rect;

	_font->GetStringExtents(_font, text.c_str(), text.size(), &rect, NULL);

	r.x = rect.x;
	r.y = rect.y;
	r.width = rect.w;
	r.height = rect.h;
	
	return r;
}

jregion_t DFBFont::GetGlyphExtends(int symbol)
{
	jregion_t r;

	r.x = 0;
	r.y = 0;
	r.width = 0;
	r.height = 0;

	if (_font == NULL) {
		return r;
	}

	DFBRectangle rect;
	int advance;

	_font->GetGlyphExtents(_font, symbol, &rect, &advance);

	r.x = rect.x;
	r.y = rect.y;
	r.width = rect.w;
	r.height = rect.h;
	
	return r;
}

bool DFBFont::CanDisplay(int ch)
{
	return true;
}

int DFBFont::GetCharWidth(char ch)
{
	return _widths[(int)ch];
}

const int * DFBFont::GetCharWidths()
{
	return (int *)_widths;
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
	if (_font == NULL) {
		dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->CreateFont(_name, GetSize(), &_font);
	}
}

}
