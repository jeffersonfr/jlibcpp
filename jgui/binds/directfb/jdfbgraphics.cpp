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

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = _scale.width;
	_clip.height = _scale.height;

	Reset();
}

DFBGraphics::~DFBGraphics()
{
}

void * DFBGraphics::GetNativeSurface()
{
	return _surface;
}

void DFBGraphics::SetNativeSurface(void *surface)
{
	_surface = (IDirectFBSurface *)surface;
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

	// _surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_BLIT));
	_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	// _surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
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

	// _surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_BLIT));
	_surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	// _surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
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

	_surface->SetColor(_surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
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
}

void DFBGraphics::SetLineStyle(jline_style_t t)
{
	if (_surface == NULL) {
		return;
	}

	_line_style = t;
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

	if (line_width == 1) {
		_surface->DrawLine(_surface, x0, y0, x1, y1);
	} else {
		double r = (line_width),
					 dx = xf-xp,
					 dy = yf-yp,
					 d = sqrt((dx*dx)+(dy*dy));

		if (d < 1.0) {
			d = 1.0;
		}

		int c = (int)((r*dy)/d),
				s = (int)((r*dx)/d),
				xdiff = (int)(c/2),
				ydiff = (int)(-s/2),
				r1 = s;

		if (line_width < 2*r1) {
			r1 = c;
		}

		FillTriangle((int)(xp-xdiff), (int)(yp-ydiff), (int)(xp+dx-xdiff), (int)(yp+dy-ydiff), (int)(xp+c-xdiff), (int)(yp-s-ydiff));
		FillTriangle((int)(xp+dx-xdiff), (int)(yp+dy-ydiff), (int)(xp+c-xdiff), (int)(yp-s-ydiff), (int)(xp+c+dx-xdiff), (int)(yp-s+dy-ydiff));
	}
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
	
	for (int i=0; i<=(npoints*interpolation); i++) {
		t = t + stepsize;

		x2 = EvaluateBezier0(x, npoints, t);
		y2 = EvaluateBezier0(y, npoints, t);
	
		_surface->DrawLine(_surface, x1, y1, x2, y2);
		
		x1 = x2;
		y1 = y2;
	}
    
	delete [] x;
	delete [] y;
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

	w = (w >= 0)?w:-w;
	h = (h >= 0)?h:-h;

	_surface->FillRectangle(_surface, x, y ,w, h);
}

void DFBGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width < 0) {
		DrawRectangle0(x, y, w, h, 0, 0, JLJ_MITER, line_width);
	} else {
		DrawRectangle0(x-line_width+1, y-line_width+1, w+2*(line_width-1), h+2*(line_width-1), 0, 0, JLJ_MITER, -line_width);
	}
}

void DFBGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	
	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width);
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	DrawRectangle0(x, y, w, h, dx, dy, JLJ_BEVEL, -std::max(w, h));
}

void DFBGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width);
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	if (line_width < 0) {
		DrawRectangle0(x, y, w, h, dx, dy, JLJ_BEVEL, line_width);
	} else {
		DrawRectangle0(x-line_width+1, y-line_width+1, w+2*(line_width-1), h+2*(line_width-1), dx+line_width, dy+line_width, JLJ_BEVEL, -line_width);
	}
}

void DFBGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	
	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width);
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	DrawRectangle0(x, y, w, h, dx, dy, JLJ_ROUND, -std::max(w, h));
}

void DFBGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	dx = SCALE_TO_SCREEN((dx), _screen.width, _scale.width);
	dy = SCALE_TO_SCREEN((dy), _screen.height, _scale.height);

	if (line_width < 0) {
		DrawRectangle0(x, y, w, h, dx, dy, JLJ_ROUND, line_width);
	} else {
		DrawRectangle0(x-line_width+1, y-line_width+1, w+2*(line_width-1), h+2*(line_width-1), dx+line_width, dy+line_width, JLJ_ROUND, -line_width);
	}
}

void DFBGraphics::FillCircle(int xcp, int ycp, int rp)
{
	FillArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void DFBGraphics::DrawCircle(int xcp, int ycp, int rp)
{
	DrawArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void DFBGraphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	FillArc(xcp, ycp, rxp, ryp, 0.0, 2*M_PI);
}

void DFBGraphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	DrawArc(xcp, ycp, rxp, ryp, 0.0, 2*M_PI);
}

void DFBGraphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width)-1; 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height)-1;
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

	arc0 = fmod(arc0, 2*M_PI);
	arc1 = fmod(arc1, 2*M_PI);

	if (arc1 == 0.0) {
		arc1 = 2*M_PI;
	}

	if (arc0 < 0.0) {
		arc0 = 2*M_PI + arc0;
	}

	if (arc1 < 0.0) {
		arc1 = 2*M_PI + arc1;
	}

	DrawChord0(xc, yc, rx, ry, arc0, arc1, std::max(rx, ry));
}

void DFBGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width)-1; 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height)-1;
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	arc0 = fmod(arc0, 2*M_PI);
	arc1 = fmod(arc1, 2*M_PI);

	if (arc1 == 0.0) {
		arc1 = 2*M_PI;
	}

	if (arc0 < 0.0) {
		arc0 = 2*M_PI + arc0;
	}

	if (arc1 < 0.0) {
		arc1 = 2*M_PI + arc1;
	}

	if (line_width < 0) {
		DrawChord0(xc, yc, rx, ry, arc0, arc1, -line_width);
	} else {
		DrawChord0(xc, yc, rx+lw, ry+lw, arc0, arc1, line_width);
	}
}

void DFBGraphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width)-1; 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height)-1;
	int rx = SCALE_TO_SCREEN((rxp), _screen.width, _scale.width);
	int ry = SCALE_TO_SCREEN((ryp), _screen.height, _scale.height);

	arc0 = fmod(arc0, 2*M_PI);
	arc1 = fmod(arc1, 2*M_PI);

	if (arc1 == 0.0) {
		arc1 = 2*M_PI;
	}

	if (arc0 < 0.0) {
		arc0 = 2*M_PI + arc0;
	}

	if (arc1 < 0.0) {
		arc1 = 2*M_PI + arc1;
	}

	int quadrant = -1;

	if (arc0 >= 0.0 && arc0 < M_PI_2) {
		quadrant = 0;
	} else if (arc0 >= M_PI_2 && arc0 < M_PI) {
		quadrant = 1;
	} else if (arc0 >= M_PI && arc0 < (M_PI+M_PI_2)) {
		quadrant = 2;
	} else if (arc0 >= (M_PI+M_PI_2) && arc0 < 2*M_PI) {
		quadrant = 3;
	}

	if (arc1 < arc0) {
		arc1 = arc1 + 2*M_PI;
	}

	while (arc0 < arc1) {
		double b = arc1,
					 q = quadrant;

		if (quadrant == 0) {
			if (arc1 > M_PI_2) {
				b = M_PI_2;
			}
		} else if (quadrant == 1) {
			if (arc1 > M_PI) {
				b = M_PI;
			}
		} else if (quadrant == 2) {
			if (arc1 > (M_PI+M_PI_2)) {
				b = (M_PI+M_PI_2);
			}
		} else if (quadrant == 3) {
			if (arc1 > 2*M_PI) {
				b = 0.0;

				arc1 = arc1-2*M_PI;
			}
		}

		DrawArc0(xc, yc, rx, ry, arc0, b, std::max(rx, ry), q);

		arc0 = b;
		quadrant = (quadrant+1)%4;
	}
}

void DFBGraphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width)-1; 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height)-1;
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	DrawArcHelper(xc, yc, rx, ry, arc0, arc1, line_width);
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
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width < 0) {
		line_width = -line_width;

		// TODO:: recursive DrawPie(xcp, ycp, rxp-line_width, ryp-line_width, arc0, arc1);
		
		return;
	}

	double t0 = fmod(arc0, 2*M_PI),
				 t1 = fmod(arc1, 2*M_PI);

	if (t1 == 0.0) {
		t1 = 2*M_PI;
	}

	if (t0 < 0.0) {
		t0 = M_PI+t0;
	}

	if (t1 < 0.0) {
		t1 = 2*M_PI+t1;
	}

	double dxangle = (line_width*M_PI_2)/rxp,
				 dyangle = (line_width*M_PI_2)/ryp,
				 step = 0.01;
	
	jpoint_t p[3];

	p[0].x = (rxp+line_width+1)*cos(t0+step);
	p[0].y = -(ryp+line_width+1)*sin(t0+step);
	p[1].x = 0;
	p[1].y = 0;
	p[2].x = (rxp+line_width)*cos(t1);
	p[2].y = -(ryp+line_width)*sin(t1);

	double pvetor = (p[0].x*p[2].y-p[0].y*p[2].x);

	if (pvetor < 0.0) {
		DrawArc(xcp, ycp, rxp, ryp, arc0, arc1);

		double p0x = p[0].x,
					 p0y = p[0].y;

		p[0].x = p[2].x;
		p[0].y = p[2].y;

		p[2].x = p0x;
		p[2].y = p0y;
	} else {
		DrawArc(xcp, ycp, rxp, ryp, arc0+dxangle/2, arc1-dyangle/2);
	}

	jline_join_t line_join = _line_join;

	_line_join = JLJ_BEVEL;

	DrawPolygon(xcp, ycp, p, 3, false);

	_line_join = line_join;
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
	if (npoints < 1) {
		return;
	}

	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	int line_width = (lw != 0)?lw:(_line_width != 0)?1:0;

	if (line_width == 1) {
		int ox = p[0].x + xp, 
				oy = p[0].y + yp, 
				tx, 
				ty;

		for (int i=1; i<npoints; i++) {
			tx = p[i].x + xp;
			ty = p[i].y + yp;

			DrawLine(ox, oy, tx, ty);

			ox = tx;
			oy = ty;
		}

		if (close) {
			DrawLine(ox, oy, p[0].x + xp, p[0].y + yp);
		}
	} else {
		jgui::jpoint_t scaled[6];
		int opened = (close == true)?0:1;

		for (int i=0; i<npoints-opened; i++) {
			int	dx = p[(i+1)%npoints].x-p[i].x,
					dy = p[(i+1)%npoints].y-p[i].y,
					d = sqrt((dx*dx)+(dy*dy));

			if (d > 0) {
				int c = (int)((line_width*dy)/d),
						s = (int)((line_width*dx)/d),
						xr = p[i].x-c,
						yr = p[i].y+s,
						index = i*2;

				FillTriangle(xp+xr, yp+yr, xp+xr+dx, yp+yr+dy, xp+xr+c, yp+yr-s);
				FillTriangle(xp+xr+dx, yp+yr+dy, xp+xr+c, yp+yr-s, xp+xr+c+dx, yp+yr-s+dy);

				scaled[index+0].x = xr;
				scaled[index+0].y = yr;
				scaled[index+1].x = xr+dx;
				scaled[index+1].y = yr+dy;
			}
		}

		if (opened != 0) {
			opened++;
		}

		for (int i=0; i<npoints-opened; i++) {
			if (_line_join == JLJ_BEVEL) {
				FillTriangle(
						xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
						xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y, 
						xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
			} else if (_line_join == JLJ_ROUND) {
				double dx0 = p[(i+1)%npoints].x-p[(i+0)%npoints].x,
							 dy0 = p[(i+1)%npoints].y-p[(i+0)%npoints].y,
							 dx1 = p[(i+2)%npoints].x-p[(i+1)%npoints].x,
							 dy1 = p[(i+2)%npoints].y-p[(i+1)%npoints].y,
							 ang0 = asin(sqrt((dx0*dx0)/(dx0*dx0+dy0*dy0))),
							 ang1 = asin(sqrt(dx1*dx1)/sqrt(dx1*dx1+dy1*dy1));

				if (dx0 > 0.0) {
					ang0 = ang0 + M_PI;
				} else if (dy0 > 0.0) {
					ang0 = M_PI - ang0;
				}

				if (dx1 > 0.0) {
					ang1 = ang1 + M_PI;
				} else if (dy1 > 0.0) {
					ang1 = M_PI - ang1;
				}

				FillArc(xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y+1, line_width, line_width-1, ang0, ang1);
			} else if (_line_join == JLJ_MITER) {
				int a1 = scaled[((i+0)%npoints)*2+0].y-scaled[((i+0)%npoints)*2+1].y,
						b1 = scaled[((i+0)%npoints)*2+0].x-scaled[((i+0)%npoints)*2+1].x,
						c1 = scaled[((i+0)%npoints)*2+0].x*scaled[((i+0)%npoints)*2+1].y-scaled[((i+0)%npoints)*2+1].x*scaled[((i+0)%npoints)*2+0].y;
				int a2 = scaled[((i+1)%npoints)*2+0].y-scaled[((i+1)%npoints)*2+1].y,
						b2 = scaled[((i+1)%npoints)*2+0].x-scaled[((i+1)%npoints)*2+1].x,
						c2 = scaled[((i+1)%npoints)*2+0].x*scaled[((i+1)%npoints)*2+1].y-scaled[((i+1)%npoints)*2+1].x*scaled[((i+1)%npoints)*2+0].y;
				int dx0 = (a1*b2-a2*b1),
						dy0 = (a1*b2-a2*b1);
				
				FillTriangle(
						xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
						xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y, 
						xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
				
				if (dx0 != 0 && dy0 != 0) {
					int x0 = (b1*c2-b2*c1)/(a1*b2-a2*b1),
							y0 = (a1*c2-a2*c1)/(a1*b2-a2*b1);

					FillTriangle(
							xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
							xp+x0, yp+y0,
							xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
				}
			}
		}
	}
}

void DFBGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
	if (npoints < 1) {
		return;
	}

	jpoint_t points[npoints];
	int x1 = 0,
			y1 = 0,
			x2 = 0,
			y2 = 0;

	for (int i=0; i<npoints; i++) {
		points[i].x = SCALE_TO_SCREEN((xp+p[i].x+_translate.x), _screen.width, _scale.width); 
		points[i].y = SCALE_TO_SCREEN((yp+p[i].y+_translate.y), _screen.height, _scale.height);

		if (points[i].x < x1) {
			x1 = points[i].x;
		}

		if (points[i].x > x2) {
			x2 = points[i].x;
		}

		if (points[i].y < y1) {
			y1 = points[i].y;
		}

		if (points[i].y > y2) {
			y2 = points[i].y;
		}
	}

	FillPolygon0(points, npoints, x1, y1, x2, y2);
}

void DFBGraphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
{
	std::vector<jgradient_t>::iterator i=_gradient_stops.begin();
	Color color0 = i->color;
	Color color1 = color0;
	int start = 0;
	int end = start;
	int height = hp;

	Color color = GetColor();

	for (; i!=_gradient_stops.end(); i++) {
		jgradient_t t = (*i);

		color1 = t.color;
		end = (int)(height*t.stop);

		for (int i=start; i<end; i++) {
			UpdateGradientColor(color0, color1, abs(end-start), height-i);
			FillArc(xcp, ycp, wp, hp, 0, 2*M_PI);

			xcp += 1;
			ycp += 1;
			wp -= 2;
			hp -= 2;

			if (wp < 0 || hp < 0) {
				break;
			}
		}

		color0 = color1;
		start = end;
	}

	SetColor(color);
}

void DFBGraphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
	if (_gradient_stops.size() == 0) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	std::vector<jgradient_t>::iterator i=_gradient_stops.begin();
	Color color0 = i->color;
	Color color1 = color0;
	int start = 0;
	int end = start;

	Color color = GetColor();

	for (; i!=_gradient_stops.end(); i++) {
		jgradient_t t = (*i);

		color1 = t.color;

		if (x1p != x2p) {
			end = (int)(w*t.stop);

			for (int i=start; i<end; i++) {
				UpdateGradientColor(color0, color1, abs(end-start), i);
				_surface->DrawLine(_surface, x+i, y, x+i, y+h-1);
			}
		} else {
			end = (int)(h*t.stop);

			for (int i=start; i<end; i++) {
				UpdateGradientColor(color0, color1, abs(end-start), i);
				_surface->DrawLine(_surface, x, y+i, x+w-1, y+i);
			}
		}

		color0 = color1;
		start = end;
	}

	SetColor(color);
}

void DFBGraphics::DrawString(std::string text, int xp, int yp)
{
	if (_surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	/*
	if (_radians == 0.0) {
		int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
				y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

		_surface->DrawString(_surface, text.c_str(), -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	} else */ {
		Image *off = Image::CreateImage(_font->GetStringWidth(text), _font->GetAscender() + _font->GetDescender(), JPF_ARGB, _scale.width, _scale.height);

		off->GetGraphics()->SetFont(_font);
		off->GetGraphics()->SetColor(_color);

		IDirectFBSurface *fsurface = (IDirectFBSurface *)(off->GetGraphics()->GetNativeSurface());

		fsurface->DrawString(fsurface, text.c_str(), -1, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
		fsurface->DrawString(fsurface, text.c_str(), -1, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

		jblitting_flags_t bf = GetBlittingFlags();
		// jcomposite_flags_t cf = GetCompositeFlags();

		SetBlittingFlags((jblitting_flags_t)(bf | JBF_COLORALPHA));
		DrawImage(off, xp, yp);
		SetBlittingFlags(bf);

		delete off;
	}
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

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.width = w;
	desc.height = h;
	desc.pixelformat = DSPF_ARGB;

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

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_PIXELFORMAT);
	desc.pixelformat = DSPF_ARGB;

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

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.width = (w*dw)/sw;
	desc.height = (h*dh)/sh;
	desc.pixelformat = DSPF_ARGB;

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

		Image *image = img->Scaled(iwp, ihp);

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
	_radians = 0.0;

	_translate_image.x = 0;
	_translate_image.y = 0;

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

	_surface->GetSize(_surface, &swmax, &shmax);
	simg->GetSize(simg, &iwmax, &ihmax);

	_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &sptr, &spitch);
	simg->Lock(simg, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &gptr, &gpitch);

	int init_x = width+2*dw-1,
			init_y = height+2*dh-1;
	// int old_x = -1,
	//		old_y = -1;

	for (j=init_y; j>0; j--) {
		int sy = y+j-dh;

		jPrime = j - height - dh;

		if (sy >=0 && sy < shmax) {
			sdst = (uint32_t *)((uint8_t *)sptr + sy * spitch);

			for (i=init_x; i>0; i--) {
				iPrime = i - width - dw;
			
				iOriginal = width + ((iPrime+xc)*cosTheta - (jPrime+yc)*sinTheta)/precision;
				jOriginal = height + ((iPrime+xc)*sinTheta + (jPrime+yc)*cosTheta)/precision;

				if ((iOriginal >= xc) && ((iOriginal-xc) < width) && (jOriginal >= yc) && ((jOriginal-yc) < height)) {
					int gx = iOriginal-xc;
					int gy = jOriginal-yc;

					if ((gx >= 0 && gx < iwmax) && (gy >= 0 && gy < ihmax)) {
						int offset = x+i-dw;

						if (offset >= 0 && offset < swmax) {
							uint32_t spixel = *((uint32_t *)((uint8_t *)gptr + gy * gpitch) + gx),
											 salpha = (spixel >> 0x18) + 1;
							uint32_t dpixel = *(sdst+offset);

							if (_is_premultiply == true) {
								spixel = ((((spixel & 0x00ff00ff) * salpha) >> 8) & 0x00ff00ff) | ((((spixel & 0x0000ff00) * salpha) >> 8) & 0x0000ff00) | ((((spixel & 0xff000000))));
							}

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
											db = sb; // , da = ga;

							if (ga == 0x00) {
								continue;
							}

							ga = 0xff;

							if (_blit_flags & JBF_ALPHACHANNEL) {
								if (_composite_flags == JCF_NONE) {
									dr = (gr*ga + sr*sa);
									dg = (gg*ga + sg*sa);
									db = (gb*ga + sb*sa);
								} else if (_composite_flags == JCF_CLEAR) {
									dr = 0;
									dg = 0;
									db = 0;
								} else if (_composite_flags == JCF_SRC) {
									dr = gr*0xff;
									dg = gg*0xff;
									db = gb*0xff;
								} else if (_composite_flags == JCF_DST) {
									dr = sr*0xff;
									dg = sg*0xff;
									db = sb*0xff;
								} else if (_composite_flags == JCF_SRC_OVER) {
									dr = (gr*0xff + sr*sa);
									dg = (gg*0xff + sg*sa);
									db = (gb*0xff + sb*sa);
								} else if (_composite_flags == JCF_DST_OVER) {
									dr = (gr*ga + sr*0xff);
									dg = (sg*ga + sg*0xff);
									db = (sb*ga + sb*0xff);
								} else if (_composite_flags == JCF_SRC_IN) {
									dr = gr*sa;
									dg = gg*sa;
									db = gb*sa;
								} else if (_composite_flags == JCF_DST_IN) {
									dr = sr*ga;
									dg = sg*ga;
									db = sb*ga;
								} else if (_composite_flags == JCF_SRC_OUT) {
									dr = gr*ga;
									dg = gg*ga;
									db = gb*ga;
								} else if (_composite_flags == JCF_DST_OUT) {
									dr = sr*sa;
									dg = sg*sa;
									db = sb*sa;
								} else if (_composite_flags == JCF_SRC_ATOP) {
									dr = (gr*sa + sr*sa);
									dg = (sg*sa + sg*sa);
									db = (sb*sa + sb*sa);
								} else if (_composite_flags == JCF_DST_ATOP) {
									dr = (gr*ga + sr*ga);
									dg = (sg*ga + sg*ga);
									db = (sb*ga + sb*ga);
								} else if (_composite_flags == JCF_ADD) {
									dr = sr*0xff + ga*0xff;
									dg = sg*0xff + ga*0xff;
									db = sb*0xff + ga*0xff;
									// da = 0xff;
								} else if (_composite_flags == JCF_XOR) {
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
						
						// old_x = offset;
					}
				}
			}
		}
		
		// old_y = sy;
	}
	
	simg->Unlock(simg);
	_surface->Unlock(_surface);
}

int DFBGraphics::CalculateGradientChannel(int schannel, int dchannel, int distance, int offset) 
{
	if (schannel == dchannel) {
		return schannel;
	}

	return (int)(schannel-((schannel-dchannel)*((double)offset/(double)distance))) & 0xff;
}

void DFBGraphics::UpdateGradientColor(Color &scolor, Color &dcolor, int distance, int offset) 
{
	int a = CalculateGradientChannel(scolor.GetAlpha(), dcolor.GetAlpha(), distance, offset);
	int r = CalculateGradientChannel(scolor.GetRed(), dcolor.GetRed(), distance, offset);
	int g = CalculateGradientChannel(scolor.GetGreen(), dcolor.GetGreen(), distance, offset);
	int b = CalculateGradientChannel(scolor.GetBlue(), dcolor.GetBlue(), distance, offset);
	
	SetColor((a << 24) | (r << 16) | (g << 8) | (b << 0));
}

void DFBGraphics::FillPolygon0(jgui::jpoint_t *points, int npoints, int x1p, int y1p, int x2p, int y2p)
{
	if (npoints < 3) {
		return;
	}

	int xnew,
			ynew,
			xold,
			yold,
			x1,
			y1,
			x2,
			y2,
			inside;

	for (int x=x1p; x<x2p; x++) {
		for (int y=y1p; y<y2p; y++) {
			inside = 0;

			xold = points[npoints-1].x;
			yold = points[npoints-1].y;

			for (int i=0; i<npoints; i++) {
				xnew = points[i].x;
				ynew = points[i].y;

				if (xnew > xold) {
					x1 = xold;
					x2 = xnew;
					y1 = yold;
					y2 = ynew;
				} else {
					x1 = xnew;
					x2 = xold;
					y1 = ynew;
					y2 = yold;
				}

				// edge "open" at one end
				if ((xnew < x) == (x <= xold) && ((long)y-(long)y1)*(long)(x2-x1) < ((long)y2-(long)y1)*(long)(x-x1)) {
					inside = !inside;
				}

				xold = xnew;
				yold = ynew;
			}

			if (inside != 0) {
				_surface->DrawLine(_surface, x, y, x, y);
			}
		}
	}
}

void DFBGraphics::DrawRectangle0(int x, int y, int w, int h, int dx, int dy, jline_join_t join, int size)
{ 
	if (_surface == NULL || size >= 0 || w <= 0 || h <= 0) {
		return;
	}

	bool close = false;

	size = -size;

	if (size > (std::min(w, h)/2)) {
		size = std::min(w, h)/2;

		close = true;
	}

	if (join == JLJ_MITER) {
		x = x + size;
		y = y + size;
		w = w - 2*size;
		h = h - 2*size;

		_surface->FillRectangle(_surface, x-size, y-size, w+2*size, size);
		_surface->FillRectangle(_surface, x-size, y+h, w+2*size, size); // _
		_surface->FillRectangle(_surface, x-size, y, size, h); // |<- 
		_surface->FillRectangle(_surface, x+w, y, size, h); // ->|

		if (close == true) {
			_surface->FillRectangle(_surface, x, y, w, h);
		}
	} else if (join == JLJ_BEVEL) {
		if (dx > w/2) {
			dx = w/2;
		}

		if (dy > h/2) {
			dy = h/2;
		}

		if (size <= (std::max(dx, dy))) {
			if (size == 1) {
				_surface->DrawLine(_surface, x+dx, y, x, y+dy);
				_surface->DrawLine(_surface, x+w-dx-1, y, x+w-1, y+dy);
				_surface->DrawLine(_surface, x+w-1, y+h-dy, x+w-dx-1, y+h);
				_surface->DrawLine(_surface, x, y+h-dy, x+dx, y+h);
			} else {
				_surface->FillTriangle(_surface, x+dx, y, x, y+dy, x+size, y+dy);
				_surface->FillTriangle(_surface, x+size, y+dy, x+dx, y, x+dx, y+size);

				_surface->FillTriangle(_surface, x+w-dx, y, x+w-dx, y+size, x+w, y+dy);
				_surface->FillTriangle(_surface, x+w-dx, y+size, x+w-size, y+dy,  x+w, y+dy);

				_surface->FillTriangle(_surface, x+w-size, y+h-dy, x+w, y+h-dy, x+w-dx, y+h);
				_surface->FillTriangle(_surface, x+w-size, y+h-dy, x+w-dx, y+h, x+w-dx, y+h-size);

				_surface->FillTriangle(_surface, x, y+h-dy, x+size, y+h-dy, x+dx, y+h);
				_surface->FillTriangle(_surface, x+size, y+h-dy, x+dx, y+h-size, x+dx, y+h);
			} 

			_surface->FillRectangle(_surface, x+dx, y, w-2*dx, size);
			_surface->FillRectangle(_surface, x+dx, y+h-size, w-2*dx, size);
			_surface->FillRectangle(_surface, x, y+dy, size, h-2*dy);
			_surface->FillRectangle(_surface, x+w-size, y+dy, size, h-2*dy);
		} else {
			_surface->FillTriangle(_surface, x+dx, y, x, y+dy, x+dx, y+dy);
			_surface->FillTriangle(_surface, x+w-dx, y, x+w, y+dy, x+w-dx, y+dy);
			_surface->FillTriangle(_surface, x, y+h-dy, x+dx, y+h-dy, x+dx, y+h);
			_surface->FillTriangle(_surface, x+w-dx, y+h-dy, x+w, y+h-dy, x+w-dx, y+h);

			_surface->FillRectangle(_surface, x+dx, y, w-2*dx, dy);
			_surface->FillRectangle(_surface, x+dx, y+h-dy, w-2*dx, dy);
			_surface->FillRectangle(_surface, x, y+dy, dx, h-2*dy);
			_surface->FillRectangle(_surface, x+w-dx, y+dy, dx, h-2*dy);

			DrawRectangle0(x+dx, y+dy, w-2*dx, h-2*dy, 0, 0, JLJ_MITER, std::max(dx, dy)-size-1);
		}
	} else if (join == JLJ_ROUND) {
		DrawArcHelper(x+dx-1, y+dy-1, dx, dy, M_PI_2, M_PI, -size);
		DrawArcHelper(x+dx-1, y+h-dy-1, dx, dy, M_PI, M_PI+M_PI_2, -size);
		DrawArcHelper(x+w-dx-1, y+h-dy-1, dx, dy, M_PI+M_PI_2, 2*M_PI, -size);
		DrawArcHelper(x+w-dx-1, y+dy-1, dx, dy, 0.0, M_PI_2, -size);

		if (dx > w/2) {
			dx = w/2;
		}

		if (dy > h/2) {
			dy = h/2;
		}

		if (size <= (std::max(dx, dy))) {
			_surface->FillRectangle(_surface, x+dx, y, w-2*dx, size);
			_surface->FillRectangle(_surface, x+dx, y+h-size, w-2*dx, size);
			_surface->FillRectangle(_surface, x, y+dy, size, h-2*dy);
			_surface->FillRectangle(_surface, x+w-size, y+dy, size, h-2*dy);
		} else {
			_surface->FillRectangle(_surface, x+dx, y, w-2*dx, dy);
			_surface->FillRectangle(_surface, x+dx, y+h-dy, w-2*dx, dy);
			_surface->FillRectangle(_surface, x, y+dy, dx, h-2*dy);
			_surface->FillRectangle(_surface, x+w-dx, y+dy, dx, h-2*dy);

			DrawRectangle0(x+dx, y+dy, w-2*dx, h-2*dy, 0, 0, JLJ_MITER, std::max(dx, dy)-size-1);
		}
	}
}

void DFBGraphics::DrawArc0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size, int quadrant)
{
	if (size == 0) {
		return;
	}

	if (_surface == NULL) {
		return;
	}

	double line_width = (double)size;
	
	if (line_width > std::max(rx, ry)-1) {
		line_width = std::max(rx, ry)-1;
	}
	
	if (quadrant == 0) {
		arc0 = arc0-0.0;
		arc1 = arc1-0.0;
	} else if (quadrant == 1) {
		double t0 = arc0;

		arc0 = M_PI-arc1;
		arc1 = M_PI-t0;
	} else if (quadrant == 2) {
		arc0 = arc0-M_PI;
		arc1 = arc1-M_PI;
	} else if (quadrant == 3) {
		double t0 = arc0;

		arc0 = 2*M_PI-arc1;
		arc1 = 2*M_PI-t0;
	}
		
	double min_rx = rx-line_width,
				 min_ry = ry-line_width,
				 max_rx = rx,
				 max_ry = ry;
	double dmin_rx = min_rx*min_rx,
				 dmin_ry = min_ry*min_ry,
				 dmax_rx = max_rx*max_rx,
				 dmax_ry = max_ry*max_ry;
	double x_inf = max_rx*cos(arc0),
				 y_inf = max_ry*sin(arc0),
				 x_sup = max_rx*cos(arc1),
				 y_sup = max_ry*sin(arc1);
	DFBRegion lines[4*(int)max_ry];
	int old_x = -1,
			old_y = -1;
	int k = 0;

	for (double y=0; y<max_ry; y+=1.0) {
		double eq_y_sup0 = x_sup*y,
					 eq_y_inf0 = x_inf*y;
		double xi = -1.0,
					 xf = -1.0;
		bool flag = false;

		for (double x=0; x<max_rx; x+=1.0) {
			double min_ellipse,
						 max_ellipse;

			if (dmin_rx == 0.0 || dmin_ry == 0.0) {
				min_ellipse = 1.0;
				max_ellipse = (x*x)/dmax_rx+(y*y)/dmax_ry;
			} else {
				min_ellipse = (x*x)/dmin_rx+(y*y)/dmin_ry;
				max_ellipse = (x*x)/dmax_rx+(y*y)/dmax_ry;
			}

			if (min_ellipse >= 1.0 && max_ellipse <= 1.0) {
				double eq_y_sup = eq_y_sup0-y_sup*x,
							 eq_y_inf = eq_y_inf0-y_inf*x;

				eq_y_sup = eq_y_inf0-y_inf*x;
				eq_y_inf = eq_y_sup0-y_sup*x;

				if (eq_y_sup >= 0.0 && eq_y_inf <= 0.0) {
						if (flag == false) {
						xi = x;

						flag = true;
					}

					xf = x;
				}
			}
		}

		if  (flag == false) {
			continue;
		}

		if (line_width <= 1) {
			if (old_x < 0) {
				old_x = xi;
			}

			if (old_y < 0) {
				old_y = y;
			}

			if (quadrant == 0) {
				lines[k].x1 = (int)(xc+old_x);
				lines[k].y1 = (int)(yc-y);
				lines[k].x2 = (int)(xc+xi);
				lines[k].y2 = (int)(yc-y);
			} else if (quadrant == 1) {
				lines[k].x1 = (int)(xc-old_x);
				lines[k].y1 = (int)(yc-y);
				lines[k].x2 = (int)(xc-xi+1);
				lines[k].y2 = (int)(yc-y);
			} else if (quadrant == 2) {
				lines[k].x1 = (int)(xc-old_x);
				lines[k].y1 = (int)(yc+y+1);
				lines[k].x2 = (int)(xc-xi+1);
				lines[k].y2 = (int)(yc+y+1);
			} else if (quadrant == 3) {
				lines[k].x1 = (int)(xc+old_x);
				lines[k].y1 = (int)(yc+y+1);
				lines[k].x2 = (int)(xc+xi);
				lines[k].y2 = (int)(yc+y+1);
			}

			k++;

			old_x = xi;
			old_y = y;
		} else {
			if (quadrant == 0) {
				lines[k].x1 = (int)(xc+xi);
				lines[k].y1 = (int)(yc-y);
				lines[k].x2 = (int)(xc+xf);
				lines[k].y2 = (int)(yc-y);
			} else if (quadrant == 1) {
				lines[k].x1 = (int)(xc-xf);
				lines[k].y1 = (int)(yc-y);
				lines[k].x2 = (int)(xc-xi+1);
				lines[k].y2 = (int)(yc-y);
			} else if (quadrant == 2) {
				lines[k].x1 = (int)(xc-xf);
				lines[k].y1 = (int)(yc+y+1);
				lines[k].x2 = (int)(xc-xi+1);
				lines[k].y2 = (int)(yc+y+1);
			} else if (quadrant == 3) {
				lines[k].x1 = (int)(xc+xi);
				lines[k].y1 = (int)(yc+y+1);
				lines[k].x2 = (int)(xc+xf);
				lines[k].y2 = (int)(yc+y+1);
			}

			k++;
		}
	}
	
	_surface->DrawLines(_surface, lines, k);
}

void DFBGraphics::DrawArcHelper(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
{
	arc0 = fmod(arc0, 2*M_PI);
	arc1 = fmod(arc1, 2*M_PI);

	if (arc1 == 0.0) {
		arc1 = 2*M_PI;
	}

	if (arc0 < 0.0) {
		arc0 = 2*M_PI + arc0;
	}

	if (arc1 < 0.0) {
		arc1 = 2*M_PI + arc1;
	}

	int quadrant = -1;

	if (arc0 >= 0.0 && arc0 < M_PI_2) {
		quadrant = 0;
	} else if (arc0 >= M_PI_2 && arc0 < M_PI) {
		quadrant = 1;
	} else if (arc0 >= M_PI && arc0 < (M_PI+M_PI_2)) {
		quadrant = 2;
	} else if (arc0 >= (M_PI+M_PI_2) && arc0 < 2*M_PI) {
		quadrant = 3;
	}

	if (arc1 < arc0) {
		arc1 = arc1 + 2*M_PI;
	}

	while (arc0 < arc1) {
		double b = arc1,
					 q = quadrant;

		if (quadrant == 0) {
			if (arc1 > M_PI_2) {
				b = M_PI_2;
			}
		} else if (quadrant == 1) {
			if (arc1 > M_PI) {
				b = M_PI;
			}
		} else if (quadrant == 2) {
			if (arc1 > (M_PI+M_PI_2)) {
				b = (M_PI+M_PI_2);
			}
		} else if (quadrant == 3) {
			if (arc1 > 2*M_PI) {
				b = 0.0;

				arc1 = arc1-2*M_PI;
			}
		}

		if (size < 0) {
			DrawArc0(xc, yc, rx-1, ry-1, arc0, b, -size, q);
		} else {
			DrawArc0(xc, yc, rx+size-1, ry+size-1, arc0, b, size, q);
		}

		arc0 = b;
		quadrant = (quadrant+1)%4;
	}
}

void DFBGraphics::DrawChord0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
{
	if (_surface == NULL) {
		return;
	}

	if (size == 0) {
		return;
	}

	double line_width = (double)size;
	
	if (line_width > std::max(rx, ry)) {
		line_width = std::max(rx, ry);
	}
	
	double min_rx = rx-line_width,
				 min_ry = ry-line_width,
				 max_rx = rx,
				 max_ry = ry;
	double dmin_rx = min_rx*min_rx,
				 dmin_ry = min_ry*min_ry,
				 dmax_rx = max_rx*max_rx,
				 dmax_ry = max_ry*max_ry;
	double x_inf = max_rx*cos(arc0),
				 y_inf = max_ry*sin(arc0),
				 x_sup = max_rx*cos(arc1),
				 y_sup = max_ry*sin(arc1);
	double a = (y_inf-y_sup),
				 b = (x_inf-x_sup),
				 c = (x_inf*y_sup-x_sup*y_inf);
	double dxangle = (line_width*M_PI_2)/max_rx,
				 dyangle = (line_width*M_PI_2)/max_ry;

	double x0_inf = min_rx*cos(arc0+dxangle),
				 y0_inf = min_ry*sin(arc0+dyangle),
				 x0_sup = min_rx*cos(arc1-dxangle),
				 y0_sup = min_ry*sin(arc1-dyangle);
	double a0 = (y0_inf-y0_sup),
				 b0 = (x0_inf-x0_sup),
				 c0 = (x0_inf*y0_sup-x0_sup*y0_inf);
	
	DFBRegion lines[4*(int)max_ry];
	int k = 0;

	for (double y=-max_ry; y<max_ry; y+=1.0) {
		double xi = -1.0,
					 xf = -1.0;
		bool flag = false;

		for (double x=-max_rx; x<max_rx; x+=1.0) {
			double min_ellipse,
						 max_ellipse;

			if (dmin_rx == 0.0 || dmin_ry == 0.0) {
				min_ellipse = 1.0;
				max_ellipse = (x*x)/dmax_rx+(y*y)/dmax_ry;
			} else {
				min_ellipse = (x*x)/dmin_rx+(y*y)/dmin_ry;
				max_ellipse = (x*x)/dmax_rx+(y*y)/dmax_ry;
			}

			double eq_y_inf = a0*x+b0*y+c0,
						 eq_y_sup = a*x+b*y+c;

			if (max_ellipse <= 1.0) {
				if (eq_y_sup <= 0.0) {
					if (min_ellipse >= 1.0 || eq_y_inf >= 0.0) {
						if (flag == false) {
							xi = x;

							flag = true;
						}

						xf = x;
					} else if (min_ellipse < 1.0) {
						if (flag == true) {
							lines[k].x1 = (int)(xc+xi);
							lines[k].y1 = (int)(yc+y);
							lines[k].x2 = (int)(xc+x);
							lines[k].y2 = (int)(yc+y);

							k++;

							flag = false;
						}
					}
				}
			}
		}

		if (flag == true) {
			lines[k].x1 = (int)(xc+xi);
			lines[k].y1 = (int)(yc+y);
			lines[k].x2 = (int)(xc+xf);
			lines[k].y2 = (int)(yc+y);

			k++;
		}
	}
	
	_surface->DrawLines(_surface, lines, k);
}

void DFBGraphics::AntiAlias0(DFBRegion *lines, int size)
{
	int cx1 = 0,
			cx2 = 0,
			cy1 = 0,
			cy2 = 0;
	int level = 4;
	double up = 0,
				 alpha = 1.5;

	for (int y=0; y<size; y++) {
		// lines[y].x1 = lines[y].x1 + 1;
		lines[y].x2 = lines[y].x2 - 1;

		if ((lines[y].x2 - lines[y].x1) < 0) {
			lines[y].x1 = lines[y].x2;
		}
	}

	for (int y=0; y<size; y++) {
		if (lines[y].x1 > lines[y+1].x1) {
			up = true;
			cx1 = 1;
			cy1 = lines[y].x1-lines[y+1].x1-1;
		} else if (lines[y].x1 < lines[y+1].x1) {
			up = false;
			cx1 = 1;
			cy1 = lines[y+1].x1-lines[y].x1-1;
		} else if (lines[y].x1 == lines[y+1].x1) {
			if (cx1 > 0) {
				cx1 = cx1 + 1;
			}
		}

		if (lines[y].x2 > lines[y+1].x2) {
			up = true;
			cx2 = 1;
			cy2 = lines[y].x2-lines[y+1].x2-1;
		} else if (lines[y].x2 < lines[y+1].x2) {
			up = false;
			cx2 = 1;
			cy2 = lines[y+1].x2-lines[y].x2-1;
		} else if (lines[y].x2 == lines[y+1].x2) {
			if (cx2 > 0) {
				cx2 = cx2 + 1;
			}
		}

		if (cx1 > level) {
			cx1 = level;
		}

		if (cy1 > level) {
			cy1 = level;
		}

		if (cx2 > level) {
			cx2 = level;
		}

		if (cy2 > level) {
			cy2 = level;
		}

		if (cx1 > 0) {
			_surface->SetColor(_surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(alpha*cx1));
			_surface->DrawLine(_surface, lines[y].x1-1, lines[y].y1, lines[y].x1-1, lines[y].y1);

			if (cy1 > 0) {
				for (int i=1; i<cy1; i++) {
					_surface->SetColor(_surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(alpha*(i+1)));
					_surface->DrawLine(_surface, lines[y].x1-i-1, lines[y].y1, lines[y].x1-i-1, lines[y].y1);
				}
			}
		}

		if (cx2 > 0) {
			_surface->SetColor(_surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(alpha*cx2));
			_surface->DrawLine(_surface, lines[y].x2+1, lines[y].y1, lines[y].x2+1, lines[y].y1);

			if (cy2 > 0) {
				for (int i=1; i<cy2; i++) {
					_surface->SetColor(_surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(alpha*(i+1)));
					_surface->DrawLine(_surface, lines[y].x2+i+1, lines[y].y1, lines[y].x2+i+1, lines[y].y1);
				}
			}
		}
	}

	_surface->SetColor(_surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
}

}
