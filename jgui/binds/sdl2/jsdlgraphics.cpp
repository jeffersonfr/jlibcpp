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
#include "jcharset.h"
#include "jsdlfont.h"
#include "jsdlgraphics.h"
#include "jsdlimage.h"
#include "joutofboundsexception.h"
#include "jnullpointerexception.h"

#define M_2PI	(2*M_PI)

namespace jgui {

SDLGraphics::SDLGraphics(void *surface, jpixelformat_t pixelformat, int wp, int hp):
	jgui::Graphics()
{
	jcommon::Object::SetClassName("jgui::SDLGraphics");

	_pixelformat = pixelformat;
	_clip.x = 0;
	_clip.y = 0;
	_clip.width = wp;
	_clip.height = hp;

	_surface = (SDL_Renderer *)surface;

	Reset();
}

SDLGraphics::~SDLGraphics()
{
}

void * SDLGraphics::GetNativeSurface()
{
	return NULL;
}

void SDLGraphics::SetNativeSurface(void *data, int wp, int hp)
{
}

void SDLGraphics::Dump(std::string dir, std::string prefix)
{
}

jregion_t SDLGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, _internal_clip.x, _internal_clip.y, _internal_clip.width, _internal_clip.height);
	
	SetClip(clip.x-_translate.x, clip.y-_translate.y, clip.width, clip.height);

	return clip;
}

void SDLGraphics::SetClip(int xp, int yp, int wp, int hp)
{
	/*
	int w;
	int h;

	_surface->GetSize(_surface, &w, &h);

	jregion_t clip = Rectangle::Intersection(xp+_translate.x, yp+_translate.y, wp, hp, 0, 0, w, h);
	
	_clip.x = clip.x - _translate.x;
	_clip.y = clip.y - _translate.y;
	_clip.width = clip.width;
	_clip.height = clip.height;
	
	_internal_clip = clip;
	*/
}

jregion_t SDLGraphics::GetClip()
{
	return _clip;
}

void SDLGraphics::ReleaseClip()
{
	/*
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
	*/
}

void SDLGraphics::SetCompositeFlags(jcomposite_flags_t t)
{
	_composite_flags = t;

	if (_composite_flags == JCF_SRC) {
	} else if (_composite_flags == JCF_SRC_OVER) {
	} else if (_composite_flags == JCF_SRC_IN) {
	} else if (_composite_flags == JCF_SRC_OUT) {
	} else if (_composite_flags == JCF_SRC_ATOP) {
	} else if (_composite_flags == JCF_DST) {
	} else if (_composite_flags == JCF_DST_OVER) {
	} else if (_composite_flags == JCF_DST_IN) {
	} else if (_composite_flags == JCF_DST_OUT) {
	} else if (_composite_flags == JCF_DST_ATOP) {
	} else if (_composite_flags == JCF_ADD) {
	} else if (_composite_flags == JCF_XOR) {
	}
}

jcomposite_flags_t SDLGraphics::GetCompositeFlags()
{
	return _composite_flags;
}

void SDLGraphics::Clear()
{
	SDL_SetRenderDrawColor(_surface, 0x00, 0x00, 0x00, 0x00);
	SDL_RenderClear(_surface);  
}

void SDLGraphics::Clear(int xp, int yp, int wp, int hp)
{
}

void SDLGraphics::Idle()
{
}

void SDLGraphics::Flip()
{
	if (_vertical_sync == false) {
		SDL_RenderPresent(_surface);  
	} else {
		SDL_RenderPresent(_surface);  
	}
}

void SDLGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (_vertical_sync == false) {
		SDL_RenderPresent(_surface);  
	} else {
		SDL_RenderPresent(_surface);  
	}
}

Color & SDLGraphics::GetColor()
{
	return _color;
} 

void SDLGraphics::SetColor(const Color &color)
{
	_color = color;

	SDL_SetRenderDrawColor(_surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
} 

void SDLGraphics::SetColor(uint32_t color)
{
	SetColor(_color = Color(color));
} 

void SDLGraphics::SetColor(int red, int green, int blue, int alpha)
{
	SetColor(_color = Color(red, green, blue, alpha));
} 

void SDLGraphics::SetFont(Font *font)
{
	_font = font;
}

void SDLGraphics::DrawLine(int xp, int yp, int xf, int yf)
{
	int x0 = _translate.x+xp;
	int y0 = _translate.y+yp;
	int x1 = _translate.x+xf;
	int y1 = _translate.y+yf;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
		line_width = -line_width;
	}
	
	SDL_RenderDrawLine(_surface, x0, y0, x1, y1);  
}

void SDLGraphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
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
	
		SDL_RenderDrawLine(_surface, x1, y1, x2, y2);  
		
		x1 = x2;
		y1 = y2;
	}
    
	delete [] x;
	delete [] y;
}

void SDLGraphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;

	w = (w >= 0)?w:-w;
	h = (h >= 0)?h:-h;

	SDL_Rect t;

	t.x = x;
	t.y = y;
	t.w = w;
	t.h = h;

	SDL_RenderFillRect(_surface, &t);  
}

void SDLGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
	} else {
	}

	SDL_Rect t;

	t.x = x;
	t.y = y;
	t.w = w;
	t.h = h;

	SDL_RenderDrawRect(_surface, &t);  
}

void SDLGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	
	DrawRectangle0(x, y, w, h, dx, dy, JLJ_BEVEL, -std::max(w, h));
}

void SDLGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	/*
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
	} else {
	}
	*/
}

void SDLGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	
	DrawRectangle0(x, y, w, h, dx, dy, JLJ_ROUND, -std::max(w, h));
}

void SDLGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	/*
	int x = _translate.x+xp;
	int y = _translate.y+yp;
	int w = wp;
	int h = hp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	if (line_width < 0) {
	} else {
	}
	*/
}

void SDLGraphics::FillCircle(int xcp, int ycp, int rp)
{
	FillArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void SDLGraphics::DrawCircle(int xcp, int ycp, int rp)
{
	DrawArc(xcp, ycp, rp, rp, 0.0, 2*M_PI);
}

void SDLGraphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	FillArc(xcp, ycp, rxp, ryp, 0.0, M_2PI);
}

void SDLGraphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	DrawArc(xcp, ycp, rxp, ryp, 0.0, 2*M_PI);
}

void SDLGraphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
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

void SDLGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
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

void SDLGraphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
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

void SDLGraphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	int xc = _translate.x+xcp;
	int yc = _translate.y+ycp;
	int rx = rxp;
	int ry = ryp;
	int lw = _pen.width;

	int line_width = (lw != 0)?lw:(_pen.width != 0)?1:0;

	DrawArcHelper(xc, yc, rx, ry, arc0, arc1, line_width);
}

void SDLGraphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	FillArc(xcp, ycp, rxp, ryp, arc0, arc1);
}

void SDLGraphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
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
		
void SDLGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	jpoint_t p[3];

	p[0].x = x1p;
	p[0].y = y1p;
	p[1].x = x2p;
	p[1].y = y2p;
	p[2].x = x3p;
	p[2].y = y3p;

	jline_join_t t = _pen.join;

	_pen.join = JLJ_MITER;

	FillPolygon(0, 0, p, 3, false);
	
	_pen.join = t;
}

void SDLGraphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	jpoint_t p[3];

	p[0].x = x1p;
	p[0].y = y1p;
	p[1].x = x2p;
	p[1].y = y2p;
	p[2].x = x3p;
	p[2].y = y3p;

	jline_join_t t = _pen.join;

	_pen.join = JLJ_MITER;

	DrawPolygon(0, 0, p, 3, true);
	
	_pen.join = t;
}

void SDLGraphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool close)
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

void SDLGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
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

void SDLGraphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
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

void SDLGraphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
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
				SDL_RenderDrawLine(_surface, x+i, y, x+i, y+h-1);  
			}
		} else {
			end = (int)(h*t.stop);

			for (int i=start; i<end; i++) {
				UpdateGradientColor(color0, color1, abs(end-start), i);
				SDL_RenderDrawLine(_surface, x, y+i, x+w-1, y+i);  
			}
		}

		color0 = color1;
		start = end;
	}

	SetColor(color);
}

void SDLGraphics::DrawString(std::string text, int xp, int yp)
{
	if (_font == NULL) {
		return;
	}

	int x = _translate.x+xp;
	int y = _translate.y+yp;

	TTF_Font *font = (TTF_Font *)_font->GetNativeFont();
	SDL_Surface *surface = NULL;
	SDL_Texture *texture = NULL;
	SDL_Color color = {
		_color.GetRed(),
		_color.GetGreen(),
		_color.GetBlue(),
		_color.GetAlpha()
	};

	if (_antialias == JAM_NONE) {
		surface = TTF_RenderText_Solid(font, text.c_str(), color);
	} else {
		surface = TTF_RenderText_Blended(font, text.c_str(), color);
	}
	
	if (surface != NULL) {
		SDL_Rect dst;

		dst.x = x;
		dst.y = y;
		dst.w = surface->w;
		dst.h = surface->h;

		texture = SDL_CreateTextureFromSurface(_surface, surface);

		if (texture != NULL) {
			SDL_RenderCopy(_surface, texture, NULL, &dst);
			SDL_DestroyTexture(texture);
		}

		SDL_FreeSurface(surface);
	}
}

void SDLGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	if (_font == NULL) {
		return;
	}

}

void SDLGraphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
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

uint32_t SDLGraphics::GetRGB(int xp, int yp, uint32_t safe)
{
	return safe;
}

void SDLGraphics::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
}

void SDLGraphics::SetRGB(uint32_t rgb, int xp, int yp) 
{
	int r = (rgb >> 0x10) & 0xff;
	int g = (rgb >> 0x08) & 0xff;
	int b = (rgb >> 0x00) & 0xff;
	int a = (rgb >> 0x18) & 0xff;

	SDL_SetRenderDrawColor(_surface, r, g, b, a);
	SDL_RenderDrawPoint(_surface, xp+_translate.x, yp+_translate.y);
}

void SDLGraphics::SetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp) 
{
}

bool SDLGraphics::DrawImage(Image *img, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp);
}

bool SDLGraphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool SDLGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	jgui::Image *aux = img->Crop(sxp, syp, swp, shp);

	if (aux == NULL) {
		return false;
	}

	SDLGraphics *g = dynamic_cast<SDLGraphics *>(aux->GetGraphics());

	if (g != NULL) {
		/*
		IDirectFBSurface *dfb_surface = (IDirectFBSurface *)g->GetNativeSurface();

		if (dfb_surface == NULL) {
			return false;
		}

		SDLSurfaceBlittingFlags blit = DSBLIT_BLEND_ALPHACHANNEL;

		if (_premultiply == true && g->_premultiply == false) {
			blit = (SDLSurfaceBlittingFlags)(blit | DSBLIT_DEMULTIPLY);
		}

		// _surface->SetBlittingFlags(_surface, blit);

		_surface->Blit(_surface, dfb_surface, NULL, xp+_translate.x, yp+_translate.y);
		*/
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

bool SDLGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
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

	SDLGraphics *g = dynamic_cast<SDLGraphics *>(scl->GetGraphics());

	if (g != NULL) {
		/*
		IDirectFBSurface *dfb_surface = (IDirectFBSurface *)g->GetNativeSurface();

		if (dfb_surface == NULL) {
			return false;
		}

		SDLSurfaceBlittingFlags blit = DSBLIT_BLEND_ALPHACHANNEL;

		if (_premultiply == true && g->_premultiply == false) {
			blit = (SDLSurfaceBlittingFlags)(blit | DSBLIT_DEMULTIPLY);
		}

		// _surface->SetBlittingFlags(_surface, blit);

		_surface->Blit(_surface, dfb_surface, NULL, xp+_translate.x, yp+_translate.y);
		*/
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

void SDLGraphics::Reset()
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

void SDLGraphics::ApplyDrawing()
{
}

double SDLGraphics::EvaluateBezier0(double *data, int ndata, double t) 
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

int SDLGraphics::CalculateGradientChannel(int schannel, int dchannel, int distance, int offset) 
{
	if (schannel == dchannel) {
		return schannel;
	}

	return (int)(schannel-((schannel-dchannel)*((double)offset/(double)distance))) & 0xff;
}

void SDLGraphics::UpdateGradientColor(Color &scolor, Color &dcolor, int distance, int offset) 
{
	int a = CalculateGradientChannel(scolor.GetAlpha(), dcolor.GetAlpha(), distance, offset);
	int r = CalculateGradientChannel(scolor.GetRed(), dcolor.GetRed(), distance, offset);
	int g = CalculateGradientChannel(scolor.GetGreen(), dcolor.GetGreen(), distance, offset);
	int b = CalculateGradientChannel(scolor.GetBlue(), dcolor.GetBlue(), distance, offset);
	
	SetColor((a << 24) | (r << 16) | (g << 8) | (b << 0));
}

void SDLGraphics::FillPolygon0(jgui::jpoint_t *points, int npoints, int x1p, int y1p, int x2p, int y2p)
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
				// SDL_RenderDrawLine(_surface, x, y, x, y);  
				SDL_RenderDrawPoint(_surface, x, y);
			}
		}
	}
}

void NativeFillRectangle(SDL_Renderer *surface, int x, int y, int w, int h)
{
	SDL_Rect t;

	t.x = x;
	t.y = y;
	t.w = w;
	t.h = h;

	SDL_RenderFillRect(surface, &t);  
}

void SDLGraphics::DrawRectangle0(int x, int y, int w, int h, int dx, int dy, jline_join_t join, int size)
{ 
	if (size >= 0 || w <= 0 || h <= 0) {
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

		NativeFillRectangle(_surface, x-size, y-size, w+2*size, size);
		NativeFillRectangle(_surface, x-size, y+h, w+2*size, size); // _
		NativeFillRectangle(_surface, x-size, y, size, h); // |<- 
		NativeFillRectangle(_surface, x+w, y, size, h); // ->|

		if (close == true) {
			NativeFillRectangle(_surface, x, y, w, h);
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
				SDL_RenderDrawLine(_surface, x+dx, y, x, y+dy);  
				SDL_RenderDrawLine(_surface, x+w-dx-1, y, x+w-1, y+dy);  
				SDL_RenderDrawLine(_surface, x+w-1, y+h-dy, x+w-dx-1, y+h);  
				SDL_RenderDrawLine(_surface, x, y+h-dy, x+dx, y+h);  
			} else {
				FillTriangle(x+dx-_translate.x, y-_translate.y, x-_translate.x, y+dy-_translate.y, x+size-_translate.x, y+dy-_translate.y);
				FillTriangle(x+size, y+dy-_translate.y, x+dx-_translate.x, y-_translate.y, x+dx-_translate.x, y+size-_translate.y);

				FillTriangle(x+w-dx-_translate.x, y-_translate.y, x+w-dx-_translate.x, y+size-_translate.y, x+w-_translate.x, y+dy-_translate.y);
				FillTriangle(x+w-dx-_translate.x, y+size-_translate.y, x+w-size-_translate.x, y+dy-_translate.y,  x+w-_translate.x, y+dy-_translate.y);

				FillTriangle(x+w-size-_translate.x, y+h-dy-_translate.y, x+w-_translate.x, y+h-dy-_translate.y, x+w-dx-_translate.x, y+h-_translate.y);
				FillTriangle(x+w-size-_translate.x, y+h-dy-_translate.y, x+w-dx-_translate.x, y+h-_translate.y, x+w-dx-_translate.x, y+h-size-_translate.y);

				FillTriangle(x-_translate.x, y+h-dy-_translate.y, x+size-_translate.x, y+h-dy-_translate.y, x+dx-_translate.x, y+h-_translate.y);
				FillTriangle(x+size-_translate.x, y+h-dy-_translate.y, x+dx-_translate.x, y+h-size-_translate.y, x+dx-_translate.x, y+h-_translate.y);
			} 

			NativeFillRectangle(_surface, x+dx, y, w-2*dx, size);
			NativeFillRectangle(_surface, x+dx, y+h-size, w-2*dx, size);
			NativeFillRectangle(_surface, x, y+dy, size, h-2*dy);
			NativeFillRectangle(_surface, x+w-size, y+dy, size, h-2*dy);
		} else {
			FillTriangle(x+dx-_translate.x, y-_translate.y, x-_translate.x, y+dy-_translate.y, x+dx-_translate.x, y+dy-_translate.y);
			FillTriangle(x+w-dx-_translate.x, y-_translate.y, x+w-_translate.x, y+dy-_translate.y, x+w-dx-_translate.x, y+dy-_translate.y);
			FillTriangle(x-_translate.x, y+h-dy-_translate.y, x+dx-_translate.x, y+h-dy-_translate.y, x+dx-_translate.x, y+h-_translate.y);
			FillTriangle(x+w-dx-_translate.x, y+h-dy-_translate.y, x+w-_translate.x, y+h-dy-_translate.y, x+w-dx-_translate.x, y+h-_translate.y);

			NativeFillRectangle(_surface, x+dx, y, w-2*dx, dy);
			NativeFillRectangle(_surface, x+dx, y+h-dy, w-2*dx, dy);
			NativeFillRectangle(_surface, x, y+dy, dx, h-2*dy);
			NativeFillRectangle(_surface, x+w-dx, y+dy, dx, h-2*dy);

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
			NativeFillRectangle(_surface, x+dx, y, w-2*dx, size);
			NativeFillRectangle(_surface, x+dx, y+h-size, w-2*dx, size);
			NativeFillRectangle(_surface, x, y+dy, size, h-2*dy);
			NativeFillRectangle(_surface, x+w-size, y+dy, size, h-2*dy);
		} else {
			NativeFillRectangle(_surface, x+dx, y, w-2*dx, dy);
			NativeFillRectangle(_surface, x+dx, y+h-dy, w-2*dx, dy);
			NativeFillRectangle(_surface, x, y+dy, dx, h-2*dy);
			NativeFillRectangle(_surface, x+w-dx, y+dy, dx, h-2*dy);

			DrawRectangle0(x+dx, y+dy, w-2*dx, h-2*dy, 0, 0, JLJ_MITER, std::max(dx, dy)-size-1);
		}
	}
}

void SDLGraphics::DrawArc0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size, int quadrant)
{
	if (size == 0) {
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
	int old_x = -1,
			old_y = -1;
	jline_t line;

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
				line.x0 = (int)(xc+old_x);
				line.y0 = (int)(yc-y);
				line.x1 = (int)(xc+xi);
				line.y1 = (int)(yc-y);
			} else if (quadrant == 1) {
				line.x0 = (int)(xc-old_x);
				line.y0 = (int)(yc-y);
				line.x1 = (int)(xc-xi+1);
				line.y1 = (int)(yc-y);
			} else if (quadrant == 2) {
				line.x0 = (int)(xc-old_x);
				line.y0 = (int)(yc+y+1);
				line.x1 = (int)(xc-xi+1);
				line.y1 = (int)(yc+y+1);
			} else if (quadrant == 3) {
				line.x0 = (int)(xc+old_x);
				line.y0 = (int)(yc+y+1);
				line.x1 = (int)(xc+xi);
				line.y1 = (int)(yc+y+1);
			}

			old_x = xi;
			old_y = y;
		} else {
			if (quadrant == 0) {
				line.x0 = (int)(xc+xi);
				line.y0 = (int)(yc-y);
				line.x1 = (int)(xc+xf);
				line.y1 = (int)(yc-y);
			} else if (quadrant == 1) {
				line.x0 = (int)(xc-xf);
				line.y0 = (int)(yc-y);
				line.x1 = (int)(xc-xi+1);
				line.y1 = (int)(yc-y);
			} else if (quadrant == 2) {
				line.x0 = (int)(xc-xf);
				line.y0 = (int)(yc+y+1);
				line.x1 = (int)(xc-xi+1);
				line.y1 = (int)(yc+y+1);
			} else { // if (quadrant == 3) {
				line.x0 = (int)(xc+xi);
				line.y0 = (int)(yc+y+1);
				line.x1 = (int)(xc+xf);
				line.y1 = (int)(yc+y+1);
			}

			SDL_RenderDrawLine(_surface, line.x0, line.y0, line.x1, line.y1);
		}
	}
}

void SDLGraphics::DrawArcHelper(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
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

void SDLGraphics::DrawChord0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
{
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
	jline_t line;

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
							line.x0 = (int)(xc+xi);
							line.y0 = (int)(yc+y);
							line.x1 = (int)(xc+x);
							line.y1 = (int)(yc+y);

							SDL_RenderDrawLine(_surface, line.x0, line.y0, line.x1, line.y1);

							flag = false;
						}
					}
				}
			}
		}

		if (flag == true) {
			line.x0 = (int)(xc+xi);
			line.y0 = (int)(yc+y);
			line.x1 = (int)(xc+xf);
			line.y1 = (int)(yc+y);

			SDL_RenderDrawLine(_surface, line.x0, line.y0, line.x1, line.y1);
		}
	}
}

}

