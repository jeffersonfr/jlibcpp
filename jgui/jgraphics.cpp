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
#include "jgui/jgraphics.h"
#include "jgui/jfont.h"
#include "jgui/jimage.h"
#include "jgui/jrectangle.h"
#include "jmath/jmath.h"
#include "jcommon/jstringtokenizer.h"
#include "jcommon/jstringutils.h"
#include "jcommon/jcharset.h"
#include "jexception/jioexception.h"
#include "jexception/joutofboundsexception.h"
#include "jexception/jnullpointerexception.h"

#define M_2PI	(2*M_PI)
	
#define IMAGE_SIZE_LIMIT 32768
#include <math.h>

namespace jgui {

Graphics::Graphics(cairo_t *cairo_context, jpixelformat_t pixelformat, int wp, int hp):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Graphics");

	_pixelformat = pixelformat;
	_translate.x = 0;
	_translate.y = 0;
	_is_vertical_sync_enabled = false;
	_font = NULL;

	_pen.dashes = NULL;
	_pen.dashes_size = 0;
	_pen.width = 1;
	_pen.join = JLJ_MITER;
	_pen.style = JLS_BUTT;

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = wp;
	_clip.height = hp;

	if (cairo_context != NULL) {
		_cairo_context = cairo_context;
	} else {
		cairo_surface_t *cairo_surface = NULL;
		cairo_format_t format = CAIRO_FORMAT_INVALID;

		if (_pixelformat == JPF_ARGB) {
			format = CAIRO_FORMAT_ARGB32;
		} else if (_pixelformat == JPF_RGB32) {
			format = CAIRO_FORMAT_ARGB32;
		} else if (_pixelformat == JPF_RGB24) {
			format = CAIRO_FORMAT_RGB24;
		} else if (_pixelformat == JPF_RGB16) {
			format = CAIRO_FORMAT_RGB16_565;
		}

		cairo_surface = cairo_image_surface_create(format, wp, hp);

		_cairo_context = cairo_create(cairo_surface);

		cairo_surface_destroy(cairo_surface);
	}

	SetAntialias(JAM_NORMAL);
	SetPen(_pen);
	SetColor(0x00000000);
	ResetGradientStop();
	SetCompositeFlags(JCF_SRC_OVER);
}

Graphics::~Graphics()
{
	cairo_destroy(_cairo_context);
}

cairo_t * Graphics::GetCairoContext()
{
	return _cairo_context;
}

std::string Graphics::Dump(std::string dir, std::string prefix)
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		throw jexception::NullPointerException("The surface is null");
	}

	/*
	mkdir(dir.c_str(), 0755);
	
	if (dir.empty() == false) {
		prefix = dir + "/" + prefix;
	}
	*/

	jio::File *temp = jio::File::CreateTemporaryFile(dir, prefix, ".png");

	if (temp == NULL) {
		throw jexception::IOException("Cannot create the temporary image file");
	}

	std::string name = temp->GetName();

	cairo_surface_flush(cairo_surface);
	cairo_surface_write_to_png(cairo_surface, temp->GetAbsolutePath().c_str());
	
	delete temp;

	return name;
}

jregion_t Graphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, _internal_clip.x, _internal_clip.y, _internal_clip.width, _internal_clip.height);
	
	SetClip(clip.x-_translate.x, clip.y-_translate.y, clip.width, clip.height);

	return clip;
}

void Graphics::SetClip(int xp, int yp, int wp, int hp)
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	int sw = cairo_image_surface_get_width(cairo_surface);
	int sh = cairo_image_surface_get_height(cairo_surface);

	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, 0, 0, sw, sh);
	
	_clip.x = clip.x - _translate.x;
	_clip.y = clip.y - _translate.y;
	_clip.width = clip.width;
	_clip.height = clip.height;
	
	_internal_clip = clip;

	int x1 = clip.x;
	int y1 = clip.y;
	int x2 = clip.x+clip.width+1;
	int y2 = clip.y+clip.height+1;

	cairo_reset_clip(_cairo_context);
	cairo_rectangle(_cairo_context, x1, y1, x2-x1, y2-y1);
	cairo_clip(_cairo_context);
}

jregion_t Graphics::GetClip()
{
	return _clip;
}

void Graphics::ReleaseClip()
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = cairo_image_surface_get_width(cairo_surface);
	_clip.height = cairo_image_surface_get_height(cairo_surface);

	_internal_clip.x = _translate.x;
	_internal_clip.y = _translate.y;
	_internal_clip.width = _clip.width-_translate.x;
	_internal_clip.height = _clip.height-_translate.y;
	
	cairo_reset_clip(_cairo_context);
}

void Graphics::SetCompositeFlags(jcomposite_flags_t t)
{
	cairo_operator_t o = CAIRO_OPERATOR_CLEAR;

	_composite_flags = t;

	if (_composite_flags == JCF_SRC) {
		o = CAIRO_OPERATOR_SOURCE;
	} else if (_composite_flags == JCF_SRC_OVER) {
		o = CAIRO_OPERATOR_OVER;
	} else if (_composite_flags == JCF_SRC_IN) {
		o = CAIRO_OPERATOR_IN;
	} else if (_composite_flags == JCF_SRC_OUT) {
		o = CAIRO_OPERATOR_OUT;
	} else if (_composite_flags == JCF_SRC_ATOP) {
		o = CAIRO_OPERATOR_ATOP;
	} else if (_composite_flags == JCF_DST) {
		o = CAIRO_OPERATOR_DEST;
	} else if (_composite_flags == JCF_DST_OVER) {
		o = CAIRO_OPERATOR_DEST_OVER;
	} else if (_composite_flags == JCF_DST_IN) {
		o = CAIRO_OPERATOR_DEST_IN;
	} else if (_composite_flags == JCF_DST_OUT) {
		o = CAIRO_OPERATOR_DEST_OUT;
	} else if (_composite_flags == JCF_DST_ATOP) {
		o = CAIRO_OPERATOR_DEST_ATOP;
	} else if (_composite_flags == JCF_XOR) {
		o = CAIRO_OPERATOR_XOR;
	} else if (_composite_flags == JCF_ADD) {
		o = CAIRO_OPERATOR_ADD;
	} else if (_composite_flags == JCF_SATURATE) {
		o = CAIRO_OPERATOR_SATURATE;
	} else if (_composite_flags == JCF_MULTIPLY) {
		o = CAIRO_OPERATOR_MULTIPLY;
	} else if (_composite_flags == JCF_SCREEN) {
		o = CAIRO_OPERATOR_SCREEN;
	} else if (_composite_flags == JCF_OVERLAY) {
		o = CAIRO_OPERATOR_OVERLAY;
	} else if (_composite_flags == JCF_DARKEN) {
		o = CAIRO_OPERATOR_DARKEN;
	} else if (_composite_flags == JCF_LIGHTEN) {
		o = CAIRO_OPERATOR_LIGHTEN;
	} else if (_composite_flags == JCF_DIFFERENCE) {
		o = CAIRO_OPERATOR_DIFFERENCE;
	} else if (_composite_flags == JCF_EXCLUSION) {
		o = CAIRO_OPERATOR_EXCLUSION;
	} else if (_composite_flags == JCF_DODGE) {
		o = CAIRO_OPERATOR_COLOR_DODGE;
	} else if (_composite_flags == JCF_BURN) {
		o = CAIRO_OPERATOR_COLOR_BURN;
	} else if (_composite_flags == JCF_HARD) {
		o = CAIRO_OPERATOR_HARD_LIGHT;
	} else if (_composite_flags == JCF_LIGHT) {
		o = CAIRO_OPERATOR_SOFT_LIGHT;
	}

	cairo_set_operator(_cairo_context, o);
}

jcomposite_flags_t Graphics::GetCompositeFlags()
{
	return _composite_flags;
}

void Graphics::Clear()
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	int sw = cairo_image_surface_get_width(cairo_surface);
	int sh = cairo_image_surface_get_height(cairo_surface);

	cairo_save(_cairo_context);
	// cairo_reset_clip(_cairo_context);
	cairo_set_operator(_cairo_context, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle(_cairo_context, 0, 0, sw, sh);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);

	SetCompositeFlags(_composite_flags);
}

void Graphics::Clear(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	if (x < 0) {
		x = 0;
	}

	if (y < 0) {
		y = 0;
	}

	cairo_save(_cairo_context);
	// cairo_reset_clip(_cairo_context);
	cairo_set_operator(_cairo_context, CAIRO_OPERATOR_CLEAR);
	cairo_rectangle(_cairo_context, x, y, w, h);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);

	SetCompositeFlags(_composite_flags);
}

Color & Graphics::GetColor()
{
	return _color;
} 

void Graphics::SetColor(const Color &color)
{
	_color = color;

	int r = _color.GetRed(),
			g = _color.GetGreen(),
			b = _color.GetBlue(),
			a = _color.GetAlpha();

	cairo_set_source_rgba(_cairo_context, r/255.0, g/255.0, b/255.0, a/255.0);
} 

void Graphics::SetColor(uint32_t color)
{
	SetColor(_color = Color(color));
} 

void Graphics::SetColor(int red, int green, int blue, int alpha)
{
	SetColor(_color = Color(red, green, blue, alpha));
} 

bool Graphics::HasFont()
{
	return (_font != NULL);
}

void Graphics::SetFont(Font *font)
{
	_font = font;
	
	if (_font != NULL) {
		_font->ApplyContext(_cairo_context);
	} else {
		cairo_set_font_face(_cairo_context, NULL);
	}
}

Font * Graphics::GetFont()
{
	return _font;
}

void Graphics::SetAntialias(jantialias_mode_t mode)
{
	cairo_antialias_t t = CAIRO_ANTIALIAS_NONE;

	if (mode == JAM_FAST) {
		t = CAIRO_ANTIALIAS_FAST;
	} else if (mode == JAM_NORMAL) {
		t = CAIRO_ANTIALIAS_DEFAULT; // DEFAULT, SUBPIXEL
	} else if (mode == JAM_GOOD) {
		t = CAIRO_ANTIALIAS_GOOD; // GOOD, BEST
	}
		
	cairo_set_antialias(_cairo_context, t);
}

jantialias_mode_t Graphics::GetAntialias()
{
	return _antialias;
}

void Graphics::SetPen(jpen_t t)
{
	_pen = t;

	if (_pen.join == JLJ_BEVEL) {
		cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_BEVEL);
	} else if (_pen.join == JLJ_ROUND) {
		cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_ROUND);
	} else if (_pen.join == JLJ_MITER) {
		cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_MITER);
	}
	
	if (_pen.style == JLS_ROUND) {
		cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_ROUND);
	} else if (_pen.style == JLS_BUTT) {
		cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_BUTT);
	} else if (_pen.style == JLS_SQUARE) {
		cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_SQUARE);
	}
	
	cairo_set_dash(_cairo_context, _pen.dashes, _pen.dashes_size, 0.0);
}

jpen_t Graphics::GetPen()
{
	return _pen;
}

void Graphics::DrawLine(int xp, int yp, int xf, int yf)
{
	int x0 = _translate.x+xp;
	int y0 = _translate.y+yp;
	int x1 = _translate.x+xf;
	int y1 = _translate.y+yf;
	int line_width = _pen.width;

	if (line_width < 0) {
		line_width = -line_width;
	}

	cairo_save(_cairo_context);
	cairo_move_to(_cairo_context, x0, y0);
	cairo_line_to(_cairo_context, x1, y1);
	cairo_set_line_width(_cairo_context, line_width);
	cairo_stroke(_cairo_context);
	cairo_restore(_cairo_context);
}

double EvaluateBezier0(double *data, int ndata, double t) 
{
	if (t < 0.0) {
		return(data[0]);
	}

	if (t >= (double)ndata) {
		return data[ndata-1];
	}

	double result,
				 blend,
				 mu,
				 muk,
				 munk;
	int n,
			k,
			kn,
			nn,
			nkn;

	mu = t/(double)ndata;

	n = ndata-1;
	result = 0.0;
	muk = 1;
	munk = pow(1-mu,(double)n);

	for (k=0; k<=n; k++) {
		nn = n;
		kn = k;
		nkn = n - k;
		blend = muk * munk;
		muk *= mu;
		munk /= (1-mu);

		while (nn >= 1) {
			blend *= nn;
			nn--;

			if (kn > 1) {
				blend /= (double)kn;
				kn--;
			}

			if (nkn > 1) {
				blend /= (double)nkn;
				nkn--;
			}
		}

		result += data[k] * blend;
	}

	return result;
}

void Graphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
	if (_pen.width == 0) {
		return;
	}

	if (npoints < 3) {
		return;
	}

	if (interpolation < 2) {
		return;
	}

	int line_width = _pen.width;

	if (line_width < 0) {
		line_width = -line_width;
	}

	double *x, 
				 *y, 
				 stepsize;
	int // x1, 
			// y1, 
			x2, 
			y2;

	stepsize = (double)1.0/(double)interpolation;

	x = new double[npoints+1];
	y = new double[npoints+1];

	for (int i=0; i<npoints; i++) {
		x[i] = (double)(_translate.x+p[i].x);
		y[i] = (double)(_translate.y+p[i].y);
	}

	x[npoints] = (double)(_translate.x+p[0].x);
	y[npoints] = (double)(_translate.y+p[0].y);

	double t = 0.0;
	
	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

	for (int i=0; i<=(npoints*interpolation); i++) {
		t = t + stepsize;

		x2 = EvaluateBezier0(x, npoints, t);
		y2 = EvaluateBezier0(y, npoints, t);
	
		cairo_line_to(_cairo_context, x2, y2);
	}
    
	delete [] x;
	delete [] y;

  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);
}

void Graphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	cairo_save(_cairo_context);
	cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int line_width = _pen.width;
	
	if (line_width > 0) {
		line_width = line_width/2;

		x = x - line_width;
		y = y - line_width;
		w = w + 2*line_width;
		h = h + 2*line_width;
	} else {
		line_width = -line_width/2;

		x = x + line_width;
		y = y + line_width;
		w = w - 2*line_width;
		h = h - 2*line_width;
	}

	cairo_save(_cairo_context);
	cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, abs(_pen.width));
	cairo_stroke(_cairo_context);
}

void Graphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	if (wp <=0 || hp <= 0) {
		return;
	}

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (corners & JRC_TOP_RIGHT) {
		cairo_line_to(_cairo_context, x + w - dx, y);
		cairo_line_to(_cairo_context, x + w, y + dy);
	} else {
    cairo_line_to(_cairo_context, x + w, y);
	}

  if (corners & JRC_BOTTOM_RIGHT) {
		cairo_line_to(_cairo_context, x + w, y + h - dy);
		cairo_line_to(_cairo_context, x + w - dx, y + h);
	} else {
    cairo_line_to(_cairo_context, x + w, y + h);
	}

  if (corners & JRC_BOTTOM_LEFT) {
		cairo_line_to(_cairo_context, x + dx, y + h);
		cairo_line_to(_cairo_context, x, y + h - dy);
	} else {
		cairo_line_to(_cairo_context, x, y + h);
	}

  if (corners & JRC_TOP_LEFT) {
		cairo_line_to(_cairo_context, x, y + dy);
		cairo_line_to(_cairo_context, x + dx, y);
	} else {
    cairo_line_to(_cairo_context, x, y);
	}

  cairo_close_path(_cairo_context);
	cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int line_width = _pen.width;

	if (wp <=0 || hp <= 0 || line_width == 0) {
		return;
	}

	if (line_width > 0) {
		line_width = line_width/2;

		x = x - line_width;
		y = y - line_width;
		w = w + 2*line_width;
		h = h + 2*line_width;
	} else {
		line_width = -line_width/2;

		x = x + line_width - 1;
		y = y + line_width - 1;
		w = w - 2*line_width + 2;
		h = h - 2*line_width + 2;
	}

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (corners & JRC_TOP_RIGHT) {
		cairo_line_to(_cairo_context, x + w - dx, y);
		cairo_line_to(_cairo_context, x + w, y + dy);
	} else {
    cairo_line_to(_cairo_context, x + w, y);
	}

  if (corners & JRC_BOTTOM_RIGHT) {
		cairo_line_to(_cairo_context, x + w, y + h - dy);
		cairo_line_to(_cairo_context, x + w - dx, y + h);
	} else {
    cairo_line_to(_cairo_context, x + w, y + h);
	}

  if (corners & JRC_BOTTOM_LEFT) {
		cairo_line_to(_cairo_context, x + dx, y + h);
		cairo_line_to(_cairo_context, x, y + h - dy);
	} else {
		cairo_line_to(_cairo_context, x, y + h);
	}

  if (corners & JRC_TOP_LEFT) {
		cairo_line_to(_cairo_context, x, y + dy);
		cairo_line_to(_cairo_context, x + dx, y);
	} else {
    cairo_line_to(_cairo_context, x, y);
	}

  cairo_close_path(_cairo_context);
  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, abs(_pen.width));
	cairo_stroke(_cairo_context);
}

void Graphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	if (wp <=0 || hp <= 0) {
		return;
	}

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (corners & JRC_TOP_RIGHT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + w - dx, y + dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, -M_PI_2, 0.0);
		cairo_restore(_cairo_context);
	} else {
    cairo_line_to(_cairo_context, x + w, y);
	}

  if (corners & JRC_BOTTOM_RIGHT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + w - dx, y + h - dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_PI_2);
  	cairo_restore(_cairo_context);
	} else {
    cairo_line_to(_cairo_context, x + w, y + h);
	}

  if (corners & JRC_BOTTOM_LEFT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + dx, y + h - dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI_2, M_PI);
  	cairo_restore(_cairo_context);
	} else {
		cairo_line_to(_cairo_context, x, y + h);
	}

  if (corners & JRC_TOP_LEFT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + dx, y + dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI, M_PI+M_PI_2);
	  cairo_restore(_cairo_context);
	} else {
    cairo_line_to(_cairo_context, x, y);
	}

  cairo_close_path(_cairo_context);
	cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
}

void Graphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int line_width = _pen.width;

	if (wp <=0 || hp <= 0 || line_width == 0) {
		return;
	}

	if (line_width > 0) {
		line_width = line_width/2;

		x = x - line_width;
		y = y - line_width;
		w = w + 2*line_width;
		h = h + 2*line_width;
	} else {
		line_width = -line_width/2;

		x = x + line_width - 1;
		y = y + line_width - 1;
		w = w - 2*line_width + 2;
		h = h - 2*line_width + 2;
	}

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

  if (corners & JRC_TOP_RIGHT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + w - dx, y + dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, -M_PI_2, 0.0);
		cairo_restore(_cairo_context);
	} else {
    cairo_line_to(_cairo_context, x + w, y);
	}

  if (corners & JRC_BOTTOM_RIGHT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + w - dx, y + h - dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_PI_2);
  	cairo_restore(_cairo_context);
	} else {
    cairo_line_to(_cairo_context, x + w, y + h);
	}

  if (corners & JRC_BOTTOM_LEFT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + dx, y + h - dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI_2, M_PI);
  	cairo_restore(_cairo_context);
	} else {
		cairo_line_to(_cairo_context, x, y + h);
	}

  if (corners & JRC_TOP_LEFT) {
		cairo_save(_cairo_context);
		cairo_translate(_cairo_context, x + dx, y + dy);
		cairo_scale(_cairo_context, dx, dy);
    cairo_arc(_cairo_context, 0.0, 0.0, 1.0, M_PI, M_PI+M_PI_2);
	  cairo_restore(_cairo_context);
	} else {
    cairo_line_to(_cairo_context, x, y);
	}

  cairo_close_path(_cairo_context);
  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, abs(_pen.width));
	cairo_stroke(_cairo_context);
}

void Graphics::FillCircle(int xcp, int ycp, int rp)
{
	FillArc(xcp, ycp, rp, rp, 0.0, M_2PI);
}

void Graphics::DrawCircle(int xcp, int ycp, int rp)
{
	DrawArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void Graphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	FillArc(xcp, ycp, rxp, ryp, 0.0, M_2PI);
}

void Graphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	DrawArc(xcp, ycp, rxp, ryp, 0.0, 2*M_PI);
}

void Graphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc(_cairo_context, 0.0, 0.0, 1.0, arc1, arc0);
	cairo_close_path(_cairo_context);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
}

void Graphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int line_width = _pen.width;

	if (line_width > 0) {
		rx = rx + line_width / 2;
		ry = ry + line_width / 2;
	} else {
		line_width = -line_width;

		rx = rx - line_width / 2;
		ry = ry - line_width / 2;
	}

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc(_cairo_context, 0.0, 0.0, 1.0, arc1, arc0);
	cairo_close_path(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);
	cairo_stroke(_cairo_context);
}

void Graphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_line_to(_cairo_context, 0, 0);
	cairo_close_path(_cairo_context);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
}

void Graphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int line_width = _pen.width;

	if (line_width > 0) {
		rx = rx + line_width / 2;
		ry = ry + line_width / 2;
	} else {
		line_width = -line_width;

		rx = rx - line_width / 2;
		ry = ry - line_width / 2;
	}

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, abs(_pen.width));
	cairo_stroke(_cairo_context);
}

void Graphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	FillArc(xcp, ycp, rxp, ryp, arc0, arc1);
}

void Graphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int line_width = _pen.width;

	if (line_width > 0) {
		rx = rx + line_width / 2;
		ry = ry + line_width / 2;
	} else {
		line_width = -line_width;

		rx = rx - line_width / 2;
		ry = ry - line_width / 2;
	}

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_line_to(_cairo_context, 0, 0);
	cairo_close_path(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, abs(_pen.width));
	cairo_stroke(_cairo_context);
}
		
void Graphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	jpoint_t p[3];

	p[0].x = x1p;
	p[0].y = y1p;
	p[1].x = x2p;
	p[1].y = y2p;
	p[2].x = x3p;
	p[2].y = y3p;

	FillPolygon(0, 0, p, 3, true);
}

void Graphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	jpoint_t p[3];

	p[0].x = x1p;
	p[0].y = y1p;
	p[1].x = x2p;
	p[1].y = y2p;
	p[2].x = x3p;
	p[2].y = y3p;

	DrawPolygon(0, 0, p, 3, true);
}

void Graphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool closed)
{
	if (npoints < 1) {
		return;
	}

	int line_width = _pen.width;

	if (line_width < 0) {
		line_width = -line_width;
	}

	jpoint_t t[npoints];

	for (int i=0; i<npoints; i++) {
		t[i].x = xp+p[i].x+_translate.x;
		t[i].y = yp+p[i].y+_translate.y;
	}
	
	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);
  cairo_move_to(_cairo_context, t[0].x, t[0].y);
  
	for (int i=1; i < npoints; i++) {
    cairo_line_to(_cairo_context, t[i].x, t[i].y);
	}

	if (closed == true) {
  	cairo_close_path(_cairo_context);
	}
	
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);
	cairo_stroke(_cairo_context);
}

void Graphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
	if (npoints < 1) {
		return;
	}

	int line_width = _pen.width;

	if (line_width < 0) {
		line_width = -line_width;
	}

	jpoint_t t[npoints];

	for (int i=0; i<npoints; i++) {
		t[i].x = xp+p[i].x+_translate.x;
		t[i].y = yp+p[i].y+_translate.y;
	}
	
	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);
  cairo_move_to(_cairo_context, t[0].x, t[0].y);
  
	for (int i=1; i < npoints; i++) {
    cairo_line_to(_cairo_context, t[i].x, t[i].y);
	}

  cairo_close_path(_cairo_context);	
	cairo_restore(_cairo_context);

	if (even_odd == true) {
		cairo_set_fill_rule(_cairo_context, CAIRO_FILL_RULE_EVEN_ODD);
	}

	cairo_fill(_cairo_context);
	
	if (even_odd == true) {
		cairo_set_fill_rule(_cairo_context, CAIRO_FILL_RULE_WINDING);
	}
}

void Graphics::SetGradientStop(double stop, const Color &color)
{
	jgradient_t t;

	t.color = color;
	t.stop = stop;

	_gradient_stops.push_back(t);
}

void Graphics::ResetGradientStop()
{
	_gradient_stops.clear();
}

void Graphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = wp;
	int ry = hp;
	int x0 = _translate.x+xcp+x0p;
	int y0 = _translate.y+ycp+y0p;
	int r0 = r0p;

	cairo_pattern_t *pattern = cairo_pattern_create_radial(xc, yc, std::max(rx, ry), x0, y0, r0);

	for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
		jgradient_t gradient = (*i);

		int sr = gradient.color.GetRed(),
				sg = gradient.color.GetGreen(),
				sb = gradient.color.GetBlue(),
				sa = gradient.color.GetAlpha();

		cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	}

	cairo_set_source(_cairo_context, pattern);
	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_2PI);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_pattern_destroy(pattern);
}

void Graphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	
	int x1 = x1p;
	int y1 = y1p;
	int x2 = x2p;
	int y2 = y2p;

	cairo_pattern_t *pattern = cairo_pattern_create_linear(x1, y1, x2, y2);
	
	for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
		jgradient_t gradient = (*i);

		int sr = gradient.color.GetRed(),
				sg = gradient.color.GetGreen(),
				sb = gradient.color.GetBlue(),
				sa = gradient.color.GetAlpha();

		cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	}
	
	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, x, y);
	cairo_rectangle(_cairo_context, 0, 0, w, h);
	cairo_set_source(_cairo_context, pattern);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_pattern_destroy(pattern);
	
	SetCompositeFlags(_composite_flags);
}

jregion_t Graphics::GetStringExtends(std::string text)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

jregion_t Graphics::GetGlyphExtends(int symbol)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void Graphics::DrawString(std::string text, int xp, int yp)
{
	if (_font == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;

	Font *font = dynamic_cast<Font *>(_font);

	const char *utf8 = text.c_str();
	int utf8_len = text.size();
	cairo_glyph_t *glyphs = NULL;
	int glyphs_len = 0;
	cairo_status_t status;

	if (_font->GetEncoding() == JFE_ISO_8859_1) {
		jcommon::Charset charset;

		utf8 = charset.Latin1ToUTF8(utf8, &utf8_len);
	}

	status = cairo_scaled_font_text_to_glyphs(
			font->GetScaledFont(), x, y+_font->GetAscender(), utf8, utf8_len, &glyphs, &glyphs_len, NULL, NULL, NULL);

	if (status == CAIRO_STATUS_SUCCESS) {
		cairo_show_glyphs(_cairo_context, glyphs, glyphs_len);
		cairo_glyph_free(glyphs);
	}

	if (_font->GetEncoding() == JFE_ISO_8859_1) {
		delete [] utf8;
	}

	cairo_stroke(_cairo_context);
}

void Graphics::DrawGlyph(int symbol, int xp, int yp)
{
	if (_font == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	
	cairo_glyph_t glyph;

	glyph.x = x;
	glyph.y = y + _font->GetAscender();
	glyph.index = symbol;

	cairo_show_glyphs(_cairo_context, &glyph, 1);
}

void Graphics::Translate(int x, int y)
{
	_translate.x += x;
	_translate.y += y;
}

jpoint_t Graphics::Translate()
{
	return _translate;
}

void Graphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
	if (_font == NULL) {
		return;
	}

	if (wp < 0 || hp < 0) {
		return;
	}

	std::vector<std::string> words,
		lines;
	int dx = 0,
			dy = 0,
			max_lines,
			font_height;
	
	font_height = _font->GetSize();

	if (font_height <= 0) {
		return;
	}

	max_lines = hp/font_height;

	if (max_lines <= 0) {
		max_lines = 1;
	}

	_font->GetStringBreak(&lines, text, wp, hp, halign);

	int line_space = 0,
			line_yinit = 0,
			line_ydiff = 0;

	if (hp > (int)lines.size()*font_height) {
		int nlines = (int)lines.size();

		if (valign == JVA_TOP) {
			line_yinit = 0;
			line_ydiff = 0;
		} else if (valign == JVA_CENTER) {
			line_yinit = (hp-nlines*font_height)/2;
			line_ydiff = 0;
		} else if (valign == JVA_BOTTOM) {
			line_yinit = hp-nlines*font_height;
			line_ydiff = 0;
		} else if (valign == JVA_JUSTIFY) {
			if (nlines == 1) {
				line_yinit = (hp-nlines*font_height)/2;
			} else {
				line_space = (hp-nlines*font_height)/(nlines-1);
				line_yinit = 0;
				line_ydiff = (hp-nlines*font_height)%(nlines-1);
			}
		}
	}

	jregion_t clip = GetClip();

	if (clipped == true) {
		ClipRect(xp, yp, wp, hp);
	}
	
	if (halign == JHA_JUSTIFY) {
		std::string token_trim;
			
		dy = line_yinit;

		for (int i=0; i<(int)lines.size() && i<max_lines; i++) {
			jcommon::StringTokenizer token(lines[i], " ", jcommon::JTT_STRING, false);


			if (lines[i].find("\n") == 0) {
				// INFO:: eh soh uma maneira de informar a ultima linha de cada linha terminada com '\n'
				DrawString(lines[i].substr(1), xp, yp+dy);
			} else {
				int size = 0,
						space = 0,
						diff = 0;

				dx = 0;

				if (token.GetSize() > 1) {
					for (int j=0; j<token.GetSize(); j++) {
						size = size + _font->GetStringWidth(jcommon::StringUtils::Trim(token.GetToken(j)));
					}

					space = (wp-size)/(token.GetSize()-1);
					diff = (wp-size)%(token.GetSize()-1);
				}

				for (int j=0; j<token.GetSize(); j++) {
					std::string word = jcommon::StringUtils::Trim(token.GetToken(j));

					DrawString(word, xp+dx, yp+dy);

					dx = dx + space + _font->GetStringWidth(word);

					if (diff-- > 0) {
						dx++;
					}
				}
			}

			dy = dy+line_space+font_height;

			if (line_ydiff-- > 0) {
				dy++;
			}
		}
	} else {
		dy = line_yinit;

		for (int i=0; i<(int)lines.size() && i<max_lines; i++) {
			std::string text = jcommon::StringUtils::Trim(lines[i]);
			
			int size = _font->GetStringWidth(text);

			if (halign == JHA_LEFT) {
				dx = 0;
			} else if (halign == JHA_CENTER) {
				dx = (wp-size)/2;
			} else if (halign == JHA_RIGHT) {
				dx = wp-size;
			}

			DrawString(text, xp+dx, yp+dy);

			dy = dy+line_space+font_height;

			if (line_ydiff-- > 0) {
				dy++;
			}
		}
	}
	
	if (clipped == true) {
		SetClip(clip.x, clip.y, clip.width, clip.height);
	}
}

uint32_t Graphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return pixel;
	}

	cairo_surface_flush(cairo_surface);

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int sw = cairo_image_surface_get_width(cairo_surface);
	int sh = cairo_image_surface_get_height(cairo_surface);
	
	if ((x < 0 || x > sw) || (y < 0 || y > sh)) {
		throw jexception::OutOfBoundsException("Index out of bounds");
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return pixel;
	}

	int stride = cairo_image_surface_get_stride(cairo_surface);
	
	return *((uint32_t *)(data + y * stride) + x);
}

void Graphics::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	if (rgb == NULL) {
		throw jexception::NullPointerException("Pixel array is null");
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int sw = cairo_image_surface_get_width(cairo_surface);
	int sh = cairo_image_surface_get_height(cairo_surface);
	
	if ((x < 0 || (x+wp) > sw) || (y < 0 || (y+hp) > sh)) {
		throw jexception::OutOfBoundsException("Index out of bounds");
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	uint32_t *ptr = (*rgb);

	if (ptr == NULL) {
		ptr = new uint32_t[hp*wp];
	}

	int stride = cairo_image_surface_get_stride(cairo_surface);

	if (_pixelformat == JPF_ARGB) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(data + (y + j) * stride + x * 4);
			uint8_t *dst = (uint8_t *)(ptr + j * wp);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				int alpha = *(src + si + 3);
				*(dst + di + 3) = alpha;
				*(dst + di + 2) = ALPHA_DEMULTIPLY(*(src + si + 2), alpha);
				*(dst + di + 1) = ALPHA_DEMULTIPLY(*(src + si + 1), alpha);
				*(dst + di + 0) = ALPHA_DEMULTIPLY(*(src + si + 0), alpha);

				si = si + 4;
				di = di + 4;
			}
		}
	} else if (_pixelformat == JPF_RGB32) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(data + (y + j) * stride + x * 4);
			uint8_t *dst = (uint8_t *)(ptr + j * wp);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				*(dst + di + 3) = *(src + si + 3);
				*(dst + di + 2) = *(src + si + 2);
				*(dst + di + 1) = *(src + si + 1);
				*(dst + di + 0) = *(src + si + 0);

				si = si + 4;
				di = di + 4;
			}
		}
	} else if (_pixelformat == JPF_RGB24) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(data + (y + j) * stride + x * 3);
			uint8_t *dst = (uint8_t *)(ptr + j * wp);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				*(dst + di + 3) = 0xff;
				*(dst + di + 2) = *(src + si + 2);
				*(dst + di + 1) = *(src + si + 1);
				*(dst + di + 0) = *(src + si + 0);

				si = si + 4; // INFO:: cairo allocates 4 bytes instead 3 to rgb24's array
				di = di + 4;
			}
		}
	}

	(*rgb) = ptr;
}

void Graphics::SetRGB(uint32_t rgb, int xp, int yp) 
{
	try {
		SetRGBArray(&rgb, xp, yp, 1, 1);
	} catch (jexception::OutOfBoundsException &e) {
	}
}

void Graphics::SetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp) 
{
	if (rgb == NULL) {
		throw jexception::NullPointerException("Pixel array is null");
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int sw = cairo_image_surface_get_width(cairo_surface);
	int sh = cairo_image_surface_get_height(cairo_surface);
	
	if ((x < 0 || (x+wp) > sw) || (y < 0 || (y+hp) > sh)) {
		throw jexception::OutOfBoundsException("Index out of bounds");
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);
	int stride = cairo_image_surface_get_stride(cairo_surface);

	if (data == NULL) {
		return;
	}

	int step = stride / sw;

	if (_pixelformat == JPF_ARGB) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)(data + (y + j) * stride + x*step);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				int a = *(src + si + 3);
				int r = *(src + si + 2);
				int g = *(src + si + 1);
				int b = *(src + si + 0);
				int alpha = *(dst + di + 3);
				int pa = alpha;
				int pr = ALPHA_DEMULTIPLY(*(dst + di + 2), pa);
				int pg = ALPHA_DEMULTIPLY(*(dst + di + 1), pa);
				int pb = ALPHA_DEMULTIPLY(*(dst + di + 0), pa);

				if (_composite_flags == JCF_CLEAR) {
					pr = 0x00;
					pg = 0x00;
					pb = 0x00;
					pa = 0x00;
				} else if (_composite_flags == JCF_SRC) {
					pr = r;
					pg = g;
					pb = b;
					pa = a;
				} else if (_composite_flags == JCF_SRC_OVER) {
					double fr = r/255.0;
					double fg = g/255.0;
					double fb = b/255.0;
					double fa = a/255.0;
					double fpr = pr/255.0;
					double fpg = pg/255.0;
					double fpb = pb/255.0;
					double fpa = pa/255.0;
					double ar = fa + fpa*(1.0 - fa);

					pr = (int)(((fa*fr + fpa*fpr*(1.0 - fa))/ar)*255);
					pg = (int)(((fa*fg + fpa*fpg*(1.0 - fa))/ar)*255);
					pb = (int)(((fa*fb + fpa*fpb*(1.0 - fa))/ar)*255);
					pa = (int)(ar * 255);
				} else if (_composite_flags == JCF_SRC_IN) {
					pr = r;
					pg = g;
					pb = b;
					pa = (a*pa) >> 0x08;
				} else if (_composite_flags == JCF_SRC_OUT) {
					pr = r;
					pg = g;
					pb = b;
					pa = (a*(0xff-pa)) >> 0x08;
				} else if (_composite_flags == JCF_SRC_ATOP) {
					pr = (int)(a*r + pr*(0xff-a)) >> 0x08;
					pg = (int)(a*g + pg*(0xff-a)) >> 0x08;
					pb = (int)(a*b + pb*(0xff-a)) >> 0x08;
					// pa = pa;
				} else if (_composite_flags == JCF_DST) {
					pr = pr;
					pg = pg;
					pb = pb;
					pa = pa;
				} else if (_composite_flags == JCF_DST_OVER) {
					double fr = r/255.0;
					double fg = g/255.0;
					double fb = b/255.0;
					double fa = a/255.0;
					double fpr = pr/255.0;
					double fpg = pg/255.0;
					double fpb = pb/255.0;
					double fpa = pa/255.0;
					double ar = fpa + fa*(1.0 - fpa);

					pr = (int)(((fpa*fpr + fa*fr*(1.0 - fpa))/ar)*255);
					pg = (int)(((fpa*fpg + fa*fg*(1.0 - fpa))/ar)*255);
					pb = (int)(((fpa*fpb + fa*fb*(1.0 - fpa))/ar)*255);
					pa = (int)(ar * 255);
				} else if (_composite_flags == JCF_DST_IN) {
					// pr = pr;
					// pg = pg;
					// pb = pb;
					pa = (a*pa) >> 0x08;
				} else if (_composite_flags == JCF_DST_OUT) {
					pr = pr;
					pg = pg;
					pb = pb;
					pa = (pa*(0xff-a)) >> 0x08;
				} else if (_composite_flags == JCF_DST_ATOP) {
					pr = (int)(pr*pa + r*(0xff-pa)) >> 0x08;
					pg = (int)(pg*pa + g*(0xff-pa)) >> 0x08;
					pb = (int)(pb*pa + b*(0xff-pa)) >> 0x08;
					pa = a;
				} else if (_composite_flags == JCF_XOR) {
					double fr = r/255.0;
					double fg = g/255.0;
					double fb = b/255.0;
					double fa = a/255.0;
					double fpr = pr/255.0;
					double fpg = pg/255.0;
					double fpb = pb/255.0;
					double fpa = pa/255.0;
					double ar = fa + fpa - 2*fa*fpa;

					pr = (int)(((fa*fr*(1.0 - fpa) + fpa*fpr*(1.0 - fa))/ar) * 255);
					pg = (int)(((fa*fg*(1.0 - fpa) + fpa*fpg*(1.0 - fa))/ar) * 255);
					pb = (int)(((fa*fb*(1.0 - fpa) + fpa*fpb*(1.0 - fa))/ar) * 255);
					pa = (int)(ar * 255);
				} else if (_composite_flags == JCF_ADD) {
					double fr = r/255.0;
					double fg = g/255.0;
					double fb = b/255.0;
					double fa = a/255.0;
					double fpr = pr/255.0;
					double fpg = pg/255.0;
					double fpb = pb/255.0;
					double fpa = pa/255.0;
					double ar = fa + fpa;

					if (ar > 1.0) {
						ar = 1.0;
					}

					pr = (int)(((fa*fr + fpa*fpr)/ar) * 255);
					pg = (int)(((fa*fg + fpa*fpg)/ar) * 255);
					pb = (int)(((fa*fb + fpa*fpb)/ar) * 255);
					pa = (int)(ar * 255);
				} else if (_composite_flags == JCF_SATURATE) {
					double fr = r/255.0;
					double fg = g/255.0;
					double fb = b/255.0;
					double fa = a/255.0;
					double fpr = pr/255.0;
					double fpg = pg/255.0;
					double fpb = pb/255.0;
					double fpa = pa/255.0;
					double ar = fa + fpa;
					double ma = 1.0 - fpa;

					if (ar > 1.0) {
						ar = 1.0;
					}

					if (ma > fa) {
						ma = fa;
					}

					pr = (int)(((fr*ma + fpa*fpr)/ar) * 255);
					pg = (int)(((fg*ma + fpa*fpg)/ar) * 255);
					pb = (int)(((fb*ma + fpa*fpb)/ar) * 255);
					pa = (int)(ar * 255);
				} else {
					double fr = r/255.0;
					double fg = g/255.0;
					double fb = b/255.0;
					double fa = a/255.0;
					double fpr = pr/255.0;
					double fpg = pg/255.0;
					double fpb = pb/255.0;
					double fpa = pa/255.0;
					double ar = fa + fpa*(1.0 - fa);
					double c1 = 1.0/ar;
					double c2 = (1.0 - fpa)*fa;
					double c3 = (1.0 - fa)*fpa;
					double c4 = fa*fpa;
					double fxr = 0.0;
					double fxg = 0.0;
					double fxb = 0.0;

					if (_composite_flags == JCF_MULTIPLY) {
						fxr = (fr*fpr);
						fxg = (fg*fpg);
						fxb = (fb*fpb);
					} else if (_composite_flags == JCF_SCREEN) {
						fxr = (fr+fpr - (fr*fpr));
						fxg = (fg+fpg - (fg*fpg));
						fxb = (fb+fpb - (fb*fpb));
					} else if (_composite_flags == JCF_OVERLAY) {
						fxr = ((fpr <= 0.5)?(2.0*fr*fpr):(1.0 - 2.0*(1.0 - fr)*(1.0 - fpr)));
						fxg = ((fpg <= 0.5)?(2.0*fg*fpg):(1.0 - 2.0*(1.0 - fg)*(1.0 - fpg)));
						fxb = ((fpb <= 0.5)?(2.0*fb*fpb):(1.0 - 2.0*(1.0 - fb)*(1.0 - fpb)));
					} else if (_composite_flags == JCF_DARKEN) {
						fxr = ((fr < fpr)?fr:fpr);
						fxg = ((fg < fpg)?fg:fpg);
						fxb = ((fb < fpb)?fb:fpb);
					} else if (_composite_flags == JCF_LIGHTEN) {
						fxr = ((fr > fpr)?fr:fpr);
						fxg = ((fg > fpg)?fg:fpg);
						fxb = ((fb > fpb)?fb:fpb);
					} else if (_composite_flags == JCF_DIFFERENCE) {
						fxr = (abs(fr-fpr));
						fxg = (abs(fg-fpg));
						fxb = (abs(fb-fpb));
					} else if (_composite_flags == JCF_EXCLUSION) {
						fxr = (fr+fpr - 2*(fr*fpr));
						fxg = (fg+fpg - 2*(fg*fpg));
						fxb = (fb+fpb - 2*(fb*fpb));
					} else if (_composite_flags == JCF_DODGE) {
						double cr = fpr/(1.0 - fr);
						double cg = fpg/(1.0 - fg);
						double cb = fpb/(1.0 - fb);

						if (cr > 1.0) {
							cr = 1.0;
						}

						if (cg > 1.0) {
							cg = 1.0;
						}

						if (cb > 1.0) {
							cb = 1.0;
						}

						fxr = (fr < 1.0)?cr:1.0;
						fxg = (fg < 1.0)?cg:1.0;
						fxb = (fb < 1.0)?cb:1.0;
					} else if (_composite_flags == JCF_BURN) {
						double cr = (1.0 - fpr)/fr;
						double cg = (1.0 - fpg)/fg;
						double cb = (1.0 - fpb)/fb;

						if (cr > 1.0) {
							cr = 1.0;
						}

						if (cg > 1.0) {
							cg = 1.0;
						}

						if (cb > 1.0) {
							cb = 1.0;
						}

						fxr = (fr > 0.0)?(1.0-cr):0.0;
						fxg = (fg > 0.0)?(1.0-cg):0.0;
						fxb = (fb > 0.0)?(1.0-cb):0.0;
					} else if (_composite_flags == JCF_HARD) {
						fxr = (fr <= 0.5)?(2*fr*fpr):(1.0 - 2*(1.0 - fr)*(1.0 - fpr));
						fxg = (fg <= 0.5)?(2*fg*fpg):(1.0 - 2*(1.0 - fg)*(1.0 - fpg));
						fxb = (fb <= 0.5)?(2*fb*fpb):(1.0 - 2*(1.0 - fb)*(1.0 - fpb));
					} else if (_composite_flags == JCF_LIGHT) {
						double gr = (fpr <= 0.25)?(((16*fpr - 12)*fpr + 4)*fpr):(sqrt(fpr));
						double gg = (fpg <= 0.25)?(((16*fpg - 12)*fpg + 4)*fpg):(sqrt(fpg));
						double gb = (fpb <= 0.25)?(((16*fpb - 12)*fpb + 4)*fpb):(sqrt(fpb));

						fxr = (fr <= 0.5)?(fpr - (1.0 - 2*fr)*fpr*(1.0 - fpr)):(fpr + (2*fr - 1.0)*(gr - fpr));
						fxg = (fg <= 0.5)?(fpg - (1.0 - 2*fg)*fpg*(1.0 - fpg)):(fpg + (2*fg - 1.0)*(gg - fpg));
						fxb = (fb <= 0.5)?(fpb - (1.0 - 2*fb)*fpb*(1.0 - fpb)):(fpb + (2*fb - 1.0)*(gb - fpb));
					}
						
					pr = (int)((c1 * (c2*fr + c3*fpr + c4*fxr)) * 255);
					pg = (int)((c1 * (c2*fg + c3*fpg + c4*fxg)) * 255);
					pb = (int)((c1 * (c2*fb + c3*fpb + c4*fxb)) * 255);
					pa = (int)(ar * 255);
				}

				pr = (pr > 0xff)?0xff:pr;
				pg = (pg > 0xff)?0xff:pg;
				pb = (pb > 0xff)?0xff:pb;
				pa = (pa > 0xff)?0xff:pa;

				*(dst + di + 3) = pa;
				*(dst + di + 2) = ALPHA_PREMULTIPLY(pr, pa);
				*(dst + di + 1) = ALPHA_PREMULTIPLY(pg, pa);
				*(dst + di + 0) = ALPHA_PREMULTIPLY(pb, pa);

				si = si + 4;
				di = di + 4;
			}
		}
	} else if (_pixelformat == JPF_RGB32) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)(data + (y + j) * stride + x * step);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				*(dst + di + 3) = *(src + si + 3);
				*(dst + di + 2) = *(src + si + 2);
				*(dst + di + 1) = *(src + si + 1);
				*(dst + di + 0) = *(src + si + 0);

				si = si + 4;
				di = di + 4;
			}
		}
	} else if (_pixelformat == JPF_RGB24) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)(data + (y + j) * stride + x * step);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				*(dst + di + 2) = *(src + si + 2);
				*(dst + di + 1) = *(src + si + 1);
				*(dst + di + 0) = *(src + si + 0);

				si = si + 4;
				di = di + 4;
			}
		}
	} else if (_pixelformat == JPF_RGB16) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)(data + (y + j) * stride + x * step);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				int r = *(src + si + 2);
				int g = *(src + si + 1);
				int b = *(src + si + 0);

				*(dst + di + 1) = (r << 0x03 | g >> 0x03) & 0xff;
				*(dst + di + 0) = (g << 0x03 | b >> 0x00) & 0xff;

				si = si + 4;
				di = di + 2;
			}
		}
	}
	
	cairo_surface_mark_dirty(cairo_surface);
}

bool Graphics::DrawImage(Image *img, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

  Graphics *g = img->GetGraphics();
  jsize_t size = img->GetSize();

	if (g != NULL) {
		cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

		if (cairo_surface == NULL) {
			return false;
		}

		jregion_t clip = GetClip();

		SetClip(xp, yp, size.width, size.height);

		cairo_surface_flush(cairo_surface);

		cairo_save(_cairo_context);
		cairo_set_source_surface(_cairo_context, cairo_surface, xp+_translate.x, yp+_translate.y);
		cairo_paint(_cairo_context);
		cairo_restore(_cairo_context);

		SetClip(clip.x, clip.y, clip.width, clip.height);
	} else {
		uint32_t *rgb = NULL;

		img->GetRGBArray(&rgb, 0, 0, size.width, size.height);
	
		if (rgb != NULL) {
			SetRGBArray(rgb, xp, yp, size.width, size.height);

			delete [] rgb;
		}
	}

	return true;
	
  // return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, img->GetWidth(), img->GetHeight());
}

bool Graphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	if (wp <= 0 || hp <= 0) {
		return false;
	}

	if (wp > IMAGE_SIZE_LIMIT || hp > IMAGE_SIZE_LIMIT) {
		return false;
	}

	jgui::Image *scl = img->Scale(wp, hp);

	if (scl == NULL) {
		return false;
	}

	Graphics *g = scl->GetGraphics();

	if (g != NULL) {
		cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

		if (cairo_surface == NULL) {
			return false;
		}

		jregion_t clip = GetClip();

		SetClip(xp, yp, wp, hp);

		cairo_surface_flush(cairo_surface);

		cairo_save(_cairo_context);
		cairo_set_source_surface(_cairo_context, cairo_surface, xp+_translate.x, yp+_translate.y);
		cairo_paint(_cairo_context);
		cairo_restore(_cairo_context);

		SetClip(clip.x, clip.y, clip.width, clip.height);
	} else {
		uint32_t *rgb = NULL;

		scl->GetRGBArray(&rgb, 0, 0, wp, hp);
	
		if (rgb != NULL) {
			SetRGBArray(rgb, xp, yp, wp, hp);

			delete [] rgb;
		}
	}

	delete scl;

  return true;

	// return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool Graphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, sxp, syp, swp, shp, xp, yp, swp, shp);
}

bool Graphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	if (swp <= 0 || shp <= 0 || wp <= 0 || hp <= 0) {
		return false;
	}

	if (swp > IMAGE_SIZE_LIMIT || shp > IMAGE_SIZE_LIMIT || wp > IMAGE_SIZE_LIMIT || hp > IMAGE_SIZE_LIMIT) {
		return false;
	}

	jgui::Image *aux = img->Crop(sxp, syp, swp, shp);

	if (aux == NULL) {
		return false;
	}

	jgui::Image *scl = aux->Scale(wp, hp);

	if (scl == NULL) {
		return false;
	}

	delete aux;

	Graphics *g = scl->GetGraphics();

	if (g != NULL) {
		cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

		if (cairo_surface == NULL) {
			return false;
		}

		jregion_t clip = GetClip();

		SetClip(xp, yp, wp, hp);

		cairo_surface_flush(cairo_surface);

		cairo_save(_cairo_context);
		cairo_set_source_surface(_cairo_context, cairo_surface, xp+_translate.x, yp+_translate.y);
		cairo_paint(_cairo_context);
		cairo_restore(_cairo_context);

		SetClip(clip.x, clip.y, clip.width, clip.height);
	} else {
		uint32_t *rgb = NULL;

		scl->GetRGBArray(&rgb, 0, 0, wp, hp);
	
		if (rgb != NULL) {
			SetRGBArray(rgb, xp, yp, wp, hp);

			delete [] rgb;
		}
	}

	delete scl;

	return true;
}

void Graphics::SetPattern(Image *image)
{
	if (image == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(image->GetGraphics()->GetCairoContext());

	if (cairo_surface == NULL) {
		return;
	}

	// struct jpoint_t t = Translate();

	// int x0 = xp+t.x;
	// int y0 = yp+t.y;
	// int x1 = wp+t.x;
	// int y1 = hp+t.y;

	cairo_pattern_t *pattern = cairo_pattern_create_for_surface(cairo_surface);

	cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
	cairo_set_source(_cairo_context, pattern);
	// cairo_rectangle(_cairo_context, x0, y0, x1-x0, y1-y0);
	cairo_fill(_cairo_context);
	cairo_pattern_destroy(pattern);
}

void Graphics::SetPattern(int x0p, int y0p, int x1p, int y1p)
{
	struct jpoint_t t = Translate();

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

	for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
		jgradient_t gradient = (*i);

		int sr = gradient.color.GetRed(),
				sg = gradient.color.GetGreen(),
				sb = gradient.color.GetBlue(),
				sa = gradient.color.GetAlpha();

		cairo_pattern_add_color_stop_rgba(pattern, gradient.stop, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	}

	// cairo_rectangle(_cairo_context, x, y, w, h);
	cairo_set_source(_cairo_context, pattern);
	cairo_fill(_cairo_context);
	cairo_pattern_destroy(pattern);
}

void Graphics::SetPattern(int x0p, int y0p, int rad0, int x1p, int y1p, int rad1)
{
	cairo_t *cairo_context = _cairo_context;

	struct jpoint_t t = Translate();

	int x0 = x0p+t.x;
	int y0 = y0p+t.y;
	int x1 = x1p+t.x;
	int y1 = y1p+t.y;
		
	cairo_pattern_t *pattern = cairo_pattern_create_radial(x0, y0, rad0, x1, y1, rad1);

	for (std::vector<jgradient_t>::iterator i=_gradient_stops.begin(); i!=_gradient_stops.end(); i++) {
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

void Graphics::MoveTo(int xp, int yp)
{
	struct jpoint_t t = Translate();

	int x = xp+t.x;
	int y = yp+t.y;

	cairo_move_to(_cairo_context, x, y);
}

void Graphics::LineTo(int xp, int yp)
{
	struct jpoint_t t = Translate();

	int x = xp+t.x;
	int y = yp+t.y;

	cairo_line_to(_cairo_context, x, y);
}

void Graphics::CurveTo(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	struct jpoint_t t = Translate();

	int x1 = x1p+t.x;
	int y1 = y1p+t.y;
	int x2 = x2p+t.x;
	int y2 = y2p+t.y;
	int x3 = x3p+t.x;
	int y3 = y3p+t.y;

	cairo_curve_to(_cairo_context, (double)x1, (double)y1, (double)x2, (double)y2, (double)x3, (double)y3);
}

void Graphics::ArcTo(int xcp, int ycp, int radius, double arc0, double arc1, bool negative)
{
	struct jpoint_t t = Translate();

	int xc = xcp+t.x;
	int yc = ycp+t.y;

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	if (negative == false) {
		cairo_arc(_cairo_context, xc, yc, radius, arc0, arc1);
	} else {
		cairo_arc_negative(_cairo_context, xc, yc, radius, arc0, arc1);
	}
}

void Graphics::TextTo(std::string text, int xp, int yp)
{
	Font *font = dynamic_cast<Font *>(GetFont());

	if (font == NULL) {
		return;
	}

	struct jpoint_t t = Translate();

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
			font->GetScaledFont(), x, y+font->GetAscender(), utf8, utf8_len, &glyphs, &glyphs_len, NULL, NULL, NULL);

	if (status == CAIRO_STATUS_SUCCESS) {
		cairo_glyph_path(_cairo_context, glyphs, glyphs_len);
		cairo_glyph_free(glyphs);
	}

	if (font->GetEncoding() == JFE_ISO_8859_1) {
		delete [] utf8;
	}
}

void Graphics::Close()
{
	cairo_close_path(_cairo_context);
}

void Graphics::Stroke()
{
	int width = _pen.width;

	if (width < 0) {
		width = -width;
	}


	cairo_set_line_width(_cairo_context, width);
	cairo_stroke(_cairo_context);
}

void Graphics::Fill()
{
	cairo_fill(_cairo_context);
}

void Graphics::SetSource(Image *image)
{
	Graphics *g = dynamic_cast<Graphics *>(image->GetGraphics());

	if (g == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

	cairo_set_source_surface(_cairo_context, cairo_surface, 0, 0);
}

void Graphics::SetMask(Image *image)
{
	Graphics *g = dynamic_cast<Graphics *>(image->GetGraphics());

	if (g == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(g->GetCairoContext());

	cairo_mask_surface(_cairo_context, cairo_surface, 0, 0);
}

void Graphics::SetMatrix(double *matrix)
{
}

void Graphics::GetMatrix(double **matrix)
{
}

void Graphics::Reset()
{
	// _translate.x = 0;
	// _translate.y = 0;

	_pen.dashes = NULL;
	_pen.dashes_size = 0;
	_pen.width = 1;
	_pen.join = JLJ_MITER;
	_pen.style = JLS_BUTT;

  // ReleaseClip();
	SetAntialias(JAM_NORMAL);
	SetPen(_pen);
	SetColor(0x00000000);
	ResetGradientStop();
	SetCompositeFlags(JCF_SRC_OVER);
}

void Graphics::SetVerticalSyncEnabled(bool enabled)
{
  _is_vertical_sync_enabled = enabled;
}

bool Graphics::IsVerticalSyncEnabled()
{
  return _is_vertical_sync_enabled;
}

}
