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
#include "joutofboundsexception.h"
#include "jrectangle.h"
#include "jdfbfont.h"
#include "jdfbgraphics.h"
#include "jdfbimage.h"

#define M_2PI	(2*M_PI)

namespace jgui {

DFBGraphics::DFBGraphics(DFBImage *image, void *surface, bool premultiplied, int scale_width, int scale_height):
	jgui::Graphics()
{
	jcommon::Object::SetClassName("jgui::DFBGraphics");

	_image = image;

	_is_premultiply = premultiplied;

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = scale_width;
	_scale.height = scale_height;

	_surface = (IDirectFBSurface *)surface;

	_cairo_surface = NULL;
	_cairo_context = NULL;

	if (_surface != NULL) {
		IDirectFB *dfb = (IDirectFB *)GFXHandler::GetInstance()->GetGraphicEngine();

		_cairo_surface = cairo_directfb_surface_create(dfb, _surface);
		_cairo_context = cairo_create(_cairo_surface);
	}

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = _scale.width;
	_clip.height = _scale.height;

	Reset();
}

DFBGraphics::~DFBGraphics()
{
	cairo_destroy(_cairo_context);
	cairo_surface_destroy(_cairo_surface);
}

void * DFBGraphics::GetNativeSurface()
{
	return _surface;
}

void DFBGraphics::SetNativeSurface(void *surface)
{
	_surface = (IDirectFBSurface *)surface;

	cairo_destroy(_cairo_context);
	cairo_surface_destroy(_cairo_surface);

	_cairo_context = NULL;
	_cairo_surface = NULL;

	if (_surface != NULL) {
		IDirectFB *dfb = (IDirectFB *)GFXHandler::GetInstance()->GetGraphicEngine();

		_cairo_surface = cairo_directfb_surface_create(dfb, _surface);
		_cairo_context = cairo_create(_cairo_surface);
	}
}

jregion_t DFBGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, _internal_clip.x, _internal_clip.y, _internal_clip.width, _internal_clip.height);
	
	SetClip(clip.x-_translate.x, clip.y-_translate.y, clip.width, clip.height);

	return clip;
}

void DFBGraphics::SetClip(int xp, int yp, int wp, int hp)
{
	int w = _scale.width,
			h = _scale.height;

	if (_surface != NULL) {
		_surface->GetSize(_surface, &w, &h);

		w = SCREEN_TO_SCALE(w, _screen.width, _scale.width);
		h = SCREEN_TO_SCALE(h, _screen.height, _scale.height);
	}

	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, 0, 0, w, h);
	
	_clip.x = clip.x - _translate.x;
	_clip.y = clip.y - _translate.y;
	_clip.width = clip.width;
	_clip.height = clip.height;
	
	_internal_clip = clip;

	if (_surface != NULL) {
		DFBRegion rgn;

		rgn.x1 = SCALE_TO_SCREEN((clip.x), _screen.width, _scale.width);
		rgn.y1 = SCALE_TO_SCREEN((clip.y), _screen.height, _scale.height);
		rgn.x2 = SCALE_TO_SCREEN((clip.x+clip.width+1), _screen.width, _scale.width);
		rgn.y2 = SCALE_TO_SCREEN((clip.y+clip.height+1), _screen.height, _scale.height);

		_surface->SetClip(_surface, NULL);
		_surface->SetClip(_surface, &rgn);
	
		cairo_reset_clip(_cairo_context);
		cairo_rectangle(_cairo_context, rgn.x1, rgn.y1, rgn.x2-rgn.x1, rgn.y2-rgn.y1);
		cairo_clip(_cairo_context);
	}
}

jregion_t DFBGraphics::GetClip()
{
	return _clip;
}

void DFBGraphics::ReleaseClip()
{
	_clip.x = 0;
	_clip.y = 0;
	_clip.width = _scale.width;
	_clip.height = _scale.height;

	_internal_clip.x = _translate.x;
	_internal_clip.y = _translate.y;
	_internal_clip.width = _clip.width-_translate.x;
	_internal_clip.height = _clip.height-_translate.y;

	DFBRegion rgn;

	if (_surface != NULL) {
		_surface->SetClip(_surface, NULL);
		_surface->GetClip(_surface, &rgn);

		_clip.x = 0;
		_clip.y = 0;
		_clip.width = SCREEN_TO_SCALE(rgn.x2, _screen.width, _scale.width);
		_clip.height = SCREEN_TO_SCALE(rgn.y2, _screen.height, _scale.height);
	}
}

void DFBGraphics::SetCompositeFlags(jcomposite_flags_t t)
{
	_composite_flags = t;

	if (_surface != NULL) {
		if (_composite_flags == JCF_NONE) {
			_surface->SetPorterDuff(_surface, DSPD_NONE);
		} else if (_composite_flags == JCF_CLEAR) {
			_surface->SetPorterDuff(_surface, DSPD_CLEAR);
		} else if (_composite_flags == JCF_SRC) {
			_surface->SetPorterDuff(_surface, DSPD_SRC);
		} else if (_composite_flags == JCF_SRC_OVER) {
			_surface->SetPorterDuff(_surface, DSPD_SRC_OVER);
		} else if (_composite_flags == JCF_DST_OVER) {
			_surface->SetPorterDuff(_surface, DSPD_DST_OVER);
		} else if (_composite_flags == JCF_SRC_IN) {
			_surface->SetPorterDuff(_surface, DSPD_SRC_IN);
		} else if (_composite_flags == JCF_DST_IN) {
			_surface->SetPorterDuff(_surface, DSPD_DST_IN);
		} else if (_composite_flags == JCF_SRC_OUT) {
			_surface->SetPorterDuff(_surface, DSPD_SRC_OUT);
		} else if (_composite_flags == JCF_DST_OUT) {
			_surface->SetPorterDuff(_surface, DSPD_DST_OUT);
		} else if (_composite_flags == JCF_SRC_ATOP) {
			_surface->SetPorterDuff(_surface, DSPD_SRC_ATOP);
		} else if (_composite_flags == JCF_DST_ATOP) {
			_surface->SetPorterDuff(_surface, DSPD_DST_ATOP);
		} else if (_composite_flags == JCF_ADD) {
			_surface->SetPorterDuff(_surface, DSPD_ADD);
		} else if (_composite_flags == JCF_XOR) {
			_surface->SetPorterDuff(_surface, DSPD_XOR);
		}
	}
}

void DFBGraphics::SetDrawingFlags(jdrawing_flags_t t)
{
	_draw_flags = t;

	if (_surface != NULL) {
		DFBSurfaceDrawingFlags flags = (DFBSurfaceDrawingFlags)DSDRAW_NOFX;

		if (_draw_flags == JDF_BLEND) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_BLEND);
		} else if (_draw_flags == JDF_XOR) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_XOR);
		}
		
		if (_is_premultiply == true) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_SRC_PREMULTIPLY);
		}

		_surface->SetDrawingFlags(_surface, (DFBSurfaceDrawingFlags)flags);
	}
}

void DFBGraphics::SetBlittingFlags(jblitting_flags_t t)
{
	_blit_flags = t;

	if (_surface != NULL) {
		DFBSurfaceBlittingFlags flags = (DFBSurfaceBlittingFlags)DSBLIT_NOFX;

		if (_blit_flags & JBF_ALPHACHANNEL) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_BLEND_ALPHACHANNEL);
		}

		if (_blit_flags & JBF_COLORALPHA) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_BLEND_COLORALPHA);
		} 

		if (_blit_flags & JBF_COLORIZE) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_COLORIZE);
		}

		if (_blit_flags & JBF_XOR) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_XOR);
		}

		if (_is_premultiply == true) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_SRC_PREMULTIPLY);
		}

		_surface->SetBlittingFlags(_surface, (DFBSurfaceBlittingFlags)flags);
	}
}

jcomposite_flags_t DFBGraphics::GetCompositeFlags()
{
	return _composite_flags;
}

jdrawing_flags_t DFBGraphics::GetDrawingFlags()
{
	return _draw_flags;
}

jblitting_flags_t DFBGraphics::GetBlittingFlags()
{
	return _blit_flags;
}

void DFBGraphics::SetWorkingScreenSize(int width, int height)
{
	if (_image != NULL) {
		jsize_t size = _image->GetSize();

		size.width = (size.width*width)/_scale.width;
		size.height = (size.height*height)/_scale.height;

		_image->SetSize(size.width, size.height);
	}

	_scale.width = width;
	_scale.height = height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}
}

jsize_t DFBGraphics::GetWorkingScreenSize()
{
	return _scale;
}

void DFBGraphics::Clear()
{
	if (_surface == NULL) {
		return;
	}

	_surface->Clear(_surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
}

void DFBGraphics::Clear(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	IDirectFBSurface *sub;
	DFBRectangle rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	_surface->GetSubSurface(_surface, &rect, &sub);

	sub->Clear(sub, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
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

	int x = SCALE_TO_SCREEN(xp, _screen.width, _scale.width),
			y = SCALE_TO_SCREEN(yp, _screen.height, _scale.height),
			w = SCALE_TO_SCREEN(wp, _screen.width, _scale.width),
			h = SCALE_TO_SCREEN(hp, _screen.height, _scale.height);

	DFBRegion rgn;

	rgn.x1 = x;
	rgn.y1 = y;
	rgn.x2 = x+w;
	rgn.y2 = y+h;

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
	if (_surface == NULL) {
		return;
	}

	_color = color;

	int r = _color.GetRed(),
			g = _color.GetGreen(),
			b = _color.GetBlue(),
			a = _color.GetAlpha();

	cairo_set_source_rgba(_cairo_context, r/255.0, g/255.0, b/255.0, a/255.0);
	
	_surface->SetColor(_surface, r, g, b, a);
} 

void DFBGraphics::SetColor(uint32_t color)
{
	SetColor(_color = Color(color));
} 

void DFBGraphics::SetColor(int red, int green, int blue, int alpha)
{
	SetColor(_color = Color(red, green, blue, alpha));
} 

bool DFBGraphics::HasFont()
{
	return (_font != NULL);
}

void DFBGraphics::SetFont(Font *font)
{
	if (_surface == NULL) {
		return;
	}

	_font = font;

	if (_font != NULL) {
		_surface->SetFont(_surface, dynamic_cast<DFBFont *>(font)->_font);
	}
}

Font * DFBGraphics::GetFont()
{
	return _font;
}

void DFBGraphics::SetAntialias(bool b)
{
	if (_surface == NULL) {
		return;
	}

	if (b == false) {
		cairo_set_antialias(_cairo_context, CAIRO_ANTIALIAS_NONE);
	} else {
		cairo_set_antialias(_cairo_context, CAIRO_ANTIALIAS_DEFAULT);
	}
}

void DFBGraphics::SetPixel(int xp, int yp, uint32_t pixel)
{
	SetRGB(pixel, xp+_translate.x, yp+_translate.y);
}

uint32_t DFBGraphics::GetPixel(int xp, int yp)
{
	return GetRGB(xp, yp);
}

void DFBGraphics::SetLineJoin(jline_join_t t)
{
	if (_surface == NULL) {
		return;
	}

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
	if (_surface == NULL) {
		return;
	}

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
	if (_surface == NULL) {
		return;
	}

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
	if (_surface == NULL) {
		return;
	}

	int x0 = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y0 = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int x1 = SCALE_TO_SCREEN((_translate.x+xf), _screen.width, _scale.width); 
	int y1 = SCALE_TO_SCREEN((_translate.y+yf), _screen.height, _scale.height);
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width < 0) {
		line_width = -line_width;
	}

	jregion_t clip = GetClip();
	
	cairo_save(_cairo_context);
	cairo_move_to(_cairo_context, x0, y0);
	cairo_line_to(_cairo_context, x1, y1);
	cairo_set_line_width(_cairo_context, line_width);
	cairo_stroke(_cairo_context);
	cairo_restore(_cairo_context);

	ApplyDrawing();
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
	if (_surface == NULL) {
		return;
	}

	if (_line_width == 0) {
		return;
	}

	if (npoints < 3) {
		return;
	}

	if (interpolation < 2) {
		return;
	}

	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

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
		x[i] = (double)SCALE_TO_SCREEN((_translate.x+p[i].x), _screen.width, _scale.width); 
		y[i] = (double)SCALE_TO_SCREEN((_translate.y+p[i].y), _screen.height, _scale.height);
	}

	x[npoints] = (double)SCALE_TO_SCREEN((_translate.x+p[0].x), _screen.width, _scale.width); 
	y[npoints] = (double)SCALE_TO_SCREEN((_translate.y+p[0].y), _screen.height, _scale.height);

	double t = 0.0;
	
	jregion_t clip = GetClip();
	
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
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	jregion_t clip = GetClip();
	
	cairo_save(_cairo_context);
	cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_fill(_cairo_context);
  cairo_restore(_cairo_context);
	
	SetClip(clip.x, clip.y, clip.width, clip.height);

	/*
	jregion_t clip = GetClip();

	cairo_move_to (_cairo_context, 128.0, 25.6);
	cairo_line_to (_cairo_context, 230.4, 230.4);
	cairo_rel_line_to (_cairo_context, -102.4, 0.0);
	cairo_curve_to (_cairo_context, 51.2, 230.4, 51.2, 128.0, 128.0, 128.0);
	cairo_close_path (_cairo_context);

	cairo_move_to (_cairo_context, 64.0, 25.6);
	cairo_rel_line_to (_cairo_context, 51.2, 51.2);
	cairo_rel_line_to (_cairo_context, -51.2, 51.2);
	cairo_rel_line_to (_cairo_context, -51.2, -51.2);
	cairo_close_path (_cairo_context);

	cairo_set_line_width (_cairo_context, 10.0);
	cairo_set_source_rgb (_cairo_context, 0, 0, 1);
	cairo_fill_preserve (_cairo_context);
	cairo_set_source_rgb (_cairo_context, 0, 0, 0);
	cairo_stroke (_cairo_context);

	SetClip(clip.x, clip.y, clip.width, clip.height);
	*/
}

void DFBGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width > 0) {
		lw = line_width/2;

		x = x - lw;
		y = y - lw;
		w = w + 2*lw;
		h = h + 2*lw;
	} else {
		line_width = -line_width;
		lw = line_width/2;

		x = x + lw;
		y = y + lw;
		w = w - 2*lw;
		h = h - 2*lw;
	}

	jregion_t clip = GetClip();
	
	cairo_save(_cairo_context);
	cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	if (wp <=0 || hp <= 0) {
		return;
	}

	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width); 
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	jregion_t clip = GetClip();

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

	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (wp <=0 || hp <= 0 || lw == 0) {
		return;
	}

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;
	
	if (line_width > 0) {
		lw = line_width/2;

		x = x - lw;
		y = y - lw;
		w = w + 2*lw;
		h = h + 2*lw;
	} else {
		line_width = -line_width;
		lw = line_width/2;

		x = x + lw;
		y = y + lw;
		w = w - 2*lw;
		h = h - 2*lw;
	}

	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width); 
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	jregion_t clip = GetClip();
	
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
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	if (wp <=0 || hp <= 0) {
		return;
	}

	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width); 
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	jregion_t clip = GetClip();
	
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

	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (wp <=0 || hp <= 0 || lw == 0) {
		return;
	}

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;
	
	if (line_width > 0) {
		lw = line_width/2;

		x = x - lw;
		y = y - lw;
		w = w + 2*lw;
		h = h + 2*lw;
	} else {
		line_width = -line_width;
		lw = line_width/2;

		x = x + lw;
		y = y + lw;
		w = w - 2*lw;
		h = h - 2*lw;
	}

	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width); 
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	if (dx > wp/2) {
		dx = wp/2;
	}

	if (dy > hp/2) {
		dy = hp/2;
	}

	jregion_t clip = GetClip();
	
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
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
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
	if (_surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	jregion_t clip = GetClip();

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc(_cairo_context, 0.0, 0.0, 1.0, arc1, arc0);
	cairo_close_path(_cairo_context);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	if (_surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;
	
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

	jregion_t clip = GetClip();

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc(_cairo_context, 0.0, 0.0, 1.0, arc1, arc0);
	cairo_close_path(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);
	
	ApplyDrawing();
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	if (_surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

	arc0 = M_2PI - arc0;
	arc1 = M_2PI - arc1;

	jregion_t clip = GetClip();

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_line_to(_cairo_context, 0, 0);
	cairo_close_path(_cairo_context);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	if (_surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;
	
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

	jregion_t clip = GetClip();

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);
	
	ApplyDrawing();

	SetClip(clip.x, clip.y, clip.width, clip.height);
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
	if (_surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;
	
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

	jregion_t clip = GetClip();

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_line_to(_cairo_context, 0, 0);
	cairo_close_path(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}
		
void DFBGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	if (_surface == NULL) {
		return;
	}

	int x1 = SCALE_TO_SCREEN((_translate.x+x1p), _screen.width, _scale.width); 
	int y1 = SCALE_TO_SCREEN((_translate.y+y1p), _screen.height, _scale.height);
	int x2 = SCALE_TO_SCREEN((_translate.x+x2p), _screen.width, _scale.width); 
	int y2 = SCALE_TO_SCREEN((_translate.y+y2p), _screen.height, _scale.height);
	int x3 = SCALE_TO_SCREEN((_translate.x+x3p), _screen.width, _scale.width); 
	int y3 = SCALE_TO_SCREEN((_translate.y+y3p), _screen.height, _scale.height);

	_surface->FillTriangle(_surface, x1, y1, x2, y2, x3, y3);
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
	if (_surface == NULL) {
		return;
	}

	if (npoints < 1) {
		return;
	}

	int line_width = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (line_width < 0) {
		line_width = -line_width;
	}

	jpoint_t t[npoints];

	for (int i=0; i<npoints; i++) {
		t[i].x = SCALE_TO_SCREEN((xp+p[i].x+_translate.x), _screen.width, _scale.width); 
		t[i].y = SCALE_TO_SCREEN((yp+p[i].y+_translate.y), _screen.height, _scale.height);
	}
	
	jregion_t clip = GetClip();

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
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
	if (_surface == NULL) {
		return;
	}

	if (npoints < 1) {
		return;
	}

	int line_width = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (line_width < 0) {
		line_width = -line_width;
	}

	jpoint_t t[npoints];

	for (int i=0; i<npoints; i++) {
		t[i].x = SCALE_TO_SCREEN((xp+p[i].x+_translate.x), _screen.width, _scale.width); 
		t[i].y = SCALE_TO_SCREEN((yp+p[i].y+_translate.y), _screen.height, _scale.height);
	}
	
	jregion_t clip = GetClip();

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
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
{
	if (_surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+wp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+hp), _screen.height, _scale.height)-yc;
	int x0 = SCALE_TO_SCREEN((_translate.x+xcp+x0p), _screen.width, _scale.width); 
	int y0 = SCALE_TO_SCREEN((_translate.y+ycp+y0p), _screen.height, _scale.height);
	int r0 = SCALE_TO_SCREEN((r0p), _screen.width, _scale.width);

	jregion_t clip = GetClip();

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
	
	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	
	int x1 = SCALE_TO_SCREEN((x1p), _screen.width, _scale.width); 
	int y1 = SCALE_TO_SCREEN((y1p), _screen.height, _scale.height);
	int x2 = SCALE_TO_SCREEN((x2p), _screen.width, _scale.width);
	int y2 = SCALE_TO_SCREEN((y2p), _screen.height, _scale.height);

	jregion_t clip = GetClip();

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
	
	SetDrawingFlags(_draw_flags);
	SetCompositeFlags(_composite_flags);
	SetBlittingFlags(_blit_flags);

	SetClip(clip.x, clip.y, clip.width, clip.height);
}

void DFBGraphics::DrawString(std::string text, int xp, int yp)
{
	if (_surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	_surface->DrawString(_surface, text.c_str(), -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
}

void DFBGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	if (_surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	IDirectFBFont *font = dynamic_cast<DFBFont *>(_font)->_font;
	int advance;

	if (font != NULL) {
		font->GetGlyphExtents(font, symbol, NULL, &advance);
	}

	Image *off = Image::CreateImage(advance, _font->GetAscender() + _font->GetDescender(), JPF_ARGB, _scale.width, _scale.height);

	off->GetGraphics()->SetFont(_font);
	off->GetGraphics()->SetColor(_color);

	IDirectFBSurface *fsurface = (IDirectFBSurface *)(off->GetGraphics()->GetNativeSurface());

	fsurface->DrawGlyph(fsurface, symbol, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	fsurface->DrawGlyph(fsurface, symbol, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

	DrawImage(off, xp, yp);

	delete off;
}

bool DFBGraphics::DrawImage(std::string img, int xp, int yp)
{
	int iwidth,
			iheight;

	if (Image::GetImageSize(img, &iwidth, &iheight) != false) {
		int wp = SCREEN_TO_SCALE((iwidth), _screen.width, _scale.width),
				hp = SCREEN_TO_SCALE((iheight), _screen.height, _scale.height);

		return DFBGraphics::DrawImage(img, xp, yp, wp, hp);
	}

	return false;
}

bool DFBGraphics::DrawImage(std::string img, int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return false;
	}

	if (xp < 0 || yp < 0) {
		return false;
	}

	if (wp < 0 || hp < 0) {
		return false;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	IDirectFBSurface *imgSurface = NULL;
	IDirectFBImageProvider *imgProvider = NULL;
	DFBSurfaceDescription desc;

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	if (engine->CreateImageProvider(engine, img.c_str(), &imgProvider) != DFB_OK) {
		return false;
	}

	if (imgProvider->GetSurfaceDescription (imgProvider, &desc) != DFB_OK) {
		imgProvider->Release(imgProvider);

		return false;
	}

	desc.width = w;
	desc.height = h;

	if (engine->CreateSurface(engine, &desc, &imgSurface) != DFB_OK) {
		imgProvider->Release(imgProvider);

		return false;
	}

	imgSurface->SetPorterDuff(imgSurface, DSPD_NONE);
	imgSurface->SetBlittingFlags(imgSurface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE));

	imgSurface->Clear(imgSurface, 0x00, 0x00, 0x00, 0x00);
	
	if (imgProvider->RenderTo(imgProvider, imgSurface, NULL) != DFB_OK) {
		imgProvider->Release(imgProvider);
		imgSurface->Release(imgSurface);

		return false;
	}

	_surface->Blit(_surface, imgSurface, NULL, x, y);

	imgProvider->Release(imgProvider);
	imgSurface->Release(imgSurface);

	return true;
}

bool DFBGraphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if (_surface == NULL) {
		return false;
	}

	if (sxp < 0 || syp < 0 || swp < 0 || shp < 0) {
		return false;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	IDirectFBSurface *imgSurface = NULL;
	IDirectFBImageProvider *imgProvider = NULL;
	DFBSurfaceDescription desc;

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	if (engine->CreateImageProvider(engine, img.c_str(), &imgProvider) != DFB_OK) {
		return false;
	}

	if (imgProvider->GetSurfaceDescription (imgProvider, &desc) != DFB_OK) {
		imgProvider->Release(imgProvider);

		return false;
	}

	if (engine->CreateSurface(engine, &desc, &imgSurface) != DFB_OK) {
		imgProvider->Release(imgProvider);

		return false;
	}

	imgSurface->SetPorterDuff(imgSurface, DSPD_NONE);
	imgSurface->SetBlittingFlags(imgSurface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));

	imgSurface->Clear(imgSurface, 0x00, 0x00, 0x00, 0x00);
	
	if (imgProvider->RenderTo(imgProvider, imgSurface, NULL) != DFB_OK) {
		imgProvider->Release(imgProvider);
		imgSurface->Release(imgSurface);

		return false;
	}
	
	DFBRectangle srect;

	srect.x = sxp;
	srect.y = syp;
	srect.w = swp;
	srect.h = shp;

	_surface->Blit(_surface, imgSurface, &srect, x, y);

	imgProvider->Release(imgProvider);
	imgSurface->Release(imgSurface);

	return true;
}

bool DFBGraphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return false;
	}

	if (sxp < 0 || syp < 0 || xp < 0 || yp < 0) {
		return false;
	}

	if (swp < 0 || shp < 0 || wp < 0 || hp < 0) {
		return false;
	}

	int sx = sxp, // SCALE_TO_SCREEN(sxp, _screen.width, _scale.width),
			sy = syp, // SCALE_TO_SCREEN(syp, _screen.height, _scale.height),
			sw = swp, // SCALE_TO_SCREEN(swp, _screen.width, _scale.width),
			sh = shp; // SCALE_TO_SCREEN(shp, _screen.height, _scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	IDirectFBSurface *imgSurface = NULL;
	IDirectFBImageProvider *imgProvider = NULL;
	DFBSurfaceDescription desc;

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	if (engine->CreateImageProvider(engine, img.c_str(), &imgProvider) != DFB_OK) {
		return false;
	}

	if (imgProvider->GetSurfaceDescription (imgProvider, &desc) != DFB_OK) {
		imgProvider->Release(imgProvider);

		return false;
	}

	// INFO:: soh para garantir que as imagens seraum as mesmas
	int dw = desc.width,
			dh = desc.height;

	desc.width = (w*dw)/sw;
	desc.height = (h*dh)/sh;

	if (engine->CreateSurface(engine, &desc, &imgSurface) != DFB_OK) {
		imgProvider->Release(imgProvider);

		return false;
	}

	imgSurface->SetPorterDuff(imgSurface, DSPD_NONE);
	imgSurface->SetBlittingFlags(imgSurface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));

	imgSurface->Clear(imgSurface, 0x00, 0x00, 0x00, 0x00);
	
	if (imgProvider->RenderTo(imgProvider, imgSurface, NULL) != DFB_OK) {
		imgProvider->Release(imgProvider);
		imgSurface->Release(imgSurface);

		return false;
	}

	DFBRectangle srect,
							 drect;

	srect.x = (sx*desc.width)/dw;
	srect.y = (sy*desc.height)/dh;
	srect.w = w;
	srect.h = h;

	drect.x = x;
	drect.y = y;
	drect.w = w;
	drect.h = h;

	_surface->StretchBlit(_surface, imgSurface, &srect, &drect);

	imgProvider->Release(imgProvider);
	imgSurface->Release(imgSurface);

	return true;
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
	if (_surface == NULL) {
		return false;
	}

	if ((void *)img == NULL) {
		return false;
	}

	jsize_t scale = img->GetGraphics()->GetWorkingScreenSize();

	int sx = SCALE_TO_SCREEN(sxp, _screen.width, scale.width),
			sy = SCALE_TO_SCREEN(syp, _screen.height, scale.height),
			sw = SCALE_TO_SCREEN(swp, _screen.width, scale.width),
			sh = SCALE_TO_SCREEN(shp, _screen.height, scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	DFBGraphics *g = dynamic_cast<DFBGraphics *>(img->GetGraphics());

	if (g != NULL) {
		DFBRectangle drect;

		drect.x = sx;
		drect.y = sy;
		drect.w = sw;
		drect.h = sh;

		_surface->Blit(_surface, g->_surface, &drect, x, y);
	} else {
		uint32_t *rgb = NULL;

		img->GetRGB(&rgb, sxp, syp, swp, shp);
	
		if (rgb != NULL) {
			SetRGB(rgb, _translate.x+xp, _translate.y+yp, swp, shp, swp);

			delete [] rgb;
		}
	}

	return true;
}

bool DFBGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return false;
	}

	if ((void *)img == NULL) {
		return false;
	}

	jsize_t scale = img->GetGraphics()->GetWorkingScreenSize();

	int sx = SCALE_TO_SCREEN(sxp, _screen.width, scale.width),
			sy = SCALE_TO_SCREEN(syp, _screen.height, scale.height),
			sw = SCALE_TO_SCREEN(swp, _screen.width, scale.width),
			sh = SCALE_TO_SCREEN(shp, _screen.height, scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	DFBGraphics *g = dynamic_cast<DFBGraphics *>(img->GetGraphics());

	if (g != NULL) {
		DFBRectangle srect,
								 drect;

		srect.x = sx;
		srect.y = sy;
		srect.w = sw;
		srect.h = sh;

		drect.x = x;
		drect.y = y;
		drect.w = w;
		drect.h = h;

		_surface->StretchBlit(_surface, g->_surface, &srect, &drect);
	} else {
		jsize_t scale = img->GetGraphics()->GetWorkingScreenSize();

		int iwp = (wp*scale.width)/_screen.width;
		int ihp = (hp*scale.height)/_screen.height;

		Image *image = img->Scale(iwp, ihp);

		if (image != NULL) {
			uint32_t *rgb = NULL;

			image->GetRGB(&rgb, 0, 0, image->GetWidth(), image->GetHeight());

			if (rgb != NULL) {
				SetRGB(rgb, _translate.x+xp, _translate.y+yp, wp, hp, wp);

				delete [] rgb;
			}

			delete image;
		}
	}

	return true;
}

void DFBGraphics::Translate(int x, int y)
{
	_translate.x += x;
	_translate.y += y;
}

jpoint_t DFBGraphics::Translate()
{
	return _translate;
}

void DFBGraphics::GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign)
{
	if (wp < 0 || hp < 0) {
		return;
	}

	jcommon::StringTokenizer token(text, "\n", jcommon::JTT_STRING, false);

	for (int i=0; i<token.GetSize(); i++) {
		std::vector<std::string> words;
		
		std::string line = token.GetToken(i);

		line = jcommon::StringUtils::ReplaceString(line, "\n", "");
		line = jcommon::StringUtils::ReplaceString(line, "\t", "    ");
		
		if (halign == JHA_JUSTIFY) {
			jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, false);

			std::string temp,
				previous;

			for (int j=0; j<line_token.GetSize(); j++) {
				temp = jcommon::StringUtils::Trim(line_token.GetToken(j));

				if (_font->GetStringWidth(temp) > wp) {
					int p = 1;

					while (p < (int)temp.size()) {
						if (_font->GetStringWidth(temp.substr(0, ++p)) > wp) {
							words.push_back(temp.substr(0, p-1));

							temp = temp.substr(p-1);

							p = 1;
						}
					}

					if (temp != "") {
						words.push_back(temp.substr(0, p));
					}
				} else {
					words.push_back(temp);
				}
			}

			temp = words[0];

			for (int j=1; j<(int)words.size(); j++) {
				previous = temp;
				temp += " " + words[j];

				if (_font->GetStringWidth(temp) > wp) {
					temp = words[j];

					lines->push_back(previous);
				}
			}

			lines->push_back("\n" + temp);
		} else {
			jcommon::StringTokenizer line_token(line, " ", jcommon::JTT_STRING, true);

			std::string temp,
				previous;

			for (int j=0; j<line_token.GetSize(); j++) {
				temp = line_token.GetToken(j);

				if (_font->GetStringWidth(temp) > wp) {
					int p = 1;

					while (p < (int)temp.size()) {
						if (_font->GetStringWidth(temp.substr(0, ++p)) > wp) {
							words.push_back(temp.substr(0, p-1));

							temp = temp.substr(p-1);

							p = 1;
						}
					}

					if (temp != "") {
						words.push_back(temp.substr(0, p));
					}
				} else {
					words.push_back(temp);
				}
			}

			temp = words[0];
			
			for (int j=1; j<(int)words.size(); j++) {
				previous = temp;
				temp += words[j];

				if (_font->GetStringWidth(temp.c_str()) > wp) {
					temp = words[j];

					lines->push_back(previous);
				}
			}

			lines->push_back(temp);
		}
	}
}

void DFBGraphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
	if (_surface == NULL) {
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

	max_lines = hp/(font_height+_font->GetLeading());

	if (max_lines <= 0) {
		max_lines = 1;
	}

	GetStringBreak(&lines, text, wp, hp, halign);

	int line_space = _font->GetLeading(),
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

uint32_t DFBGraphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	if (_surface == NULL) {
		return pixel;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	int swmax,
			shmax;

	_surface->GetSize(_surface, &swmax, &shmax);

	if ((x < 0 || x >= swmax) || (y < 0 || y >= shmax)) {
		return pixel;
	}

	void *ptr;
	uint32_t *dst,
					 rgb;
	int pitch;

	_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

	dst = (uint32_t *)((uint8_t *)ptr + y * pitch);
	rgb = *(dst + x);

	_surface->Unlock(_surface);

	return rgb;
}

void DFBGraphics::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp, int scansize)
{
	if (_surface == NULL) {
		return;
	}

	int startx = SCALE_TO_SCREEN(xp, _screen.width, _scale.width); 
	int starty = SCALE_TO_SCREEN(yp, _screen.height, _scale.height);
	// int w = SCALE_TO_SCREEN(wp, _screen.width, _scale.width);
	// int h = SCALE_TO_SCREEN(hp, _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((xp+wp), _screen.width, _scale.width)-startx;
	int h = SCALE_TO_SCREEN((yp+hp), _screen.height, _scale.height)-starty;

	void *ptr;
	uint32_t *src,
					 *dst;
	int x,
			y,
			pitch;
	uint32_t *array = (*rgb);

	if (*rgb == NULL) {
		array = new uint32_t[wp*hp];
	}

	int img_w,
			img_h;
	int max_w = startx+w,
			max_h = starty+h;

	_surface->GetSize(_surface, &img_w, &img_h);

	if (max_w > img_w || max_h > img_h) {
		if ((*rgb) == NULL) {
			delete [] array;
		}

		(*rgb) = NULL;

		return;
	}

	_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

	double scale_x = (double)_screen.width/(double)_scale.width,
				 scale_y = (double)_screen.height/(double)_scale.height;

	for (y=0; y<hp; y++) {
		src = (uint32_t *)(array + y * scansize);
		dst = (uint32_t *)((uint8_t *)ptr + ((int)((yp + y) * scale_y)) * pitch);
		
		for (x=0; x<wp; x++) {
			*(src + x) = *(dst + (int)((xp + x) * scale_x));
		}
	}

	_surface->Unlock(_surface);

	(*rgb) = array;
}

void DFBGraphics::SetRGB(uint32_t argb, int xp, int yp) 
{
	if (_surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((yp), _screen.height, _scale.height);

	int r = (argb >> 0x10) & 0xff,
			g = (argb >> 0x08) & 0xff,
			b = (argb >> 0x00) & 0xff,
			a = (argb >> 0x18) & 0xff;

	_surface->SetColor(_surface, r, g, b, a);
	_surface->DrawLine(_surface, x, y, x, y);
}

void DFBGraphics::SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp, int scanline) 
{
	if (_surface == NULL) {
		return;
	}

	void *ptr;
	uint32_t *dst,
					 *src = rgb;
	int wmax,
			hmax,
			pitch;

	int x = SCALE_TO_SCREEN((xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((yp+hp), _screen.height, _scale.height)-y;

	_surface->GetSize(_surface, &wmax, &hmax);

	if (x > wmax || y > hmax) {
		return;
	}

	if (x+w > wmax) {
		w = wmax-x;
	}
	
	if (y+h > hmax) {
		h = hmax-y;
	}

	wmax = x+w;
	hmax = y+h;

	_surface->Lock(_surface, DSLF_WRITE, &ptr, &pitch);

	double scale_x = (double)_scale.width/(double)_screen.width,
				 scale_y = (double)_scale.height/(double)_screen.height;

	if (_draw_flags == JDF_NOFX) {
		for (int j=0; j<h; j++) {
			dst = (uint32_t *)((uint8_t *)ptr+(y+j)*pitch)+x;
			src = (uint32_t *)(rgb+(int)(j*scale_y)*scanline);

			double k = 0;
			int last = -1;

			for (int i=0; i<w; i++) {
				if (last != (int)k) {
					uint32_t pixel = *(src+(int)k);

					if (_is_premultiply == true) {
						uint32_t pa = (pixel >> 0x18) + 1;

						pixel = ((((pixel & 0x00ff00ff) * pa) >> 8) & 0x00ff00ff) | ((((pixel & 0x0000ff00) * pa) >> 8) & 0x0000ff00) | ((((pixel & 0xff000000))));
					}
					
					*dst++ = pixel;
					
					last = (int)k;
				}
				
				k = k + scale_x;
			}
		}
	} else if (_draw_flags == JDF_BLEND) {
		for (int j=0; j<h; j++) {
			dst = (uint32_t *)((uint8_t *)ptr+(y+j)*pitch);
			src = (uint32_t *)(rgb+(int)(j*scale_y)*scanline);

			for (int i=0; i<w; i++) {
				int argb = *(src+(int)(i*scale_x)),
						pixel = *(dst+x+i),
						r = (argb >> 0x10) & 0xff,
						g = (argb >> 0x08) & 0xff,
						b = (argb >> 0x00) & 0xff,
						a = (argb >> 0x18) & 0xff,
						pr = (pixel >> 0x10) & 0xff,
						pg = (pixel >> 0x08) & 0xff,
						pb = (pixel >> 0x00) & 0xff;

				pr = (int)(pr*(0xff-a) + r*a) >> 0x08;
				pg = (int)(pg*(0xff-a) + g*a) >> 0x08;
				pb = (int)(pb*(0xff-a) + b*a) >> 0x08;

				*(dst+x+i) = 0xff000000 | (pr << 0x10) | (pg << 0x08) | (pb << 0x00);
			}
		}
	} else if (_draw_flags == JDF_XOR) {
		for (int j=0; j<h; j++) {
			dst = (uint32_t *)((uint8_t *)ptr+(y+j)*pitch)+x;
			src = (uint32_t *)(rgb+(int)(j*scale_y)*scanline);

			double k = 0;
			int last = -1;

			for (int i=0; i<w; i++) {
				if (last != (int)k) {
					uint32_t pixel = *(src+(int)k);

					if (_is_premultiply == true) {
						uint32_t pa = (pixel >> 0x18) + 1;

						pixel = ((((pixel & 0x00ff00ff) * pa) >> 8) & 0x00ff00ff) | ((((pixel & 0x0000ff00) * pa) >> 8) & 0x0000ff00) | ((((pixel & 0xff000000))));
					}
					
					*dst++ ^= pixel;
					
					last = (int)k;
				}

				k = k + scale_x;
			}
		}
	}

	_surface->Unlock(_surface);


	/* INFO:: uses DrawLine
	if (_surface == NULL) {
		return;
	}

	double 
		scale_x = (double)_scale.width/(double)_screen.width,
		scale_y = (double)_scale.height/(double)_screen.height;
	double step;
	int 
		x = SCALE_TO_SCREEN((xp), _screen.width, _scale.width),
		y = SCALE_TO_SCREEN((yp), _screen.height, _scale.height),
		w = SCALE_TO_SCREEN((xp+wp), _screen.width, _scale.width)-x,
		h = SCALE_TO_SCREEN((yp+hp), _screen.height, _scale.height)-y;
	int 
		wmax,
		hmax;
	int
		i,
		j,
		index,
		pixel,
		lastp;

	_surface->GetSize(_surface, &wmax, &hmax);

	if (x > wmax || y > hmax) {
		return;
	}

	if (x+w > wmax) {
		w = wmax-x;
	}
	
	if (y+h > hmax) {
		h = hmax-y;
	}

	wmax = x+w;
	hmax = y+h;

	uint8_t *ptr = (uint8_t *)rgb;

	lastp = pixel = *rgb;
	
	_surface->SetColor(_surface, ptr[0], ptr[1], ptr[2], ptr[3]);

	for (j=y; j<h; j++) {
		index = (int)(j*scale_y)*scanline;
		step = 0.0;

		for (i=x; i<w; i++) {
			step = step + scale_x;
			pixel = *(rgb+index+(int)step);

			if (pixel != lastp) {
				uint8_t *ptr = (uint8_t *)&pixel;

				_surface->SetColor(_surface, ptr[2], ptr[1], ptr[0], ptr[3]);
			}

			lastp = pixel;

			_surface->DrawLine(_surface, i, j, i, j);
		}
	}
	*/
}

void DFBGraphics::Reset()
{
	SetAntialias(true);

	SetColor(0x00000000);

	SetLineWidth(1);
	SetLineJoin(JLJ_MITER);
	SetLineStyle(JLS_BUTT);
	SetLineDash(NULL, 0);

	ResetGradientStop();
	SetDrawingFlags(JDF_BLEND);
	SetBlittingFlags(JBF_ALPHACHANNEL);
	SetCompositeFlags(JCF_SRC_OVER);
	SetDrawingMode(JDM_STROKE);
}

void DFBGraphics::ApplyDrawing()
{
	/* TODO::
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
