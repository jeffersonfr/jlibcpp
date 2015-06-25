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
#include "jmath.h"
#include "jstringtokenizer.h"
#include "jimage.h"
#include "jgfxhandler.h"
#include "jfont.h"
#include "jstringutils.h"
#include "jrectangle.h"
#include "jdfbfont.h"
#include "jdfbgraphics.h"
#include "jdfbimage.h"
#include "joutofboundsexception.h"
#include "jnullpointerexception.h"

#define M_2PI	(2*M_PI)

namespace jgui {

DFBGraphics::DFBGraphics(void *surface, int wp, int hp):
	jgui::Graphics()
{
	jcommon::Object::SetClassName("jgui::DFBGraphics");

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = wp;
	_clip.height = hp;

	_surface = (IDirectFBSurface *)surface;

	_cairo_context = NULL;

	cairo_surface_t *cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wp, hp);

	_cairo_context = cairo_create(cairo_surface);
	
	cairo_surface_destroy(cairo_surface);

	Reset();
}

DFBGraphics::~DFBGraphics()
{
	cairo_destroy(_cairo_context);
}

void * DFBGraphics::GetNativeSurface()
{
	return _surface;
}

void DFBGraphics::SetNativeSurface(void *data, int wp, int hp)
{
	_surface = (IDirectFBSurface *)data;

	cairo_destroy(_cairo_context);

	_cairo_context = NULL;

	if (_surface != NULL) {
		int sw;
		int sh;

		_surface->GetSize(_surface, &sw, &sh);

		cairo_surface_t *cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

		_cairo_context = cairo_create(cairo_surface);
	}
}

void DFBGraphics::Dump(std::string dir, std::string prefix)
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	jio::File *temp = jio::File::CreateTemporaryFile(prefix);
	std::string path = dir + "/" + temp->GetName() + ".png";

	delete temp;

	cairo_surface_flush(cairo_surface);
	cairo_surface_write_to_png(cairo_surface, path.c_str());
}

jregion_t DFBGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, _internal_clip.x, _internal_clip.y, _internal_clip.width, _internal_clip.height);
	
	SetClip(clip.x-_translate.x, clip.y-_translate.y, clip.width, clip.height);

	return clip;
}

void DFBGraphics::SetClip(int xp, int yp, int wp, int hp)
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

jregion_t DFBGraphics::GetClip()
{
	return _clip;
}

void DFBGraphics::ReleaseClip()
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

void DFBGraphics::SetCompositeFlags(jcomposite_flags_t t)
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
	} else if (_composite_flags == JCF_ADD) {
		o = CAIRO_OPERATOR_ADD;
	} else if (_composite_flags == JCF_XOR) {
		o = CAIRO_OPERATOR_XOR;
	}

	cairo_set_operator(_cairo_context, o);
}

jcomposite_flags_t DFBGraphics::GetCompositeFlags()
{
	return _composite_flags;
}

void DFBGraphics::Clear()
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

void DFBGraphics::Clear(int xp, int yp, int wp, int hp)
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

void DFBGraphics::Idle()
{
	IDirectFB *engine = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

	engine->WaitIdle(engine);
	engine->WaitForSync(engine);
}

void DFBGraphics::Flip()
{
	if (_surface == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);
	
	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	int dw = cairo_image_surface_get_width(cairo_surface);
	int dh = cairo_image_surface_get_height(cairo_surface);
	int stride = cairo_image_surface_get_stride(cairo_surface);

	void *ptr;
	int sw;
	int sh;
	int pitch;

	_surface->GetSize(_surface, &sw, &sh);
	_surface->Lock(_surface, DSLF_WRITE, &ptr, &pitch);

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	for (int j=0; j<sh && j<dh; j++) {
		uint32_t *src = (uint32_t *)(data + j * stride);
		uint32_t *dst = (uint32_t *)((uint8_t *)ptr + j * pitch);
		
		for (int i=0; i<sw && i<dw; i++) {
			*(dst + i) = *(src + i);
		}
	}

	_surface->Unlock(_surface);
		
	if (_vertical_sync == false) {
		_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	} else {
		_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
	}
}

void DFBGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	int dw = cairo_image_surface_get_width(cairo_surface);
	int dh = cairo_image_surface_get_height(cairo_surface);
	int stride = cairo_image_surface_get_stride(cairo_surface);

	int x = xp;
	int y = yp;
	int w = wp;
	int h = hp;

	DFBRegion rgn;

	rgn.x1 = x;
	rgn.y1 = y;
	rgn.x2 = x+w;
	rgn.y2 = y+h;

	void *ptr;
	int sw;
	int sh;
	int pitch;

	_surface->GetSize(_surface, &sw, &sh);
	_surface->Lock(_surface, DSLF_WRITE, &ptr, &pitch);

	cairo_surface_flush(cairo_surface);

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	for (int j=0; j<sh && j<dh; j++) {
		uint32_t *src = (uint32_t *)(data + j * stride);
		uint32_t *dst = (uint32_t *)((uint8_t *)ptr + j * pitch);
		
		for (int i=0; i<sw && i<dw; i++) {
			*(dst + i) = *(src + i);
		}
	}


	_surface->Unlock(_surface);

	if (_vertical_sync == false) {
		_surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	} else {
		_surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
	}
}

Color & DFBGraphics::GetColor()
{
	return _color;
} 

void DFBGraphics::SetColor(const Color &color)
{
	_color = color;

	int r = _color.GetRed(),
			g = _color.GetGreen(),
			b = _color.GetBlue(),
			a = _color.GetAlpha();

	cairo_set_source_rgba(_cairo_context, r/255.0, g/255.0, b/255.0, a/255.0);
} 

void DFBGraphics::SetColor(uint32_t color)
{
	SetColor(_color = Color(color));
} 

void DFBGraphics::SetColor(int red, int green, int blue, int alpha)
{
	SetColor(_color = Color(red, green, blue, alpha));
} 

void DFBGraphics::SetFont(Font *font)
{
	_font = font;
	
	if (_font != NULL) {
		_font->ApplyContext(_cairo_context);
	} else {
		cairo_set_font_face(_cairo_context, NULL);
	}
}

void DFBGraphics::SetAntialias(jantialias_mode_t mode)
{
	Graphics::SetAntialias(mode);

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

void DFBGraphics::SetLineJoin(jline_join_t t)
{
	_line_join = t;

	if (_line_join == JLJ_BEVEL) {
		cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_BEVEL);
	} else if (_line_join == JLJ_ROUND) {
		cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_ROUND);
	} else if (_line_join == JLJ_MITER) {
		cairo_set_line_join(_cairo_context, CAIRO_LINE_JOIN_MITER);
	}
}

void DFBGraphics::SetLineStyle(jline_style_t t)
{
	_line_style = t;

	if (_line_style == JLS_ROUND) {
		cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_ROUND);
	} else if (_line_style == JLS_BUTT) {
		cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_BUTT);
	} else if (_line_style == JLS_SQUARE) {
		cairo_set_line_cap(_cairo_context, CAIRO_LINE_CAP_SQUARE);
	}
}

void DFBGraphics::SetLineWidth(int size)
{
	_line_width = size;
}

void DFBGraphics::SetLineDash(double *dashes, int ndashes)
{
	cairo_set_dash(_cairo_context, dashes, ndashes, 0.0);
}

jline_join_t DFBGraphics::GetLineJoin()
{
	return _line_join;
}

jline_style_t DFBGraphics::GetLineStyle()
{
	return _line_style;
}

int DFBGraphics::GetLineWidth()
{
	return _line_width;
}

void DFBGraphics::DrawLine(int xp, int yp, int xf, int yf)
{
	int x0 = _translate.x+xp;
	int y0 = _translate.y+yp;
	int x1 = _translate.x+xf;
	int y1 = _translate.y+yf;
	int line_width = _line_width;

	if (line_width < 0) {
		line_width = -line_width;
	}

	cairo_save(_cairo_context);
	cairo_move_to(_cairo_context, x0, y0);
	cairo_line_to(_cairo_context, x1, y1);
	cairo_set_line_width(_cairo_context, line_width);
	cairo_stroke(_cairo_context);
	cairo_restore(_cairo_context);

	ApplyDrawing();
}

void DFBGraphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
	if (_line_width == 0) {
		return;
	}

	if (npoints < 3) {
		return;
	}

	if (interpolation < 2) {
		return;
	}

	int line_width = _line_width;

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

	ApplyDrawing();
}

void DFBGraphics::FillRectangle(int xp, int yp, int wp, int hp)
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

void DFBGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int line_width = _line_width;
	
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
	cairo_set_line_width(_cairo_context, abs(_line_width));

	ApplyDrawing();
}

void DFBGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
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

void DFBGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int line_width = _line_width;

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
	cairo_set_line_width(_cairo_context, abs(_line_width));

	ApplyDrawing();
}

void DFBGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
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

void DFBGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int line_width = _line_width;

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
	cairo_set_line_width(_cairo_context, abs(_line_width));

	ApplyDrawing();
}

void DFBGraphics::FillCircle(int xcp, int ycp, int rp)
{
	FillArc(xcp, ycp, rp, rp, 0.0, M_2PI);
}

void DFBGraphics::DrawCircle(int xcp, int ycp, int rp)
{
	DrawArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void DFBGraphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	FillArc(xcp, ycp, rxp, ryp, 0.0, M_2PI);
}

void DFBGraphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	DrawArc(xcp, ycp, rxp, ryp, 0.0, 2*M_PI);
}

void DFBGraphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
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

void DFBGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int line_width = _line_width;

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
	
	ApplyDrawing();
}

void DFBGraphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
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

void DFBGraphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int line_width = _line_width;

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
	cairo_set_line_width(_cairo_context, abs(_line_width));
	
	ApplyDrawing();
}

void DFBGraphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	FillArc(xcp, ycp, rxp, ryp, arc0, arc1);
}

void DFBGraphics::SetDrawingMode(jdrawing_mode_t mode)
{
	_drawing_mode = mode;
}

jdrawing_mode_t DFBGraphics::GetDrawingMode()
{
	return _drawing_mode;
}

void DFBGraphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int line_width = _line_width;

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
	cairo_set_line_width(_cairo_context, abs(_line_width));

	ApplyDrawing();
}
		
void DFBGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
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

void DFBGraphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
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

void DFBGraphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool close)
{
	if (npoints < 1) {
		return;
	}

	int line_width = _line_width;

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

	if (close == true) {
  	cairo_close_path(_cairo_context);
	}
	
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);

  ApplyDrawing();
}

void DFBGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
	if (npoints < 1) {
		return;
	}

	int line_width = _line_width;

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

void DFBGraphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
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

void DFBGraphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
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


void DFBGraphics::DrawString(std::string text, int xp, int yp)
{
	if (_font == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int line_width = _line_width;

	if (line_width < 0) {
		line_width = -line_width;
	}

	cairo_move_to(_cairo_context, x, y+_font->GetAscender());
	cairo_show_text(_cairo_context, text.c_str());
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
}

void DFBGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	if (_font == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	
	cairo_glyph_t glyph;

	glyph.x = 0;
	glyph.y = 0;
	glyph.index = symbol;

	cairo_move_to(_cairo_context, x, y+_font->GetAscender());
	cairo_show_glyphs(_cairo_context, &glyph, 1);
}

void DFBGraphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
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

uint32_t DFBGraphics::GetRGB(int xp, int yp, uint32_t safe)
{
	try {
		uint32_t rgb[1];

		GetRGBArray((uint32_t **)&rgb, xp, yp, 1, 1);

		return rgb[0];
	} catch (jcommon::OutOfBoundsException &e) {
	}

	return safe;
}

void DFBGraphics::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	if (rgb == NULL) {
		throw jcommon::NullPointerException("Pixel array is null");
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
		throw jcommon::OutOfBoundsException("Index out of bounds");
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

	for (int j=0; j<hp; j++) {
		uint32_t *src = (uint32_t *)(data + (yp + y + j) * stride);
		uint32_t *dst = (uint32_t *)(ptr + j * wp);
		
		for (int i=0; i<wp; i++) {
			*(dst + i) = *(src + xp + x + i);
		}
	}

	(*rgb) = ptr;
}

void DFBGraphics::SetRGB(uint32_t rgb, int xp, int yp) 
{
	try {
		SetRGBArray(&rgb, xp, yp, 1, 1);
	} catch (jcommon::OutOfBoundsException &e) {
	}
}

void DFBGraphics::SetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp) 
{
	if (rgb == NULL) {
		throw jcommon::NullPointerException("Pixel array is null");
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
		throw jcommon::OutOfBoundsException("Index out of bounds");
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);
	int stride = cairo_image_surface_get_stride(cairo_surface);

	if (data == NULL) {
		return;
	}

	int pitch = 4*wp;

	for (int j=0; j<hp; j++) {
		uint8_t *src = (uint8_t *)(rgb + j * wp);
		uint8_t *dst = (uint8_t *)(data + (y + j) * stride + 4*x);
		
		for (int i=0; i<pitch; i+=4) {
			if (_composite_flags == JCF_CLEAR) {
				*(dst + i + 3) = 0x00;
				*(dst + i + 2) = 0x00;
				*(dst + i + 1) = 0x00;
				*(dst + i + 0) = 0x00;
			} else if (_composite_flags == JCF_SRC) {
				*(dst + i + 3) = *(src + i + 3);
				*(dst + i + 2) = *(src + i + 2);
				*(dst + i + 1) = *(src + i + 1);
				*(dst + i + 0) = *(src + i + 0);
			} else if (_composite_flags == JCF_SRC_OVER) {
				int a = *(src + i + 3);
				int r = *(src + i + 2);
				int g = *(src + i + 1);
				int b = *(src + i + 0);
				// int pa = *(dst + i + 3);
				int pr = *(dst + i + 2);
				int pg = *(dst + i + 1);
				int pb = *(dst + i + 0);

				pr = (int)(pr*(0xff-a) + r*a) >> 0x08;
				pg = (int)(pg*(0xff-a) + g*a) >> 0x08;
				pb = (int)(pb*(0xff-a) + b*a) >> 0x08;

				*(dst + i + 3) = a;
				*(dst + i + 2) = pr;
				*(dst + i + 1) = pg;
				*(dst + i + 0) = pb;
			} else if (_composite_flags == JCF_SRC_IN) {
			} else if (_composite_flags == JCF_SRC_OUT) {
			} else if (_composite_flags == JCF_SRC_ATOP) {
			} else if (_composite_flags == JCF_DST) {
				// do nothing
			} else if (_composite_flags == JCF_DST_OVER) {
				// int a = *(src + i + 3);
				int r = *(src + i + 2);
				int g = *(src + i + 1);
				int b = *(src + i + 0);
				int pa = *(dst + i + 3);
				int pr = *(dst + i + 2);
				int pg = *(dst + i + 1);
				int pb = *(dst + i + 0);

				pr = (int)(pr*(0xff-pa) + r*pa) >> 0x08;
				pg = (int)(pg*(0xff-pa) + g*pa) >> 0x08;
				pb = (int)(pb*(0xff-pa) + b*pa) >> 0x08;

				*(dst + i + 3) = pa;
				*(dst + i + 2) = pr;
				*(dst + i + 1) = pg;
				*(dst + i + 0) = pb;
			} else if (_composite_flags == JCF_DST_IN) {
			} else if (_composite_flags == JCF_DST_OUT) {
			} else if (_composite_flags == JCF_DST_ATOP) {
			} else if (_composite_flags == JCF_ADD) {
			} else if (_composite_flags == JCF_XOR) {
				*(dst + x + i) ^= *(src + i);
			}
		}
	}
	
	cairo_surface_mark_dirty(cairo_surface);
}

bool DFBGraphics::DrawImage(Image *img, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp);
}

bool DFBGraphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool DFBGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	jgui::Image *aux = img->Crop(sxp, syp, swp, shp);

	if (aux == NULL) {
		return false;
	}

	DFBGraphics *g = dynamic_cast<DFBGraphics *>(aux->GetGraphics());

	if (g != NULL) {
		cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<DFBGraphics *>(g)->_cairo_context);

		if (cairo_surface == NULL) {
			return false;
		}

		cairo_surface_flush(cairo_surface);
	
		cairo_save(_cairo_context);
		cairo_set_source_surface(_cairo_context, cairo_surface, xp+_translate.x, yp+_translate.y);
		cairo_paint(_cairo_context);
		cairo_restore(_cairo_context);
	} else {
		uint32_t *rgb = NULL;

		aux->GetRGBArray(&rgb, 0, 0, swp, shp);
	
		if (rgb != NULL) {
			SetRGBArray(rgb, xp, yp, swp, shp);

			delete [] rgb;
		}
	}

	delete aux;

	return true;
}

bool DFBGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
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

	DFBGraphics *g = dynamic_cast<DFBGraphics *>(scl->GetGraphics());

	if (g != NULL) {
		cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<DFBGraphics *>(g)->_cairo_context);

		if (cairo_surface == NULL) {
			return false;
		}

		cairo_surface_flush(cairo_surface);

		cairo_save(_cairo_context);
		cairo_set_source_surface(_cairo_context, cairo_surface, xp+_translate.x, yp+_translate.y);
		cairo_paint(_cairo_context);
		cairo_restore(_cairo_context);
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

void DFBGraphics::Reset()
{
	SetAntialias(JAM_NORMAL);

	SetColor(0x00000000);

	SetLineWidth(1);
	SetLineJoin(JLJ_MITER);
	SetLineStyle(JLS_BUTT);
	SetLineDash(NULL, 0);

	ResetGradientStop();
	SetCompositeFlags(JCF_SRC_OVER);
	SetDrawingMode(JDM_STROKE);
}

void DFBGraphics::ApplyDrawing()
{
	/* TODO:: apply paths
	if (_dashPattern) {
		cairo_set_dash(context, _dashPattern, _dashCount, _dashOffset);
	} else {
		cairo_set_dash(context, 0, 0, 0);
	}

	if (_mode == GradientMode && _gradient.getType() != Gradient::None) {
		cairo_set_source(context, _gradient.getCairoGradient());
	} else {
		cairo_set_source_rgba(context, _color.red(), _color.green(), _color.blue(), _color.alpha());
	}
	*/

	if (_drawing_mode == JDM_PATH) {
	} else if (_drawing_mode == JDM_STROKE) {
		cairo_stroke(_cairo_context);
	} else if (_drawing_mode == JDM_FILL) {
		cairo_fill(_cairo_context);
	}
}

double DFBGraphics::EvaluateBezier0(double *data, int ndata, double t) 
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

}
