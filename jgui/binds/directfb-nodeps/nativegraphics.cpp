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
#include "nativegraphics.h"
#include "nativefont.h"
#include "nativeimage.h"
#include "jmath.h"
#include "jstringtokenizer.h"
#include "jimage.h"
#include "jgfxhandler.h"
#include "jfont.h"
#include "jstringutils.h"
#include "jrectangle.h"
#include "jcharset.h"
#include "joutofboundsexception.h"
#include "jnullpointerexception.h"

#define M_2PI	(2*M_PI)

namespace jgui {

NativeGraphics::NativeGraphics(void *surface, jpixelformat_t pixelformat, int wp, int hp, bool premultiply):
	jgui::Graphics()
{
	jcommon::Object::SetClassName("jgui::NativeGraphics");

	_pixelformat = pixelformat;
	_clip.x = 0;
	_clip.y = 0;
	_clip.width = wp;
	_clip.height = hp;
	_premultiply = premultiply;

	_surface = (IDirectFBSurface *)surface;

	if (_surface != NULL) {
		DFBSurfaceDrawingFlags draw = DSDRAW_BLEND;
		DFBSurfaceBlittingFlags blit = DSBLIT_BLEND_ALPHACHANNEL;

		if (_premultiply == true) {
			draw = (DFBSurfaceDrawingFlags)(draw | DSDRAW_SRC_PREMULTIPLY);
			blit = (DFBSurfaceBlittingFlags)(blit | DSBLIT_SRC_PREMULTIPLY);
		}

		_surface->SetDrawingFlags(_surface, draw);
		_surface->SetBlittingFlags(_surface, blit);
		_surface->SetPorterDuff(_surface, (DFBSurfacePorterDuffRule)(DSPD_SRC_OVER));
	}

	Reset();
}

NativeGraphics::~NativeGraphics()
{
}

void * NativeGraphics::GetNativeSurface()
{
	return _surface;
}

void NativeGraphics::SetNativeSurface(void *data, int wp, int hp)
{
	_surface = (IDirectFBSurface *)data;
	
	if (_surface != NULL) {
		DFBSurfaceDrawingFlags draw = DSDRAW_BLEND;
		DFBSurfaceBlittingFlags blit = DSBLIT_BLEND_ALPHACHANNEL;

		if (_premultiply == true) {
			draw = (DFBSurfaceDrawingFlags)(draw | DSDRAW_SRC_PREMULTIPLY);
			blit = (DFBSurfaceBlittingFlags)(blit | DSBLIT_SRC_PREMULTIPLY);
		}

		_surface->SetDrawingFlags(_surface, draw);
		_surface->SetBlittingFlags(_surface, blit);
	}
}

void NativeGraphics::Dump(std::string dir, std::string prefix)
{
	if (_surface != NULL) {
		_surface->Dump(_surface, dir.c_str(), prefix.c_str());
	}
}

jregion_t NativeGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, _internal_clip.x, _internal_clip.y, _internal_clip.width, _internal_clip.height);
	
	SetClip(clip.x-_translate.x, clip.y-_translate.y, clip.width, clip.height);

	return clip;
}

void NativeGraphics::SetClip(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int w;
	int h;

	_surface->GetSize(_surface, &w, &h);

	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, 0, 0, w, h);
	
	_clip.x = clip.x - _translate.x;
	_clip.y = clip.y - _translate.y;
	_clip.width = clip.width;
	_clip.height = clip.height;
	
	_internal_clip = clip;

	if (_surface != NULL) {
		DFBRegion rgn;

		rgn.x1 = clip.x;
		rgn.y1 = clip.y;
		rgn.x2 = clip.x+clip.width+1;
		rgn.y2 = clip.y+clip.height+1;

		_surface->SetClip(_surface, NULL);
		_surface->SetClip(_surface, &rgn);
	}
}

jregion_t NativeGraphics::GetClip()
{
	return _clip;
}

void NativeGraphics::ReleaseClip()
{
	if (_surface == NULL) {
		return;
	}

	int w;
	int h;

	_surface->GetSize(_surface, &w, &h);

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = w;
	_clip.height = h;

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
		_clip.width = rgn.x2;
		_clip.height = rgn.y2;
	}
}

void NativeGraphics::SetCompositeFlags(jcomposite_flags_t t)
{
	if (_surface == NULL) {
		return;
	}

	_composite_flags = t;

	if (_composite_flags == JCF_CLEAR) {
		_surface->SetPorterDuff(_surface, DSPD_CLEAR);
	} else if (_composite_flags == JCF_SRC) {
		_surface->SetPorterDuff(_surface, DSPD_SRC);
	} else if (_composite_flags == JCF_SRC_OVER) {
		_surface->SetPorterDuff(_surface, DSPD_SRC_OVER);
	} else if (_composite_flags == JCF_SRC_IN) {
		_surface->SetPorterDuff(_surface, DSPD_SRC_IN);
	} else if (_composite_flags == JCF_SRC_OUT) {
		_surface->SetPorterDuff(_surface, DSPD_SRC_OUT);
	} else if (_composite_flags == JCF_SRC_ATOP) {
		_surface->SetPorterDuff(_surface, DSPD_SRC_ATOP);
	} else if (_composite_flags == JCF_DST) {
		_surface->SetPorterDuff(_surface, DSPD_DST);
	} else if (_composite_flags == JCF_DST_OVER) {
		_surface->SetPorterDuff(_surface, DSPD_DST_OVER);
	} else if (_composite_flags == JCF_DST_IN) {
		_surface->SetPorterDuff(_surface, DSPD_DST_IN);
	} else if (_composite_flags == JCF_DST_OUT) {
		_surface->SetPorterDuff(_surface, DSPD_DST_OUT);
	} else if (_composite_flags == JCF_DST_ATOP) {
		_surface->SetPorterDuff(_surface, DSPD_DST_ATOP);
	} else if (_composite_flags == JCF_ADD) {
		_surface->SetPorterDuff(_surface, DSPD_ADD);
	} else if (_composite_flags == JCF_XOR) {
		_surface->SetPorterDuff(_surface, DSPD_XOR);
	}
}

jcomposite_flags_t NativeGraphics::GetCompositeFlags()
{
	return _composite_flags;
}

void NativeGraphics::Clear()
{
	if (_surface == NULL) {
		return;
	}

	_surface->Clear(_surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
}

void NativeGraphics::Clear(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	IDirectFBSurface *sub;
	DFBRectangle rect;

	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;

	_surface->GetSubSurface(_surface, &rect, &sub);

	sub->Clear(sub, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
}

void NativeGraphics::Idle()
{
	IDirectFB *engine = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

	engine->WaitIdle(engine);
	engine->WaitForSync(engine);
}

void NativeGraphics::Flip()
{
	if (_surface == NULL) {
		return;
	}

	if (_vertical_sync == false) {
		_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	} else {
		// _surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_BLIT));
		// _surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
		_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
	}
}

void NativeGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int x = xp;
	int y = yp;
	int w = wp;
	int h = hp;

	DFBRegion rgn;

	rgn.x1 = x;
	rgn.y1 = y;
	rgn.x2 = x+w;
	rgn.y2 = y+h;

	if (_vertical_sync == false) {
		_surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	} else {
		// _surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_BLIT));
		// _surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
		_surface->Flip(_surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
	}
}

Color & NativeGraphics::GetColor()
{
	return _color;
} 

void NativeGraphics::SetColor(const Color &color)
{
	if (_surface == NULL) {
		return;
	}

	_color = color;

	_surface->SetColor(_surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
} 

void NativeGraphics::SetColor(uint32_t color)
{
	SetColor(_color = Color(color));
} 

void NativeGraphics::SetColor(int red, int green, int blue, int alpha)
{
	SetColor(_color = Color(red, green, blue, alpha));
} 

void NativeGraphics::SetFont(Font *font)
{
	if (_surface == NULL) {
		return;
	}

	_font = font;

	if (_font != NULL) {
		_font->ApplyContext(_surface);
	}
}

void NativeGraphics::SetAntialias(jantialias_mode_t mode)
{
}

void NativeGraphics::DrawLine(int xp, int yp, int xf, int yf)
{
	if (_surface == NULL) {
		return;
	}

	int x0 = _translate.x+xp;
	int y0 = _translate.y+yp;
	int x1 = _translate.x+xf;
	int y1 = _translate.y+yf;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

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

void NativeGraphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
	if (_surface == NULL) {
		return;
	}

	if (_pen.width == 0) {
		return;
	}

	if (npoints < 3) {
		return;
	}

	if (interpolation < 2) {
		return;
	}

	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
		line_width = -line_width;
	}

	double *x, *y, stepsize;
	int x1, y1, x2, y2; 

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

void NativeGraphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	w = (w >= 0)?w:-w;
	h = (h >= 0)?h:-h;

	_surface->FillRectangle(_surface, x, y ,w, h);
}

void NativeGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
		DrawRectangle0(x, y, w, h, 0, 0, JLJ_MITER, line_width);
	} else {
		DrawRectangle0(x-line_width+1, y-line_width+1, w+2*(line_width-1), h+2*(line_width-1), 0, 0, JLJ_MITER, -line_width);
	}
}

void NativeGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	
	DrawRectangle0(x, y, w, h, dx, dy, JLJ_BEVEL, -std::max(w, h));
}

void NativeGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
		DrawRectangle0(x, y, w, h, dx, dy, JLJ_BEVEL, line_width);
	} else {
		DrawRectangle0(x-line_width+1, y-line_width+1, w+2*(line_width-1), h+2*(line_width-1), dx+line_width, dy+line_width, JLJ_BEVEL, -line_width);
	}
}

void NativeGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	
	DrawRectangle0(x, y, w, h, dx, dy, JLJ_ROUND, -std::max(w, h));
}

void NativeGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
		DrawRectangle0(x, y, w, h, dx, dy, JLJ_ROUND, line_width);
	} else {
		DrawRectangle0(x-line_width+1, y-line_width+1, w+2*(line_width-1), h+2*(line_width-1), dx+line_width, dy+line_width, JLJ_ROUND, -line_width);
	}
}

void NativeGraphics::FillCircle(int xcp, int ycp, int rp)
{
	FillArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void NativeGraphics::DrawCircle(int xcp, int ycp, int rp)
{
	DrawArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void NativeGraphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	FillArc(xcp, ycp, rxp, ryp, 0.0, M_2PI);
}

void NativeGraphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	DrawArc(xcp, ycp, rxp, ryp, 0.0, 2*M_PI);
}

void NativeGraphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;

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

void NativeGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

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

void NativeGraphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;

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

void NativeGraphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	DrawArcHelper(xc, yc, rx, ry, arc0, arc1, line_width);
}

void NativeGraphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	FillArc(xcp, ycp, rxp, ryp, arc0, arc1);
}

void NativeGraphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

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

	jline_join_t join = _pen.join;

	_pen.join = JLJ_BEVEL;

	DrawPolygon(xcp, ycp, p, 3, false);

	_pen.join = join;
}
		
void NativeGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	if (_surface == NULL) {
		return;
	}

	int x1 = _translate.x+x1p;
	int y1 = _translate.y+y1p;
	int x2 = _translate.x+x2p;
	int y2 = _translate.y+y2p;
	int x3 = _translate.x+x3p;
	int y3 = _translate.y+y3p;

	_surface->FillTriangle(_surface, x1, y1, x2, y2, x3, y3);
}

void NativeGraphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
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

void NativeGraphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool close)
{
	if (npoints < 1) {
		return;
	}

	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

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
		jgui::jpoint_t scaled[npoints+1];
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
			if (_pen.join == JLJ_BEVEL) {
				FillTriangle(
						xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
						xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y, 
						xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
			} else if (_pen.join == JLJ_ROUND) {
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
			} else if (_pen.join == JLJ_MITER) {
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

void NativeGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
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
		points[i].x = xp+p[i].x+_translate.x;
		points[i].y = yp+p[i].y+_translate.y;

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

void NativeGraphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
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

void NativeGraphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
	if (_surface == NULL) {
		return;
	}

	if (_gradient_stops.size() == 0) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

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

void NativeGraphics::DrawString(std::string text, int xp, int yp)
{
	if (_surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;

	_surface->DrawString(_surface, text.c_str(), -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
}

void NativeGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	if (_surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	IDirectFBFont *font = dynamic_cast<NativeFont *>(_font)->_font;
	int advance;

	if (font != NULL) {
		font->GetGlyphExtents(font, symbol, NULL, &advance);
	}

	Image *off = Image::CreateImage(JPF_ARGB, advance, _font->GetAscender() + _font->GetDescender());

	off->GetGraphics()->SetFont(_font);
	off->GetGraphics()->SetColor(_color);

	IDirectFBSurface *fsurface = (IDirectFBSurface *)(off->GetGraphics()->GetNativeSurface());

	fsurface->DrawGlyph(fsurface, symbol, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	fsurface->DrawGlyph(fsurface, symbol, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

	DrawImage(off, xp, yp);

	delete off;
}

void NativeGraphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
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

uint32_t NativeGraphics::GetRGB(int xp, int yp, uint32_t safe)
{
	if (_surface == NULL) {
		return safe;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;

	int swmax,
			shmax;

	_surface->GetSize(_surface, &swmax, &shmax);

	if ((x < 0 || x >= swmax) || (y < 0 || y >= shmax)) {
		return safe;
	}

	void *ptr;
	uint32_t *dst, rgb;
	int pitch;

	_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

	dst = (uint32_t *)((uint8_t *)ptr + y * pitch);
	rgb = *(dst + x);

	_surface->Unlock(_surface);

	return rgb;
}

void NativeGraphics::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		throw jcommon::NullPointerException("Surface is null");
	}

	if (rgb == NULL) {
		throw jcommon::NullPointerException("Pixel array is null");
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int sw;
	int sh;
	
	_surface->GetSize(_surface, &sw, &sh);

	if ((x < 0 || (x+wp) > sw) || (y < 0 || (y+hp) > sh)) {
		throw jcommon::OutOfBoundsException("Index out of bounds");
	}

	void *ptr;
	int pitch;
	uint32_t *array = (*rgb);

	if (array == NULL) {
		array = new uint32_t[wp*hp];
	}

	_surface->Lock(_surface, DSLF_WRITE, &ptr, &pitch);

	if (_pixelformat == JPF_ARGB) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 4);
			uint8_t *dst = (uint8_t *)(array + j * wp);
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
	} else if (_pixelformat == JPF_RGB32) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 4);
			uint8_t *dst = (uint8_t *)(array + j * wp);
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
			uint8_t *src = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 3);
			uint8_t *dst = (uint8_t *)(array + j * wp);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				*(dst + di + 3) = 0xff;
				*(dst + di + 2) = *(src + si + 2);
				*(dst + di + 1) = *(src + si + 1);
				*(dst + di + 0) = *(src + si + 0);

				si = si + 3;
				di = di + 4;
			}
		}
	}
	
	_surface->Unlock(_surface);

	(*rgb) = array;
}

void NativeGraphics::SetRGB(uint32_t rgb, int xp, int yp) 
{
	if (_surface == NULL) {
		return;
	}

	int x = xp;
	int y = yp;

	int r = (rgb >> 0x10) & 0xff;
	int g = (rgb >> 0x08) & 0xff;
	int b = (rgb >> 0x00) & 0xff;
	int a = (rgb >> 0x18) & 0xff;

	_surface->SetColor(_surface, r, g, b, a);
	_surface->DrawLine(_surface, x, y, x, y);
}

void NativeGraphics::SetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp) 
{
	if (_surface == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int sw;
	int sh;
	
	_surface->GetSize(_surface, &sw, &sh);

	if ((x < 0 || (x+wp) > sw) || (y < 0 || (y+hp) > sh)) {
		throw jcommon::OutOfBoundsException("Index out of bounds");
	}

	void *ptr;
	int pitch;

	_surface->Lock(_surface, DSLF_WRITE, &ptr, &pitch);

	if (_pixelformat == JPF_ARGB) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 4);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				int a = *(src + si + 3);
				int r = *(src + si + 2);
				int g = *(src + si + 1);
				int b = *(src + si + 0);
				int pa = *(dst + di + 3);
				int pr = *(dst + di + 2);
				int pg = *(dst + di + 1);
				int pb = *(dst + di + 0);

				/*
				if (_premultiply == true) {
					uint32_t pixel = *(uint32_t *)(src + si);
					uint32_t pa = (pixel >> 0x18) + 1;

					pixel = ((((pixel & 0x00ff00ff) * pa) >> 8) & 0x00ff00ff) | ((((pixel & 0x0000ff00) * pa) >> 8) & 0x0000ff00) | ((((pixel & 0xff000000))));

					*(dst + di + 3) = (pixel >> 0x18) & 0xff;
					*(dst + di + 2) = (pixel >> 0x10) & 0xff;
					*(dst + di + 1) = (pixel >> 0x08) & 0xff;
					*(dst + di + 0) = (pixel >> 0x00) & 0xff;
				}
				*/

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
					pr = (int)(r + (((0xff-a)*pr) >> 0x08));
					pg = (int)(g + (((0xff-a)*pg) >> 0x08));
					pb = (int)(b + (((0xff-a)*pb) >> 0x08));
					pa = a + (((0xff-a)*pa) >> 0x08);
				} else if (_composite_flags == JCF_SRC_IN) {
					pr = (int)(r*pa) >> 0x08;
					pg = (int)(g*pa) >> 0x08;
					pb = (int)(b*pa) >> 0x08;
					pa = (a*pa) >> 0x08;
				} else if (_composite_flags == JCF_SRC_OUT) {
					pr = (int)(r*(0xff-pa)) >> 0x08;
					pg = (int)(g*(0xff-pa)) >> 0x08;
					pb = (int)(b*(0xff-pa)) >> 0x08;
					pa = (a*(0xff-pa)) >> 0x08;
				} else if (_composite_flags == JCF_SRC_ATOP) {
					pr = (int)(r*pa + (0xff-a)*pr) >> 0x08;
					pg = (int)(g*pa + (0xff-a)*pg) >> 0x08;
					pb = (int)(b*pa + (0xff-a)*pb) >> 0x08;
					pa = pa;
				} else if (_composite_flags == JCF_DST) {
					pr = pr;
					pg = pg;
					pb = pb;
					pa = pa;
				} else if (_composite_flags == JCF_DST_OVER) {
					pr = (int)(pr + (((0xff-pa)*r) >> 0x08));
					pg = (int)(pg + (((0xff-pa)*g) >> 0x08));
					pb = (int)(pb + (((0xff-pa)*b) >> 0x08));
					pa = a + (((0xff-a)*pa) >> 0x08);
				} else if (_composite_flags == JCF_DST_IN) {
					pr = (int)(pr*a) >> 0x08;
					pg = (int)(pg*a) >> 0x08;
					pb = (int)(pb*a) >> 0x08;
					pa = (a*pa) >> 0x08;
				} else if (_composite_flags == JCF_DST_OUT) {
					pr = (int)(pr*(0xff-a)) >> 0x08;
					pg = (int)(pg*(0xff-a)) >> 0x08;
					pb = (int)(pb*(0xff-a)) >> 0x08;
					pa = (pa*(0xff-a)) >> 0x08;
				} else if (_composite_flags == JCF_DST_ATOP) {
					pr = (int)(pr*a + (0xff-pa)*r) >> 0x08;
					pg = (int)(pg*a + (0xff-pa)*g) >> 0x08;
					pb = (int)(pb*a + (0xff-pa)*b) >> 0x08;
					pa = a;
				} else if (_composite_flags == JCF_XOR) {
					pr = (int)(r*(0xff-pa) + (0xff-a)*pr) >> 0x08;
					pg = (int)(g*(0xff-pa) + (0xff-a)*pg) >> 0x08;
					pb = (int)(b*(0xff-pa) + (0xff-a)*pb) >> 0x08;
					pa = a + pa - ((2*a*pa) >> 0x08);
				} else if (_composite_flags == JCF_ADD) {
					pr = r + pr;
					pg = g + pg;
					pb = b + pb;
					pa = a + pa;

					pr = (pr > 0xff)?0xff:pr;
					pg = (pg > 0xff)?0xff:pg;
					pb = (pb > 0xff)?0xff:pb;
					pa = (pa > 0xff)?0xff:pa;
				} else if (_composite_flags == JCF_MULTIPLY) {
					/*
					pr = (int)((r*pr) >> 0x08);
					pg = (int)((g*pg) >> 0x08);
					pb = (int)((b*pb) >> 0x08);
					pa = (a*pa) >> 0x08;
					*/
				} else if (_composite_flags == JCF_SCREEN) {
					pr = (int)(r+pr - ((r*pr) >> 0x08));
					pg = (int)(g+pg - ((g*pg) >> 0x08));
					pb = (int)(b+pb - ((b*pb) >> 0x08));
					pa = a+pa - ((a*pa) >> 0x08);
				}

				/*
				pr = (pr > 0xff)?0xff:pr;
				pg = (pg > 0xff)?0xff:pg;
				pb = (pb > 0xff)?0xff:pb;
				pa = (pa > 0xff)?0xff:pa;
				*/

				*(dst + di + 3) = pa;
				*(dst + di + 2) = pr;
				*(dst + di + 1) = pg;
				*(dst + di + 0) = pb;

				si = si + 4;
				di = di + 4;
			}
		}
	} else if (_pixelformat == JPF_RGB32) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 4);
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
			uint8_t *dst = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 3);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				*(dst + di + 2) = *(src + si + 2);
				*(dst + di + 1) = *(src + si + 1);
				*(dst + di + 0) = *(src + si + 0);

				si = si + 4;
				di = di + 3;
			}
		}
	} else if (_pixelformat == JPF_RGB16) {
		for (int j=0; j<hp; j++) {
			uint8_t *src = (uint8_t *)(rgb + j * wp);
			uint8_t *dst = (uint8_t *)((uint8_t *)ptr + (y + j) * pitch + x * 2);
			int si = 0;
			int di = 0;

			for (int i=0; i<wp; i++) {
				int r = *(src + si + 2);
				int g = *(src + si + 1);
				int b = *(src + si + 0);

				*(dst + di + 1) = (r << 0x03 | g >> 0x03) & 0xff;
				*(dst + di + 0) = (g << 0x03 | b >> 0x00) & 0xff;
			}
		}
	}

	_surface->Unlock(_surface);
}

bool NativeGraphics::DrawImage(Image *img, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp);
}

bool NativeGraphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool NativeGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	jgui::Image *aux = img->Crop(sxp, syp, swp, shp);

	if (aux == NULL) {
		return false;
	}

	NativeGraphics *g = dynamic_cast<NativeGraphics *>(aux->GetGraphics());

	if (g != NULL) {
		IDirectFBSurface *dfb_surface = (IDirectFBSurface *)g->GetNativeSurface();

		if (dfb_surface == NULL) {
			return false;
		}

		DFBSurfaceBlittingFlags blit = DSBLIT_BLEND_ALPHACHANNEL;

		if (_premultiply == true && g->_premultiply == false) {
			blit = (DFBSurfaceBlittingFlags)(blit | DSBLIT_DEMULTIPLY);
		}

		// _surface->SetBlittingFlags(_surface, blit);

		_surface->Blit(_surface, dfb_surface, NULL, xp+_translate.x, yp+_translate.y);
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

bool NativeGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
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

	NativeGraphics *g = dynamic_cast<NativeGraphics *>(scl->GetGraphics());

	if (g != NULL) {
		IDirectFBSurface *dfb_surface = (IDirectFBSurface *)g->GetNativeSurface();

		if (dfb_surface == NULL) {
			return false;
		}

		DFBSurfaceBlittingFlags blit = DSBLIT_BLEND_ALPHACHANNEL;

		if (_premultiply == true && g->_premultiply == false) {
			blit = (DFBSurfaceBlittingFlags)(blit | DSBLIT_DEMULTIPLY);
		}

		// _surface->SetBlittingFlags(_surface, blit);

		_surface->Blit(_surface, dfb_surface, NULL, xp+_translate.x, yp+_translate.y);
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

void NativeGraphics::Reset()
{
	SetAntialias(JAM_NORMAL);

	SetColor(0x00000000);

	_pen.dashes = NULL;
	_pen.dashes_size = 0;
	_pen.width = 1;
	_pen.join = JLJ_MITER;
	_pen.style = JLS_BUTT;

	SetPen(_pen);
	ResetGradientStop();
	SetCompositeFlags(JCF_SRC_OVER);
}

void NativeGraphics::ApplyDrawing()
{
}

double NativeGraphics::EvaluateBezier0(double *data, int ndata, double t) 
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

int NativeGraphics::CalculateGradientChannel(int schannel, int dchannel, int distance, int offset) 
{
	if (schannel == dchannel) {
		return schannel;
	}

	return (int)(schannel-((schannel-dchannel)*((double)offset/(double)distance))) & 0xff;
}

void NativeGraphics::UpdateGradientColor(Color &scolor, Color &dcolor, int distance, int offset) 
{
	int a = CalculateGradientChannel(scolor.GetAlpha(), dcolor.GetAlpha(), distance, offset);
	int r = CalculateGradientChannel(scolor.GetRed(), dcolor.GetRed(), distance, offset);
	int g = CalculateGradientChannel(scolor.GetGreen(), dcolor.GetGreen(), distance, offset);
	int b = CalculateGradientChannel(scolor.GetBlue(), dcolor.GetBlue(), distance, offset);
	
	SetColor((a << 24) | (r << 16) | (g << 8) | (b << 0));
}

void NativeGraphics::FillPolygon0(jgui::jpoint_t *points, int npoints, int x1p, int y1p, int x2p, int y2p)
{
	if (_surface == NULL) {
		return;
	}

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

void NativeGraphics::DrawRectangle0(int x, int y, int w, int h, int dx, int dy, jline_join_t join, int size)
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

void NativeGraphics::DrawArc0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size, int quadrant)
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

void NativeGraphics::DrawArcHelper(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
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

void NativeGraphics::DrawChord0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
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

}
