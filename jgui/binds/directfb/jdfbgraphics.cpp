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

#define M_2PI	(2*M_PI)

namespace jgui {

DFBGraphics::DFBGraphics(void *surface, bool premultiplied):
	jgui::Graphics()
{
	jcommon::Object::SetClassName("jgui::DFBGraphics");

	_is_premultiply = premultiplied;

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;

	IDirectFB *dfb = (IDirectFB *)GFXHandler::GetInstance()->GetGraphicEngine();

	this->surface = (IDirectFBSurface *)surface;

	_cairo_surface = cairo_directfb_surface_create(dfb, this->surface);
	_cairo_context = cairo_create(_cairo_surface);

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = DEFAULT_SCALE_WIDTH;
	_clip.height = DEFAULT_SCALE_HEIGHT;

	_line_join = JLJ_BEVEL;
	_line_style = JLS_BUTT;
	_line_width = 1;

	SetDrawingFlags(JDF_BLEND);
	SetBlittingFlags(JBF_ALPHACHANNEL);
	SetPorterDuffFlags(JPF_SRC_OVER);
}

DFBGraphics::~DFBGraphics()
{
}

void * DFBGraphics::GetNativeSurface()
{
	return surface;
}

void DFBGraphics::SetNativeSurface(void *addr)
{
	_graphics_mutex.Lock();

	surface = (IDirectFBSurface *)addr;

	_graphics_mutex.Unlock();
}

jregion_t DFBGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, _internal_clip.x, _internal_clip.y, _internal_clip.width, _internal_clip.height);
	
	SetClip(clip.x-_translate.x, clip.y-_translate.y, clip.width, clip.height);

	return clip;
}

void DFBGraphics::SetClip(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, 0, 0, _scale.width, _scale.height);
	
	_clip.x = clip.x - _translate.x;
	_clip.y = clip.y - _translate.y;
	_clip.width = clip.width;
	_clip.height = clip.height;
	
	_internal_clip = clip;

	if (surface != NULL) {
		DFBRegion rgn;

		rgn.x1 = SCALE_TO_SCREEN((clip.x), _screen.width, _scale.width);
		rgn.y1 = SCALE_TO_SCREEN((clip.y), _screen.height, _scale.height);
		rgn.x2 = SCALE_TO_SCREEN((clip.x+clip.width), _screen.width, _scale.width);
		rgn.y2 = SCALE_TO_SCREEN((clip.y+clip.height), _screen.height, _scale.height);

		surface->SetClip(surface, NULL);
		surface->SetClip(surface, &rgn);
	
		cairo_reset_clip(_cairo_context);
		cairo_rectangle(_cairo_context, rgn.x1, rgn.y1, rgn.x2-rgn.x1+1, rgn.y2-rgn.y1+1);
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
	_internal_clip.width = _clip.width;
	_internal_clip.height = _clip.height;

	DFBRegion rgn;

	if (surface != NULL) {
		surface->SetClip(surface, NULL);
		surface->GetClip(surface, &rgn);

		_clip.x = 0;
		_clip.y = 0;
		_clip.width = SCREEN_TO_SCALE(rgn.x2, _screen.width, _scale.width);
		_clip.height = SCREEN_TO_SCALE(rgn.y2, _screen.height, _scale.height);
	}
}

void DFBGraphics::SetPorterDuffFlags(jporterduff_flags_t t)
{
	_porterduff_flags = t;

	if (surface != NULL) {
		if (_porterduff_flags == JPF_NONE) {
			surface->SetPorterDuff(surface, DSPD_NONE);
		} else if (_porterduff_flags == JPF_CLEAR) {
			surface->SetPorterDuff(surface, DSPD_CLEAR);
		} else if (_porterduff_flags == JPF_SRC) {
			surface->SetPorterDuff(surface, DSPD_SRC);
		} else if (_porterduff_flags == JPF_SRC_OVER) {
			surface->SetPorterDuff(surface, DSPD_SRC_OVER);
		} else if (_porterduff_flags == JPF_DST_OVER) {
			surface->SetPorterDuff(surface, DSPD_DST_OVER);
		} else if (_porterduff_flags == JPF_SRC_IN) {
			surface->SetPorterDuff(surface, DSPD_SRC_IN);
		} else if (_porterduff_flags == JPF_DST_IN) {
			surface->SetPorterDuff(surface, DSPD_DST_IN);
		} else if (_porterduff_flags == JPF_SRC_OUT) {
			surface->SetPorterDuff(surface, DSPD_SRC_OUT);
		} else if (_porterduff_flags == JPF_DST_OUT) {
			surface->SetPorterDuff(surface, DSPD_DST_OUT);
		} else if (_porterduff_flags == JPF_SRC_ATOP) {
			surface->SetPorterDuff(surface, DSPD_SRC_ATOP);
		} else if (_porterduff_flags == JPF_DST_ATOP) {
			surface->SetPorterDuff(surface, DSPD_DST_ATOP);
		} else if (_porterduff_flags == JPF_ADD) {
			surface->SetPorterDuff(surface, DSPD_ADD);
		} else if (_porterduff_flags == JPF_XOR) {
			surface->SetPorterDuff(surface, DSPD_XOR);
		}
	}
}

void DFBGraphics::SetDrawingFlags(jdrawing_flags_t t)
{
	_draw_flags = t;

	if (surface != NULL) {
		DFBSurfaceDrawingFlags flags = (DFBSurfaceDrawingFlags)DSDRAW_NOFX;

		if (_draw_flags == JDF_BLEND) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_BLEND);
		} else if (_draw_flags == JDF_XOR) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_XOR);
		}
		
		if (_is_premultiply == false) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_SRC_PREMULTIPLY);
		}

		surface->SetDrawingFlags(surface, (DFBSurfaceDrawingFlags)flags);
	}
}

void DFBGraphics::SetBlittingFlags(jblitting_flags_t t)
{
	_blit_flags = t;

	if (surface != NULL) {
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

		if (_is_premultiply == false) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_SRC_PREMULTIPLY);
		}

		surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)flags);
	}
}

jporterduff_flags_t DFBGraphics::GetPorterDuffFlags()
{
	return _porterduff_flags;
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

void DFBGraphics::Clear(int red, int green, int blue, int alpha)
{
	if (surface == NULL) {
		return;
	}

	surface->Clear(surface, red, green, blue, alpha);
}

void DFBGraphics::Idle()
{
	IDirectFB *engine = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

	engine->WaitIdle(engine);
	engine->WaitForSync(engine);
}

void DFBGraphics::Flip()
{
	if (surface == NULL) {
		return;
	}

	surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	// surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
}

void DFBGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (surface == NULL) {
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

	surface->Flip(surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	// surface->Flip(surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
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
	
	surface->SetColor(surface, r, g, b, a);
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
	_font = font;

	if (_font != NULL) {
		surface->SetFont(surface, dynamic_cast<DFBFont *>(font)->_font);
	}
}

Font * DFBGraphics::GetFont()
{
	return _font;
}

void DFBGraphics::SetAntialias(bool b)
{
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
	int x0 = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y0 = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int x1 = SCALE_TO_SCREEN((_translate.x+xf), _screen.width, _scale.width); 
	int y1 = SCALE_TO_SCREEN((_translate.y+yf), _screen.height, _scale.height);
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width < 0) {
		line_width = -line_width;
	}

	// CHANGE:: comment the first 'if' for increase speed
	if (line_width == 1 && cairo_get_antialias(_cairo_context) == CAIRO_ANTIALIAS_NONE) {
		surface->DrawLine(surface, x0, y0, x1, y1);
	} else {
		cairo_save(_cairo_context);
		cairo_move_to(_cairo_context, x0, y0);
		cairo_line_to(_cairo_context, x1, y1);
		cairo_restore(_cairo_context);
		cairo_set_line_width(_cairo_context, line_width);

		// applyDrawingMode(mode);
		ApplyDrawing();
	}
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

	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width < 0) {
		line_width = -line_width;
	}

	double *x, 
				 *y, 
				 stepsize;
	int x1, 
			y1, 
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
	
	x1 = lrint(EvaluateBezier0(x, npoints+1, t));
	y1 = lrint(EvaluateBezier0(y, npoints+1, t));
	
	cairo_save(_cairo_context);
  cairo_new_sub_path(_cairo_context);

	for (int i=0; i<=(npoints*interpolation); i++) {
		t = t + stepsize;

		x2 = EvaluateBezier0(x, npoints, t);
		y2 = EvaluateBezier0(y, npoints, t);
	
		cairo_line_to(_cairo_context, x2, y2);
		
		x1 = x2;
		y1 = y2;
	}
    
  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();

	delete [] x;
	delete [] y;
}

void DFBGraphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	cairo_save(_cairo_context);
	cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_restore(_cairo_context);
  cairo_fill(_cairo_context);
}

void DFBGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
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

	cairo_save(_cairo_context);
	cairo_rectangle(_cairo_context, x, y, w, h);
  cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
}

void DFBGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
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
}

void DFBGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
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
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

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
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

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

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);
	
	// applyPen();
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

	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc_negative(_cairo_context, 0.0, 0.0, 1.0, arc0, arc1);
	cairo_line_to(_cairo_context, 0, 0);
	cairo_close_path(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_set_line_width(_cairo_context, line_width);

	ApplyDrawing();
}
		
void DFBGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	if (surface == NULL) {
		return;
	}

	int x1 = SCALE_TO_SCREEN((_translate.x+x1p), _screen.width, _scale.width); 
	int y1 = SCALE_TO_SCREEN((_translate.y+y1p), _screen.height, _scale.height);
	int x2 = SCALE_TO_SCREEN((_translate.x+x2p), _screen.width, _scale.width); 
	int y2 = SCALE_TO_SCREEN((_translate.y+y2p), _screen.height, _scale.height);
	int x3 = SCALE_TO_SCREEN((_translate.x+x3p), _screen.width, _scale.width); 
	int y3 = SCALE_TO_SCREEN((_translate.y+y3p), _screen.height, _scale.height);

	surface->FillTriangle(surface, x1, y1, x2, y2, x3, y3);
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

	int line_width = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (line_width < 0) {
		line_width = -line_width;
	}

	jpoint_t t[npoints];

	for (int i=0; i<npoints; i++) {
		t[i].x = SCALE_TO_SCREEN((xp+p[i].x+_translate.x), _screen.width, _scale.width); 
		t[i].y = SCALE_TO_SCREEN((yp+p[i].y+_translate.y), _screen.height, _scale.height);
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

	int line_width = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (line_width < 0) {
		line_width = -line_width;
	}

	jpoint_t t[npoints];

	for (int i=0; i<npoints; i++) {
		t[i].x = SCALE_TO_SCREEN((xp+p[i].x+_translate.x), _screen.width, _scale.width); 
		t[i].y = SCALE_TO_SCREEN((yp+p[i].y+_translate.y), _screen.height, _scale.height);
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

void DFBGraphics::FillRadialGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	int xc = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-yc;

	int sr = scolor.GetRed(),
			sg = scolor.GetGreen(),
			sb = scolor.GetBlue(),
			sa = scolor.GetAlpha();
	int dr = dcolor.GetRed(),
			dg = dcolor.GetGreen(),
			db = dcolor.GetBlue(),
			da = dcolor.GetAlpha();

	cairo_pattern_t *pattern = cairo_pattern_create_radial(xc, yc, std::max(rx, ry), xc, yc, 0.0);

	cairo_pattern_add_color_stop_rgba(pattern, 0.0, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	cairo_pattern_add_color_stop_rgba(pattern, 1.0, dr/255.0, dg/255.0, db/255.0, da/255.0);
	
	cairo_set_source(_cairo_context, pattern);
	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, xc, yc);
	cairo_scale(_cairo_context, rx, ry);
	cairo_arc(_cairo_context, 0.0, 0.0, 1.0, 0.0, M_2PI);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	cairo_pattern_destroy(pattern);
}

void DFBGraphics::FillHorizontalGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	int sr = scolor.GetRed(),
			sg = scolor.GetGreen(),
			sb = scolor.GetBlue(),
			sa = scolor.GetAlpha();
	int dr = dcolor.GetRed(),
			dg = dcolor.GetGreen(),
			db = dcolor.GetBlue(),
			da = dcolor.GetAlpha();

	cairo_pattern_t *pattern = cairo_pattern_create_linear(0.0, 0.0, w, 0.0);
	
	cairo_pattern_add_color_stop_rgba(pattern, 0.0, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	cairo_pattern_add_color_stop_rgba(pattern, 1.0, dr/255.0, dg/255.0, db/255.0, da/255.0);
	
	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, x, y);
	cairo_rectangle(_cairo_context, 0, 0, w, h);
	cairo_set_source(_cairo_context, pattern);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	
	cairo_pattern_destroy(pattern);
}

void DFBGraphics::FillVerticalGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	int sr = scolor.GetRed(),
			sg = scolor.GetGreen(),
			sb = scolor.GetBlue(),
			sa = scolor.GetAlpha();
	int dr = dcolor.GetRed(),
			dg = dcolor.GetGreen(),
			db = dcolor.GetBlue(),
			da = dcolor.GetAlpha();

	cairo_pattern_t *pattern = cairo_pattern_create_linear(0.0, 0.0, 0.0, h);
	
	cairo_pattern_add_color_stop_rgba(pattern, 0.0, sr/255.0, sg/255.0, sb/255.0, sa/255.0);
	cairo_pattern_add_color_stop_rgba(pattern, 1.0, dr/255.0, dg/255.0, db/255.0, da/255.0);
	
	cairo_save(_cairo_context);
	cairo_translate(_cairo_context, x, y);
	cairo_rectangle(_cairo_context, 0, 0, w, h);
	cairo_set_source(_cairo_context, pattern);
	cairo_fill(_cairo_context);
	cairo_restore(_cairo_context);
	
	cairo_pattern_destroy(pattern);
}

void DFBGraphics::DrawString(std::string text, int xp, int yp)
{
	if (surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	Image *off = Image::CreateImage(_font->GetStringWidth(text), _font->GetAscender() + _font->GetDescender(), JSP_ARGB, _scale.width, _scale.height);

	off->GetGraphics()->SetFont(_font);
	off->GetGraphics()->SetColor(_color);

	IDirectFBSurface *fsurface = (IDirectFBSurface *)(off->GetGraphics()->GetNativeSurface());

	fsurface->DrawString(fsurface, text.c_str(), -1, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	fsurface->DrawString(fsurface, text.c_str(), -1, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

	DrawImage(off, xp, yp);

	delete off;
}

void DFBGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	if (surface == NULL) {
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

	Image *off = Image::CreateImage(advance, _font->GetAscender() + _font->GetDescender(), JSP_ARGB, _scale.width, _scale.height);

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
	if (xp < 0 || yp < 0) {
		return false;
	}

	if (wp < 0 || hp < 0) {
		return false;
	}

	if (surface == NULL) {
		return false;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	if (_radians != 0.0) {
		Image *off = Image::CreateImage(wp, hp);
		Graphics *g = off->GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, 0, 0, wp, hp);
		
		RotateImage0(off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		delete off;

		return true;
	}

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
	imgSurface->SetBlittingFlags(imgSurface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));

	imgSurface->Clear(imgSurface, 0x00, 0x00, 0x00, 0x00);
	
	if (imgProvider->RenderTo(imgProvider, imgSurface, NULL) != DFB_OK) {
		imgProvider->Release(imgProvider);
		imgSurface->Release(imgSurface);

		return false;
	}

	surface->Blit(surface, imgSurface, NULL, x, y);

	imgProvider->Release(imgProvider);
	imgSurface->Release(imgSurface);

	return true;
}

bool DFBGraphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if (sxp < 0 || syp < 0 || swp < 0 || shp < 0) {
		return false;
	}

	if (surface == NULL) {
		return false;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	if (_radians != 0.0) {
		int wp = SCREEN_TO_SCALE((swp), _screen.width, _scale.width),
				hp = SCREEN_TO_SCALE((shp), _screen.height, _scale.height);

		Image *off = Image::CreateImage(wp, hp);
		Graphics *g = off->GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0);
		
		RotateImage0(off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		delete off;

		return true;
	}

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

	surface->Blit(surface, imgSurface, &srect, x, y);

	imgProvider->Release(imgProvider);
	imgSurface->Release(imgSurface);

	return true;
}

bool DFBGraphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	if (sxp < 0 || syp < 0 || xp < 0 || yp < 0) {
		return false;
	}

	if (swp < 0 || shp < 0 || wp < 0 || hp < 0) {
		return false;
	}

	if (surface == NULL) {
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

	if (_radians != 0.0) {
		Image *off = Image::CreateImage(wp, hp);
		Graphics *g = off->GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0, wp, hp);
		
		RotateImage0(off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		delete off;

		return true;
	}

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

	surface->StretchBlit(surface, imgSurface, &srect, &drect);

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
	if ((void *)surface == NULL) {
		return false;
	}

	if ((void *)img == NULL) {
		return false;
	}

	jsize_t scale = img->GetWorkingScreenSize();

	int sx = SCALE_TO_SCREEN(sxp, _screen.width, scale.width),
			sy = SCALE_TO_SCREEN(syp, _screen.height, scale.height),
			sw = SCALE_TO_SCREEN(swp, _screen.width, scale.width),
			sh = SCALE_TO_SCREEN(shp, _screen.height, scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	if (_radians != 0.0) {
		Image *off = Image::CreateImage(img->GetWidth(), img->GetHeight());
		Graphics *g = off->GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0);

		RotateImage0(off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, img->GetWidth(), img->GetHeight(), _radians, _color.GetAlpha());

		delete off;

		return true;
	}

	Graphics *g = img->GetGraphics();

	if ((void *)g != NULL) {
		DFBRectangle drect;

		drect.x = sx;
		drect.y = sy;
		drect.w = sw;
		drect.h = sh;

		surface->Blit(surface, dynamic_cast<jgui::DFBGraphics *>(g)->surface, &drect, x, y);
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
	if ((void *)surface == NULL) {
		return false;
	}

	if ((void *)img == NULL) {
		return false;
	}

	jsize_t scale = img->GetWorkingScreenSize();

	int sx = SCALE_TO_SCREEN(sxp, _screen.width, scale.width),
			sy = SCALE_TO_SCREEN(syp, _screen.height, scale.height),
			sw = SCALE_TO_SCREEN(swp, _screen.width, scale.width),
			sh = SCALE_TO_SCREEN(shp, _screen.height, scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	if (_radians != 0.0) {
		Image *off = Image::CreateImage(wp, hp);
		Graphics *g = off->GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0, wp, hp);

		RotateImage0(off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		delete off;

		return true;
	}

	Graphics *g = img->GetGraphics();

	if ((void *)g != NULL) {
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

		surface->StretchBlit(surface, dynamic_cast<jgui::DFBGraphics *>(g)->surface, &srect, &drect);
	} else {
		jsize_t scale = img->GetWorkingScreenSize();

		int iwp = (wp*scale.width)/_screen.width;
		int ihp = (hp*scale.height)/_screen.height;

		Image *image = img->Scaled(iwp, ihp);

		uint32_t *rgb = NULL;

		image->GetRGB(&rgb, 0, 0, image->GetWidth(), image->GetHeight());
		
		if (rgb != NULL) {
			SetRGB(rgb, _translate.x+xp, _translate.y+yp, wp, hp, wp);

			delete [] rgb;
		}

		delete image;
	}

	return true;
}

void DFBGraphics::Rotate(double radians)
{
	_radians = fmod(radians, 2*M_PI);
}

void DFBGraphics::Translate(int x, int y)
{
	_translate.x += x;
	_translate.y += y;
}

void DFBGraphics::TranslateImage(int x, int y)
{
	_translate_image.x = x;
	_translate_image.y = y;
}

double DFBGraphics::Rotate()
{
	return _radians;
}

jpoint_t DFBGraphics::Translate()
{
	return _translate;
}

jpoint_t DFBGraphics::TranslateImage()
{
	return _translate_image;
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
	if (wp < 0 || hp < 0) {
		return;
	}

	if (surface == NULL) {
		return;
	}

	std::vector<std::string> words,
		lines;
	int dx = 0,
			dy = 0,
			max_lines,
			font_height;
	
	font_height = _font->GetAscender() + _font->GetDescender();

	if (font_height <= 0) {
		return;
	}

	max_lines = hp/font_height;

	if (max_lines <= 0) {
		max_lines = 1;
	}

	GetStringBreak(&lines, text, wp, hp, halign);

	int line_space = 0,
			line_yinit = 0,
			line_ydiff = 0;

	if (hp > (int)lines.size()*font_height) {
		int nlines = (int)lines.size();

		if (valign == JVA_TOP) {
			line_space = 0;
			line_yinit = 0;
			line_ydiff = 0;
		} else if (valign == JVA_CENTER) {
			line_space = 0;
			line_yinit = (hp-nlines*font_height)/2;
			line_ydiff = 0;
		} else if (valign == JVA_BOTTOM) {
			line_space = 0;
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
	if (surface == NULL) {
		return pixel;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	int swmax,
			shmax;

	surface->GetSize(surface, &swmax, &shmax);

	if ((x < 0 || x >= swmax) || (y < 0 || y >= shmax)) {
		return pixel;
	}

	void *ptr;
	uint32_t *dst,
					 rgb;
	int pitch;

	surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

	dst = (uint32_t *)((uint8_t *)ptr + y * pitch);
	rgb = *(dst + x);

	surface->Unlock(surface);

	return rgb;
}

void DFBGraphics::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp, int scansize)
{
	if (surface == NULL) {
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

	surface->GetSize(surface, &img_w, &img_h);

	if (max_w > img_w || max_h > img_h) {
		(*rgb) = NULL;

		return;
	}

	surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

	double scale_x = (double)_screen.width/(double)_scale.width,
				 scale_y = (double)_screen.height/(double)_scale.height;

	for (y=0; y<hp; y++) {
		src = (uint32_t *)(array + y * scansize);
		dst = (uint32_t *)((uint8_t *)ptr + ((int)((yp + y) * scale_y)) * pitch);
		
		for (x=0; x<wp; x++) {
			*(src + x) = *(dst + (int)((xp + x) * scale_x));
		}
	}

	surface->Unlock(surface);

	(*rgb) = array;
}

void DFBGraphics::SetRGB(uint32_t argb, int xp, int yp) 
{
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((yp), _screen.height, _scale.height);

	int r = (argb >> 0x10) & 0xff,
			g = (argb >> 0x08) & 0xff,
			b = (argb >> 0x00) & 0xff,
			a = (argb >> 0x18) & 0xff;

	surface->SetColor(surface, r, g, b, a);
	surface->DrawLine(surface, x, y, x, y);
}

void DFBGraphics::SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp, int scanline) 
{
	if (surface == NULL) {
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

	surface->GetSize(surface, &wmax, &hmax);

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

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	double scale_x = (double)_scale.width/(double)_screen.width,
				 scale_y = (double)_scale.height/(double)_screen.height;

	if (_draw_flags == JDF_NOFX) {
		for (int j=0; j<h; j++) {
			dst = (uint32_t *)((uint8_t *)ptr+(y+j)*pitch);
			src = (uint32_t *)(rgb+(int)(j*scale_y)*scanline);

			for (int i=0; i<w; i++) {
				*(dst+x+i) = *(src+(int)(i*scale_x));
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
			dst = (uint32_t *)((uint8_t *)ptr+(y+j)*pitch);
			src = (uint32_t *)(rgb+(int)(j*scale_y)*scanline);

			for (int i=0; i<w; i++) {
				*(dst+x+i) ^= *(src+(int)(i*scale_x));
			}
		}
	}

	surface->Unlock(surface);
}

void DFBGraphics::Reset()
{
	_radians = 0.0;

	_translate_image.x = 0;
	_translate_image.y = 0;

	SetAntialias(true);

	SetColor(0x00000000);

	SetLineWidth(1);
	SetLineJoin(JLJ_MITER);
	SetLineStyle(JLS_BUTT);
	SetLineDash(NULL, 0);

	SetDrawingFlags(JDF_BLEND);
	SetBlittingFlags(JBF_ALPHACHANNEL);
	SetPorterDuffFlags(JPF_SRC_OVER);
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

void DFBGraphics::RotateImage0(Image *img, int xcp, int ycp, int xp, int yp, int wp, int hp, double angle, uint8_t alpha)
{
	int xc = SCALE_TO_SCREEN((xcp), _screen.width, _scale.width),
			yc = SCALE_TO_SCREEN((ycp), _screen.height, _scale.height);
	int x = SCALE_TO_SCREEN((xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((yp), _screen.height, _scale.height),
			width = SCALE_TO_SCREEN((xp+wp), _screen.width, _scale.width)-x,
			height = SCALE_TO_SCREEN((yp+hp), _screen.height, _scale.height)-y;

	Graphics *gimg = img->GetGraphics();
	int cosTheta,
			sinTheta,
			i,
			j,
			iOriginal,
			iPrime,
			jPrime,
			jOriginal;
	int precision = 1024;

	angle = angle;

	sinTheta = precision*sin(angle);
	cosTheta = precision*cos(angle);

	int dw = width,
			dh = height,
			size = width;

	if (xc == 0 && yc == 0) {
		if (height > width) {
			dw = height;
			dh = width;
		}

		dw = dw+width/2-width+jmath::Math<int>::Abs(xc);
		dh = dh+height/2-height+jmath::Math<int>::Abs(yc);
	} else {
		if (height > width) {
			size = height;
		}

		dw = 2*(size+jmath::Math<int>::Abs(xc)+jmath::Math<int>::Abs(yc));
		dh = 2*(size+jmath::Math<int>::Abs(xc)+jmath::Math<int>::Abs(yc));
	}

	xc = xc + width/2;
	yc = yc + height/2;

	x = x;
	y = y;

	IDirectFBSurface *simg = (IDirectFBSurface *)gimg->GetNativeSurface();
	void *sptr;
	uint32_t *sdst;
	int spitch;
	void *gptr;
	int gpitch;
	int swmax,
			shmax;
	int iwmax,
			ihmax;

	surface->GetSize(surface, &swmax, &shmax);
	simg->GetSize(simg, &iwmax, &ihmax);

	surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &sptr, &spitch);
	simg->Lock(simg, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &gptr, &gpitch);

	int old_x = -1,
			old_y = -1;

	for (j=height+2*dh-1; j>0; j--) {
		int sy = y+j-dh;

		old_y = sy;

		jPrime = j - height - dh;

		if (sy >=0 && sy < shmax) {
			sdst = (uint32_t *)((uint8_t *)sptr + sy * spitch);

			for (i=width+2*dw-1; i>0; i--) {
				iPrime = i - width - dw;
			
				iOriginal = width + ((iPrime+xc)*cosTheta - (jPrime+yc)*sinTheta)/precision;
				jOriginal = height + ((iPrime+xc)*sinTheta + (jPrime+yc)*cosTheta)/precision;

				if ((iOriginal >= xc) && ((iOriginal-xc) < width) && (jOriginal >= yc) && ((jOriginal-yc) < height)) {
					int gx = iOriginal-xc;
					int gy = jOriginal-yc;

					if ((gx >= 0 && gx < iwmax) && (gy >= 0 && gy < ihmax)) {
						int offset = x+i-dw;

						old_x = offset;

						if (offset >= 0 && offset < swmax) {
							uint32_t spixel = *((uint32_t *)((uint8_t *)gptr + gy * gpitch) + gx),
											 salpha = (spixel >> 0x18) + 1;
							uint32_t dpixel = *(sdst+offset);

							spixel = ((((spixel & 0x00ff00ff) * salpha) >> 8) & 0x00ff00ff) |
								       ((((spixel & 0x0000ff00) * salpha) >> 8) & 0x0000ff00) |
								       ((((spixel & 0xff000000))));

							int32_t gr = (spixel >> 0x10) & 0xff,
											gg = (spixel >> 0x08) & 0xff,
											gb = (spixel >> 0x00) & 0xff,
											ga = (spixel >> 0x18) & 0xff;
							int32_t sr = (dpixel >> 0x10) & 0xff,
											sg = (dpixel >> 0x08) & 0xff,
											sb = (dpixel >> 0x00) & 0xff,
											sa = (0xff - ga);
							int32_t dr = sr,
											dg = sg,
											db = sb,
											da = ga;

							if (ga == 0x00) {
								continue;
							}

							ga = 0xff;

							if (_blit_flags & JBF_ALPHACHANNEL) {
								if (_porterduff_flags == JPF_NONE) {
									dr = (gr*ga + sr*sa);
									dg = (gg*ga + sg*sa);
									db = (gb*ga + sb*sa);
								} else if (_porterduff_flags == JPF_CLEAR) {
									dr = 0;
									dg = 0;
									db = 0;
								} else if (_porterduff_flags == JPF_SRC) {
									dr = gr*0xff;
									dg = gg*0xff;
									db = gb*0xff;
								} else if (_porterduff_flags == JPF_DST) {
									dr = sr*0xff;
									dg = sg*0xff;
									db = sb*0xff;
								} else if (_porterduff_flags == JPF_SRC_OVER) {
									dr = (gr*0xff + sr*sa);
									dg = (gg*0xff + sg*sa);
									db = (gb*0xff + sb*sa);
								} else if (_porterduff_flags == JPF_DST_OVER) {
									dr = (gr*ga + sr*0xff);
									dg = (sg*ga + sg*0xff);
									db = (sb*ga + sb*0xff);
								} else if (_porterduff_flags == JPF_SRC_IN) {
									dr = gr*sa;
									dg = gg*sa;
									db = gb*sa;
								} else if (_porterduff_flags == JPF_DST_IN) {
									dr = sr*ga;
									dg = sg*ga;
									db = sb*ga;
								} else if (_porterduff_flags == JPF_SRC_OUT) {
									dr = gr*ga;
									dg = gg*ga;
									db = gb*ga;
								} else if (_porterduff_flags == JPF_DST_OUT) {
									dr = sr*sa;
									dg = sg*sa;
									db = sb*sa;
								} else if (_porterduff_flags == JPF_SRC_ATOP) {
									dr = (gr*sa + sr*sa);
									dg = (sg*sa + sg*sa);
									db = (sb*sa + sb*sa);
								} else if (_porterduff_flags == JPF_DST_ATOP) {
									dr = (gr*ga + sr*ga);
									dg = (sg*ga + sg*ga);
									db = (sb*ga + sb*ga);
								} else if (_porterduff_flags == JPF_ADD) {
									dr = sr*0xff + ga*0xff;
									dg = sg*0xff + ga*0xff;
									db = sb*0xff + ga*0xff;
									da = 0xff;
								} else if (_porterduff_flags == JPF_XOR) {
									dr = (gr*ga + sr*sa);
									dg = (gg*ga + sg*sa);
									db = (gb*ga + sb*sa);
								}
							}

							dr = dr >> 8;
							dg = dg >> 8;
							db = db >> 8;

							if (dr > 0xff) {
								dr = 0xff;
							}

							if (dg > 0xff) {
								dg = 0xff;
							}

							if (db > 0xff) {
								db = 0xff;
							}
							
							*(sdst+offset) = 0xff000000 | (dr << 0x10) | (dg << 0x08) | (db << 0x00);
						}
					}
				}
			}
		}
	}
	
	simg->Unlock(simg);
	surface->Unlock(surface);
}

}
