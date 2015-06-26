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

	jio::File file(name);

	_font = NULL;
	_is_builtin = false;

	if (file.Exists() == false) {
		_is_builtin = true;
	} else {
		dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->CreateFont(name, size, &_font, "Unicode");
	}

	if (_is_builtin == false && _font == NULL) {
		throw jcommon::NullPointerException("Cannot create a native font");
	}

	_charset = "Unicode";

	surface_ref = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
	context_ref = cairo_create(surface_ref);

	ApplyContext(context_ref);

	// TODO:: problem initializing invalid chars
	for (int i=0; i<256; i++) {
		std::string str;
		jregion_t bounds;

		bounds = DFBFont::GetGlyphExtends(i);

		_widths[i] = bounds.x+bounds.width;
	}

	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Add(this);
}

DFBFont::~DFBFont()
{
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_font != NULL) {
		cairo_surface_destroy(surface_ref);
		cairo_destroy(context_ref);

		cairo_font_face_destroy(_font);
		// FT_Done_Face (ft_face);
	}
}

void DFBFont::ApplyContext(void *ctx)
{
	cairo_t *context = (cairo_t *)ctx;

	cairo_font_extents_t t;

	if (_is_builtin == false) {
		int attr = 0;

		if ((_attributes & JFA_BOLD) != 0) {
			attr = attr | CAIRO_FT_SYNTHESIZE_BOLD;
		}

		if ((_attributes & JFA_ITALIC) != 0) {
			attr = attr | CAIRO_FT_SYNTHESIZE_OBLIQUE;
		}

		cairo_ft_font_face_set_synthesize(_font, attr);
		cairo_set_font_face(context, (cairo_font_face_t *)_font);
	} else {
		cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
		cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;

		if ((_attributes & JFA_BOLD) != 0) {
			weight = CAIRO_FONT_WEIGHT_BOLD;
		}

		if ((_attributes & JFA_ITALIC) != 0) {
			slant = CAIRO_FONT_SLANT_ITALIC;
		}

		cairo_select_font_face(context, _name.c_str(), slant, weight);
	}

	// INFO:: setting font options
	cairo_font_options_t *options = cairo_font_options_create();

	// INFO:: DEFAULT, NONE, GRAY, SUBPIXEL, FAST, GOOD, BEST
	cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_SUBPIXEL);
	cairo_set_font_options(context, options);
	cairo_font_options_destroy(options);

	cairo_set_font_size(context, _size);
	cairo_font_extents (context, &t);

	_ascender = t.ascent;
	_descender = t.descent;
	_leading = t.height - t.ascent - t.descent;
	_max_advance_width = t.max_x_advance;
	_max_advance_height = t.max_y_advance;
}

jfont_attributes_t DFBFont::GetFontAttributes()
{
	return _attributes;
}

void * DFBFont::GetNativeFont()
{
	return _font;
}

bool DFBFont::SetEncoding(std::string charset)
{
	if (_is_builtin == true) {
		return false;
	}

	if (strcasecmp(charset.c_str(), "utf") == 0 || strcasecmp(charset.c_str(), "uni") == 0) {
		charset = "Unicode";
	} else if (strcasecmp(charset.c_str(), "latin1") == 0 || strcasecmp(charset.c_str(), "latin-1") == 0) {
		charset = "Latin-1";
	} else if (strcasecmp(charset.c_str(), "ms_symbol") == 0 || strcasecmp(charset.c_str(), "ms-symbol") == 0) {
		charset = "MS Symbol";
	} else if (strcasecmp(charset.c_str(), "sjis") == 0) {
		charset = "SJIS";
	} else if (strcasecmp(charset.c_str(), "gb2312") == 0 || strcasecmp(charset.c_str(), "gb-2312") == 0) {
		charset = "GB-2312";
	} else if (strcasecmp(charset.c_str(), "big5") == 0 || strcasecmp(charset.c_str(), "big-5") == 0) {
		charset = "Big-5";
	} else if (strcasecmp(charset.c_str(), "wansung") == 0) {
		charset = "Wansung";
	} else if (strcasecmp(charset.c_str(), "johab") == 0) {
		charset = "Johab";
	} else {
		return false;
	}

	_charset = charset;

	if (_font != NULL) {
		cairo_font_face_destroy(_font);

		_font = NULL;
	}
		
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->CreateFont(_name, _size, &_font, _charset);

	return true;
}

std::string DFBFont::GetEncoding()
{
	return _charset;
}

std::string DFBFont::GetName()
{
	return _name;
}

int DFBFont::GetSize()
{
	return Font::GetSize();
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
	jregion_t t = GetStringExtends(text);

	return t.x+t.width;
}

jregion_t DFBFont::GetStringExtends(std::string text)
{
	cairo_text_extents_t ts, tc;

	ApplyContext(context_ref);

	cairo_text_extents(context_ref, (text+'A').c_str(), &ts);
	cairo_text_extents(context_ref, "A", &tc);
	
	jregion_t r;

	r.x = ts.x_bearing;
	r.y = ts.y_bearing;
	r.width = ts.width-tc.width;
	r.height = ts.height;

	return r;
}

jregion_t DFBFont::GetGlyphExtends(int symbol)
{
	cairo_text_extents_t t;
	cairo_glyph_t glyph;

	glyph.x = 0;
	glyph.y = 0;
	glyph.index = symbol;

	cairo_set_font_face(context_ref, (cairo_font_face_t *)_font);
	cairo_set_font_size(context_ref, _size);

	cairo_glyph_extents(context_ref, &glyph, 1, &t);
	
	jregion_t r;

	r.x = t.x_bearing;
	r.y = t.y_bearing;
	r.width = t.width;
	r.height = t.height;

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
}

void DFBFont::Restore()
{
}

}
