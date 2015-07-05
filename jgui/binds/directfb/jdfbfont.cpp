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

	DFBHandler *handler = dynamic_cast<DFBHandler *>(GFXHandler::GetInstance());

	if (file.Exists() == false) {
		_is_builtin = true;
	} else {
		handler->CreateFont(name, &_font);
	
		if (_font == NULL) {
			throw jcommon::NullPointerException("Cannot load a native font");
		}
	}

	_surface_ref = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 0, 0);
	_context_ref = cairo_create(_surface_ref);

	// INFO:: DEFAULT, NONE, GRAY, SUBPIXEL, FAST, GOOD, BEST
	_options = cairo_font_options_create();

	cairo_font_options_set_antialias(_options, CAIRO_ANTIALIAS_SUBPIXEL);

	// INFO:: initializing font parameters
	if (_is_builtin == false) {
		int attr = 0;

		if ((_attributes & JFA_BOLD) != 0) {
			attr = attr | CAIRO_FT_SYNTHESIZE_BOLD;
		}

		if ((_attributes & JFA_ITALIC) != 0) {
			attr = attr | CAIRO_FT_SYNTHESIZE_OBLIQUE;
		}

		cairo_ft_font_face_set_synthesize(_font, attr);
		cairo_set_font_face(_context_ref, (cairo_font_face_t *)_font);
	} else {
		cairo_font_slant_t slant = CAIRO_FONT_SLANT_NORMAL;
		cairo_font_weight_t weight = CAIRO_FONT_WEIGHT_NORMAL;

		if ((_attributes & JFA_BOLD) != 0) {
			weight = CAIRO_FONT_WEIGHT_BOLD;
		}

		if ((_attributes & JFA_ITALIC) != 0) {
			slant = CAIRO_FONT_SLANT_ITALIC;
		}

		cairo_select_font_face(_context_ref, _name.c_str(), slant, weight);
	}

	cairo_font_extents_t t;

	cairo_set_font_options(_context_ref, _options);
	cairo_set_font_size(_context_ref, _size);
	cairo_font_extents(_context_ref, &t);

	_ascender = t.ascent;
	_descender = t.descent;
	_leading = t.height - t.ascent - t.descent;
	_max_advance_width = t.max_x_advance;
	_max_advance_height = t.max_y_advance;
	
	// INFO:: creating a scaled font
	cairo_font_face_t *font_face = cairo_get_font_face(_context_ref);
	cairo_matrix_t fm;
	cairo_matrix_t tm;

	cairo_get_matrix(_context_ref, &tm);
	cairo_get_font_matrix(_context_ref, &fm);

	_scaled_font = cairo_scaled_font_create(font_face, &fm, &tm, _options);

	cairo_surface_destroy(_surface_ref);
	cairo_destroy(_context_ref);

	// INFO:: intializing the first 256 characters withs
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
		cairo_scaled_font_destroy(_scaled_font);
		cairo_font_face_destroy(_font);
		cairo_font_options_destroy(_options);
		// FT_Done_Face (ft_face);
	}
}

void DFBFont::ApplyContext(void *ctx)
{
	cairo_t *context = (cairo_t *)ctx;

	cairo_set_scaled_font(context, _scaled_font);
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
	jregion_t t = GetStringExtends(text);

	return t.x+t.width;
}

jregion_t DFBFont::GetStringExtends(std::string text)
{
	const char *utf8 = text.c_str();
	cairo_text_extents_t t;

	// TODO:: convert to latin1

	cairo_scaled_font_text_extents(_scaled_font, utf8, &t);

	jregion_t r;

	r.x = t.x_bearing;
	r.y = t.y_bearing;
	r.width = t.width;
	r.height = t.height;

	return r;
}

jregion_t DFBFont::GetGlyphExtends(int symbol)
{
	cairo_glyph_t glyph;
	cairo_text_extents_t t;

	glyph.index = symbol;
	glyph.x = 0;
	glyph.y = 0;

	cairo_scaled_font_glyph_extents(_scaled_font, &glyph, 1, &t);

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
