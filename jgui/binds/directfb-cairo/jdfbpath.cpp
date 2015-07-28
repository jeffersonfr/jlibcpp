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
#include "jdfbpath.h"
#include "jdfbfont.h"
#include "jcharset.h"

#define M_2PI	(2*M_PI)

namespace jgui {

DFBPath::DFBPath(DFBGraphics *g):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::DFBPath");

	_graphics = g;
}

DFBPath::~DFBPath()
{
}

void DFBPath::SetPattern(Image *image)
{
	if (image == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<DFBGraphics *>(image->GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	// struct jpoint_t t = _graphics->Translate();

	// int x0 = xp+t.x;
	// int y0 = yp+t.y;
	// int x1 = wp+t.x;
	// int y1 = hp+t.y;

	cairo_pattern_t *pattern = cairo_pattern_create_for_surface(cairo_surface);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	cairo_set_source(_graphics->_cairo_context, pattern);
	// cairo_rectangle(_graphics->_cairo_context, x0, y0, x1-x0, y1-y0);
	cairo_fill(_graphics->_cairo_context);
	cairo_pattern_destroy(pattern);
}

void DFBPath::SetPattern(int x0p, int y0p, int x1p, int y1p)
{
	cairo_t *cairo_context = _graphics->_cairo_context;

	struct jpoint_t t = _graphics->Translate();

	int x0 = x0p+t.x;
	int y0 = y0p+t.y;
	int x1 = x1p+t.x;
	int y1 = y1p+t.y;
		
	// int x = x0;
	// int y = y0;
	// int w = x1-x0;
	// int h = y1-y0;

	// if (w < 0) {
	//	x = x1;
	//	w = -w;
	// }

	// if (h < 0) {
	//	y = y1;
	//	h = -h;
	// }

	cairo_pattern_t *pattern = cairo_pattern_create_linear(x0, y0, x1, y1);

	for (std::vector<jgradient_t>::iterator i=_graphics->_gradient_stops.begin(); i!=_graphics->_gradient_stops.end(); i++) {
		jgradient_t gradient = (*i);

		int sr = gradient.color.GetRed(),
				sg = gradient.color.GetGreen(),
				sb = gradient.color.GetBlue(),
				sa = gradient.color.GetAlpha();

		cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	}

	// cairo_rectangle(cairo_context, x, y, w, h);
	cairo_set_source(cairo_context, pattern);
	cairo_fill(cairo_context);
	cairo_pattern_destroy(pattern);
}

void DFBPath::SetPattern(int x0p, int y0p, int rad0, int x1p, int y1p, int rad1)
{
	cairo_t *cairo_context = _graphics->_cairo_context;

	struct jpoint_t t = _graphics->Translate();

	int x0 = x0p+t.x;
	int y0 = y0p+t.y;
	int x1 = x1p+t.x;
	int y1 = y1p+t.y;
		
	cairo_pattern_t *pattern = cairo_pattern_create_radial(x0, y0, rad0, x1, y1, rad1);

	for (std::vector<jgradient_t>::iterator i=_graphics->_gradient_stops.begin(); i!=_graphics->_gradient_stops.end(); i++) {
		jgradient_t gradient = (*i);

		int sr = gradient.color.GetRed(),
				sg = gradient.color.GetGreen(),
				sb = gradient.color.GetBlue(),
				sa = gradient.color.GetAlpha();

		cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	}

	// cairo_arc(cairo_context, x0, y0, rad0, 0.0, 2 * M_PI);
	cairo_set_source(cairo_context, pattern);
	cairo_fill(cairo_context);
	cairo_pattern_destroy(pattern);
}

void DFBPath::MoveTo(int xp, int yp)
{
	struct jpoint_t t = _graphics->Translate();

	int x = xp+t.x;
	int y = yp+t.y;

	cairo_move_to(_graphics->_cairo_context, x, y);
}

void DFBPath::LineTo(int xp, int yp)
{
	struct jpoint_t t = _graphics->Translate();

	int x = xp+t.x;
	int y = yp+t.y;

	cairo_line_to(_graphics->_cairo_context, x, y);
}

void DFBPath::CurveTo(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	struct jpoint_t t = _graphics->Translate();

	int x1 = x1p+t.x;
	int y1 = y1p+t.y;
	int x2 = x2p+t.x;
	int y2 = y2p+t.y;
	int x3 = x3p+t.x;
	int y3 = y3p+t.y;

	cairo_curve_to(_graphics->_cairo_context, (double)x1, (double)y1, (double)x2, (double)y2, (double)x3, (double)y3);
}

void DFBPath::ArcTo(int xcp, int ycp, int radius, double arc0, double arc1, bool negative)
{
	struct jpoint_t t = _graphics->Translate();

	int xc = xcp+t.x;
	int yc = ycp+t.y;

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	if (negative == false) {
		cairo_arc(_graphics->_cairo_context, xc, yc, radius, arc0, arc1);
	} else {
		cairo_arc_negative(_graphics->_cairo_context, xc, yc, radius, arc0, arc1);
	}
}

void DFBPath::TextTo(std::string text, int xp, int yp)
{
	DFBFont *font = dynamic_cast<DFBFont *>(_graphics->GetFont());

	if (font == NULL) {
		return;
	}

	struct jpoint_t t = _graphics->Translate();

	int x = t.x+xp;
	int y = t.y+yp;

	const char *utf8 = text.c_str();
	int utf8_len = text.size();
	cairo_glyph_t *glyphs = NULL;
	int glyphs_len = 0;
	cairo_status_t status;

	if (font->GetEncoding() == JFE_ISO_8859_1) {
		jcommon::Charset charset;

		utf8 = charset.Latin1ToUTF8(utf8, &utf8_len);
	}

	status = cairo_scaled_font_text_to_glyphs(
			font->_scaled_font, x, y+font->GetAscender(), utf8, utf8_len, &glyphs, &glyphs_len, NULL, NULL, NULL);

	if (status == CAIRO_STATUS_SUCCESS) {
		cairo_glyph_path(_graphics->_cairo_context, glyphs, glyphs_len);
		cairo_glyph_free(glyphs);
	}

	if (font->GetEncoding() == JFE_ISO_8859_1) {
		delete [] utf8;
	}
}

void DFBPath::Close()
{
	cairo_close_path(_graphics->_cairo_context);
}

void DFBPath::Stroke()
{
	int width = _graphics->_pen.width;

	if (width < 0) {
		width = -width;
	}


	cairo_set_line_width(_graphics->_cairo_context, width);
	cairo_stroke(_graphics->_cairo_context);
}

void DFBPath::Fill()
{
	cairo_fill(_graphics->_cairo_context);
}

void DFBPath::SetSource(Image *image)
{
	DFBGraphics *g = dynamic_cast<DFBGraphics *>(image->GetGraphics());

	if (g == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(g->_cairo_context);

	cairo_set_source_surface(_graphics->_cairo_context, cairo_surface, 0, 0);
}

void DFBPath::SetMask(Image *image)
{
	DFBGraphics *g = dynamic_cast<DFBGraphics *>(image->GetGraphics());

	if (g == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(g->_cairo_context);

	cairo_mask_surface(_graphics->_cairo_context, cairo_surface, 0, 0);
}

void DFBPath::SetMatrix(double *matrix)
{
}

void DFBPath::GetMatrix(double **matrix)
{
}

}
