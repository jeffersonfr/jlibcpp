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
#include "jgraphics.h"
#include "jmath.h"
#include "jstringtokenizer.h"
#include "joffscreenimage.h"
#include "jgfxhandler.h"
#include "jfont.h"
#include "jstringutils.h"

namespace jgui {

static const float EPSILON = 0.0000000001f;

class Vector2d {

	private:
		float mX;
		float mY;

	public:
		Vector2d(float x,float y);

		float GetX(void) const;
		float GetY(void) const;
		void  Set(float x,float y);

};

// Vector of vertices which are used to represent a polygon/contour and a series of triangles
typedef std::vector< Vector2d > Vector2dVector;

class Triangulate {
	private:
		static bool Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V);

	public:
  		// triangulate a contour/polygon, places results in STL vector as series of triangles
		static bool Process(const Vector2dVector &contour, Vector2dVector &result);
		// compute area of a contour/polygon
		static float Area(const Vector2dVector &contour);
		// decide if point Px/Py is inside triangle defined by (Ax,Ay) (Bx,By) (Cx,Cy)
		static bool InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py);

};

// Vector of vertices which are used to represent a polygon/contour and a series of triangles
typedef std::vector< Vector2d > Vector2dVector;

Graphics::Graphics(void *s, bool premultiplied):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Graphics");

	_is_premultiply = premultiplied;
	_antialias_enabled = false;

	_radians = 0.0;
	_translate.x = 0;
	_translate.y = 0;
	_translate_image.x = 0;
	_translate_image.y = 0;

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;

#ifdef DIRECTFB_UI
	surface = (IDirectFBSurface *)s;
#endif

	_clip.x = 0;
	_clip.y = 0;
	_clip.width = DEFAULT_SCALE_WIDTH;
	_clip.height = DEFAULT_SCALE_HEIGHT;

	_font = NULL;

	_line_join = BEVEL_JOIN;
	_line_style = SOLID_LINE;
	_line_width = 1;

	SetDrawingFlags(DF_BLEND);
	SetBlittingFlags(BF_ALPHACHANNEL);
	SetPorterDuffFlags(PDF_SRC_OVER);
}

Graphics::~Graphics()
{
}

bool Graphics::GetImageSize(std::string img, int *width, int *height)
{
	if (width != NULL) {
		*width = -1;
	}

	if (height != NULL) {
		*height = -1;
	}

#ifdef DIRECTFB_UI
	GFXHandler *dfb = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)dfb->GetGraphicEngine();

	IDirectFBImageProvider *provider = NULL;
	DFBSurfaceDescription desc;

	if (engine->CreateImageProvider(engine, img.c_str(), &provider) != DFB_OK) {
		return false;
	}

	if (provider->GetSurfaceDescription(provider, &desc) == DFB_OK) {
		provider->Release(provider);

		if (width != NULL) {
			*width = desc.width;
		}

		if (height != NULL) {
			*height = desc.height;
		}

		return true;
	}

	provider->Release(provider);
#endif
	
	return false;
}

OffScreenImage * Graphics::Create()
{
	OffScreenImage *image = NULL;

#ifdef DIRECTFB_UI
	if (surface != NULL) {
		int w,
				h;

		surface->GetSize(surface, &w, &h);

		w = SCREEN_TO_SCALE(w, _screen.width, _scale.width);
		h = SCREEN_TO_SCALE(h, _screen.height, _scale.height);

		image = new OffScreenImage(w, h);

		IDirectFBSurface *s = image->GetGraphics()->surface;

		s->Blit(s, surface, NULL, 0, 0);
		s->Flip(s, NULL, DSFLIP_NONE);
	}
#endif

	return image;
}

void * Graphics::GetNativeSurface()
{
#ifdef DIRECTFB_UI
	return surface;
#endif

	return NULL;
}

void Graphics::SetNativeSurface(void *addr)
{
	Lock();

#ifdef DIRECTFB_UI
	surface = (IDirectFBSurface *)addr;
#endif

	Unlock();
}

void Graphics::SetClip(int x, int y, int width, int height)
{
	_clip.x = (x < 0)?0:x;
	_clip.y = (y < 0)?0:y;
	_clip.width = (width < 0)?0:width;
	_clip.height = (height < 0)?0:height;

#ifdef DIRECTFB_UI
	if (surface != NULL) {
		DFBRegion rgn;

		rgn.x1 = SCALE_TO_SCREEN((_translate.x+_clip.x), _screen.width, _scale.width);
		rgn.y1 = SCALE_TO_SCREEN((_translate.y+_clip.y), _screen.height, _scale.height);
		rgn.x2 = SCALE_TO_SCREEN((_translate.x+_clip.x+_clip.width), _screen.width, _scale.width);
		rgn.y2 = SCALE_TO_SCREEN((_translate.y+_clip.y+_clip.height), _screen.height, _scale.height);

		surface->SetClip(surface, NULL);
		surface->SetClip(surface, &rgn);
	}
#endif
}

jregion_t Graphics::GetClip()
{
	return _clip;
}

void Graphics::ReleaseClip()
{
	_clip.x = 0;
	_clip.y = 0;
	_clip.width = _scale.width;
	_clip.height = _scale.height;

#ifdef DIRECTFB_UI
	DFBRegion rgn;

	if (surface != NULL) {
		surface->SetClip(surface, NULL);
		surface->GetClip(surface, &rgn);

		_clip.x = 0;
		_clip.y = 0;
		_clip.width = SCREEN_TO_SCALE(rgn.x2, _screen.width, _scale.width);
		_clip.height = SCREEN_TO_SCALE(rgn.y2, _screen.height, _scale.height);
	}
#endif
}

void Graphics::SetPorterDuffFlags(jporter_duff_flags_t t)
{
	_porter_duff_flags = t;

#ifdef DIRECTFB_UI
	if (surface != NULL) {
		if (_porter_duff_flags == PDF_NONE) {
			surface->SetPorterDuff(surface, DSPD_NONE);
		} else if (_porter_duff_flags == PDF_CLEAR) {
			surface->SetPorterDuff(surface, DSPD_CLEAR);
		} else if (_porter_duff_flags == PDF_SRC) {
			surface->SetPorterDuff(surface, DSPD_SRC);
		} else if (_porter_duff_flags == PDF_SRC_OVER) {
			surface->SetPorterDuff(surface, DSPD_SRC_OVER);
		} else if (_porter_duff_flags == PDF_DST_OVER) {
			surface->SetPorterDuff(surface, DSPD_DST_OVER);
		} else if (_porter_duff_flags == PDF_SRC_IN) {
			surface->SetPorterDuff(surface, DSPD_SRC_IN);
		} else if (_porter_duff_flags == PDF_DST_IN) {
			surface->SetPorterDuff(surface, DSPD_DST_IN);
		} else if (_porter_duff_flags == PDF_SRC_OUT) {
			surface->SetPorterDuff(surface, DSPD_SRC_OUT);
		} else if (_porter_duff_flags == PDF_DST_OUT) {
			surface->SetPorterDuff(surface, DSPD_DST_OUT);
		} else if (_porter_duff_flags == PDF_SRC_ATOP) {
			surface->SetPorterDuff(surface, DSPD_SRC_ATOP);
		} else if (_porter_duff_flags == PDF_DST_ATOP) {
			surface->SetPorterDuff(surface, DSPD_DST_ATOP);
		} else if (_porter_duff_flags == PDF_ADD) {
			surface->SetPorterDuff(surface, DSPD_ADD);
		} else if (_porter_duff_flags == PDF_XOR) {
			surface->SetPorterDuff(surface, DSPD_XOR);
		}
	}
#endif
}

void Graphics::SetDrawingFlags(jdrawing_flags_t t)
{
	_draw_flags = t;

#ifdef DIRECTFB_UI
	if (surface != NULL) {
		DFBSurfaceDrawingFlags flags = (DFBSurfaceDrawingFlags)DSDRAW_NOFX;

		if (_draw_flags == DF_BLEND) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_BLEND);
		} else if (_draw_flags == DF_XOR) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_XOR);
		}
		
		if (_is_premultiply == false) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_SRC_PREMULTIPLY);
		}

		surface->SetDrawingFlags(surface, (DFBSurfaceDrawingFlags)flags);
	}
#endif
}

void Graphics::SetBlittingFlags(jblitting_flags_t t)
{
	_blit_flags = t;

#ifdef DIRECTFB_UI
	if (surface != NULL) {
		DFBSurfaceBlittingFlags flags = (DFBSurfaceBlittingFlags)DSBLIT_NOFX;

		if (_blit_flags & BF_ALPHACHANNEL) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_BLEND_ALPHACHANNEL);
		}

		if (_blit_flags & BF_COLORALPHA) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_BLEND_COLORALPHA);
		} 

		if (_blit_flags & BF_COLORIZE) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_COLORIZE);
		}

		if (_blit_flags & BF_XOR) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_XOR);
		}

		if (_is_premultiply == false) {
			flags = (DFBSurfaceBlittingFlags)(flags | DSBLIT_SRC_PREMULTIPLY);
		}

		surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)flags);
	}
#endif
}

jporter_duff_flags_t Graphics::GetPorterDuffFlags()
{
	return _porter_duff_flags;
}

jdrawing_flags_t Graphics::GetDrawingFlags()
{
	return _draw_flags;
}

jblitting_flags_t Graphics::GetBlittingFlags()
{
	return _blit_flags;
}

void Graphics::SetWorkingScreenSize(int width, int height)
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

void Graphics::Clear(int red, int green, int blue, int alpha)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->Clear(surface, red, green, blue, alpha);
#endif
}

void Graphics::Idle()
{
#ifdef DIRECTFB_UI
	IDirectFB *dfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

	dfb->WaitIdle(dfb);
	dfb->WaitForSync(dfb);
#endif
}

void Graphics::Flip()
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	// surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
#endif
}

void Graphics::Flip(int xp, int yp, int wp, int hp)
{
#ifdef DIRECTFB_UI
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
#endif
}

Color & Graphics::GetColor()
{
	return _color;
} 

void Graphics::SetColor(Color &color)
{
	_color = color;

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->SetColor(surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
#endif
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

#ifdef DIRECTFB_UI
	if (_font != NULL) {
		surface->SetFont(surface, font->_font);
	}
#endif
}

Font * Graphics::GetFont()
{
	return _font;
}

void Graphics::SetAntiAliasEnabled(bool b)
{
	_antialias_enabled = b;
}

void Graphics::SetPixel(int xp, int yp, uint32_t pixel)
{
	SetRGB(xp, yp, pixel);
}

uint32_t Graphics::GetPixel(int xp, int yp)
{
	return GetRGB(xp, yp);
}

void Graphics::SetLineJoin(jline_join_t t)
{
	_line_join = t;
}

void Graphics::SetLineStyle(jline_style_t t)
{
	_line_style = t;
}

void Graphics::SetLineWidth(int size)
{
	_line_width = size;
}

jline_join_t Graphics::GetLineJoin()
{
	return _line_join;
}

jline_style_t Graphics::GetLineStyle()
{
	return _line_style;
}

int Graphics::GetLineWidth()
{
	return _line_width;
}

void Graphics::DrawLine(int xp, int yp, int xf, int yf)
{
	if (_line_width == 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (_line_width == 1) {
		int x0 = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
		int y0 = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
		int x1 = SCALE_TO_SCREEN((_translate.x+xf), _screen.width, _scale.width); 
		int y1 = SCALE_TO_SCREEN((_translate.y+yf), _screen.height, _scale.height);

		surface->DrawLine( surface, x0, y0, x1, y1);
	} else {
		double r = (_line_width),
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

		if (_line_width < 2*r1) {
			r1 = c;
		}

		FillTriangle((int)(xp-xdiff), (int)(yp-ydiff), (int)(xp+dx-xdiff), (int)(yp+dy-ydiff), (int)(xp+c-xdiff), (int)(yp-s-ydiff));
		FillTriangle((int)(xp+dx-xdiff), (int)(yp+dy-ydiff), (int)(xp+c-xdiff), (int)(yp-s-ydiff), (int)(xp+c+dx-xdiff), (int)(yp-s+dy-ydiff));
	}
#endif
}

void Graphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
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
	
		surface->DrawLine(surface, x1, y1, x2, y2);
		
		x1 = x2;
		y1 = y2;
	}

	delete [] x;
	delete [] y;
}

void Graphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	if (w < 1) {
		w = 1;
	}

	if (h < 1) {
		h = 1;
	}

	surface->FillRectangle(surface, x, y ,w, h);
#endif
}

void Graphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
#ifdef DIRECTFB_UI
	DrawRectangle0(xp, yp, wp, hp, 0, 0, MITER_JOIN, _line_width);
#endif
}

void Graphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
#ifdef DIRECTFB_UI
	DrawRectangle0(xp, yp, wp, hp, dx, dy, BEVEL_JOIN, -std::max(wp, hp));
#endif
}

void Graphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
#ifdef DIRECTFB_UI
	if (_line_width < 0) {
		DrawRectangle0(xp+1, yp+1, wp-1, hp-1, dx, dy, BEVEL_JOIN, _line_width);
	} else {
		DrawRectangle0(xp-_line_width+1, yp-_line_width+1, wp+2*_line_width-1, hp+2*_line_width-1, dx+_line_width, dy+_line_width, BEVEL_JOIN, -_line_width);
	}
#endif
}

void Graphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
#ifdef DIRECTFB_UI
	DrawRectangle0(xp, yp, wp, hp, dx, dy, ROUND_JOIN, -std::max(wp, hp));
#endif
}

void Graphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
#ifdef DIRECTFB_UI
	if (_line_width < 0) {
		DrawRectangle0(xp+1, yp+1, wp-1, hp-1, dx, dy, ROUND_JOIN, _line_width);
	} else {
		DrawRectangle0(xp-_line_width+1, yp-_line_width+1, wp+2*_line_width-1, hp+2*_line_width-1, dx+_line_width, dy+_line_width, ROUND_JOIN, -_line_width);
	}
#endif
}

void Graphics::FillCircle(int xcp, int ycp, int rp)
{
#ifdef DIRECTFB_UI
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int r = SCALE_TO_SCREEN((_translate.x+xcp+rp), _screen.width, _scale.width)-xc;

	DrawEllipse0(xc, yc, r, r, r);
#endif
}

void Graphics::DrawCircle(int xcp, int ycp, int rp)
{
#ifdef DIRECTFB_UI
	if (_line_width == 0) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int r = SCALE_TO_SCREEN((_translate.x+xcp+rp), _screen.width, _scale.width)-xc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (lw == 0 && _line_width != 0) {
		lw = 1;
	}

	if (_line_width < 0) {
		DrawEllipse0(xc, yc, r, r, -lw);
	} else {
		DrawEllipse0(xc, yc, r+lw, r+lw, lw);
	}
#endif
}

void Graphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
#ifdef DIRECTFB_UI
	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;

	DrawEllipse0(xc, yc, rx, ry, std::max(rx, ry));
#endif
}

void Graphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
#ifdef DIRECTFB_UI
	if (_line_width == 0) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width); 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height);
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (lw == 0 && _line_width != 0) {
		lw = 1;
	}

	if (_line_width < 0) {
		DrawEllipse0(xc, yc, rx, ry, -lw);
	} else {
		DrawEllipse0(xc, yc, rx+lw, ry+lw, lw);
	}
#endif
}

void Graphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

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
#endif
}

void Graphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width)-1; 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height)-1;
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (lw == 0 && _line_width != 0) {
		lw = 1;
	}

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

	if (_line_width < 0) {
		DrawChord0(xc, yc, rx, ry, arc0, arc1, -lw);
	} else {
		DrawChord0(xc, yc, rx+lw, ry+lw, arc0, arc1, lw);
	}
#endif
}

void Graphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

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
#endif
}

void Graphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int xc = SCALE_TO_SCREEN((_translate.x+xcp), _screen.width, _scale.width)-1; 
	int yc = SCALE_TO_SCREEN((_translate.y+ycp), _screen.height, _scale.height)-1;
	int rx = SCALE_TO_SCREEN((_translate.x+xcp+rxp), _screen.width, _scale.width)-xc;
	int ry = SCALE_TO_SCREEN((_translate.y+ycp+ryp), _screen.height, _scale.height)-yc;
	int lw = SCALE_TO_SCREEN((_line_width), _screen.width, _scale.width);

	if (lw == 0 && _line_width != 0) {
		lw = 1;
	}

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

		if (_line_width < 0) {
			DrawArc0(xc, yc, rx-1, ry-1, arc0, b, -lw, q);
		} else {
			DrawArc0(xc, yc, rx+lw-1, ry+lw-1, arc0, b, lw, q);
		}

		arc0 = b;
		quadrant = (quadrant+1)%4;
	}
#endif
}

void Graphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	FillArc(xcp, ycp, rxp, ryp, arc0, arc1);
}

void Graphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	if (_line_width < 0) {
		_line_width = -_line_width;

		DrawPie(xcp, ycp, rxp-_line_width, ryp-_line_width, arc0, arc1);
		
		_line_width = -_line_width;

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

	double dxangle = (_line_width*M_PI_2)/rxp,
				 dyangle = (_line_width*M_PI_2)/ryp,
				 step = 0.01;
	
	jpoint_t p[3];

	p[0].x = (rxp+_line_width+1)*cos(t0+step);
	p[0].y = -(ryp+_line_width+1)*sin(t0+step);
	p[1].x = 0;
	p[1].y = 0;
	p[2].x = (rxp+_line_width)*cos(t1);
	p[2].y = -(ryp+_line_width)*sin(t1);

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

	_line_join = MITER_JOIN;

	DrawPolygon(xcp, ycp, p, 3, false);

	_line_join = line_join;
}
		
void Graphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
#ifdef DIRECTFB_UI
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
#endif
}

void Graphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
#ifdef DIRECTFB_UI
	jpoint_t p[3];

	p[0].x = x1p;
	p[0].y = y1p;
	p[1].x = x2p;
	p[1].y = y2p;
	p[2].x = x3p;
	p[2].y = y3p;

	DrawPolygon(0, 0, p, 3, true);
#endif
}

void Graphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool close)
{
	if (_line_width <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (_line_width == 1) {
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
				int c = (int)((_line_width*dy)/d),
						s = (int)((_line_width*dx)/d),
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
			if (_line_join == BEVEL_JOIN) {
				FillTriangle(
						xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
						xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y, 
						xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
			} else if (_line_join == ROUND_JOIN) {
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

				FillArc(xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y+1, _line_width, _line_width-1, ang0, ang1);
			} else if (_line_join == MITER_JOIN) {
				int a1 = scaled[((i+0)%npoints)*2+0].y-scaled[((i+0)%npoints)*2+1].y,
						b1 = scaled[((i+0)%npoints)*2+0].x-scaled[((i+0)%npoints)*2+1].x,
						c1 = scaled[((i+0)%npoints)*2+0].x*scaled[((i+0)%npoints)*2+1].y-scaled[((i+0)%npoints)*2+1].x*scaled[((i+0)%npoints)*2+0].y;
				int a2 = scaled[((i+1)%npoints)*2+0].y-scaled[((i+1)%npoints)*2+1].y,
						b2 = scaled[((i+1)%npoints)*2+0].x-scaled[((i+1)%npoints)*2+1].x,
						c2 = scaled[((i+1)%npoints)*2+0].x*scaled[((i+1)%npoints)*2+1].y-scaled[((i+1)%npoints)*2+1].x*scaled[((i+1)%npoints)*2+0].y;
				int x0 = (b1*c2-b2*c1)/(a1*b2-a2*b1),
						y0 = (a1*c2-a2*c1)/(a1*b2-a2*b1);

				FillTriangle(
						xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
						xp+p[(i+1)%npoints].x, yp+p[(i+1)%npoints].y, 
						xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
				FillTriangle(
						xp+scaled[((i+0)%npoints)*2+1].x, yp+scaled[((i+0)%npoints)*2+1].y, 
						xp+x0, yp+y0,
						xp+scaled[((i+1)%npoints)*2+0].x, yp+scaled[((i+1)%npoints)*2+0].y);
			}
		}
	}
#endif
}

void Graphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int points[2*npoints];
	int i,
			j=0;

	for (i=0; i<npoints; i++) {
		points[j++] = SCALE_TO_SCREEN((_translate.x+p[i].x + xp), _screen.width, _scale.width);
		points[j++] = SCALE_TO_SCREEN((_translate.y+p[i].y + yp), _screen.height, _scale.height);
	}

	FillPolygon0(npoints, points);
#endif
}

void Graphics::FillHorizontalGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	int sr = scolor.GetRed(),
			sg = scolor.GetGreen(),
			sb = scolor.GetBlue(),
			sa = scolor.GetAlpha(); 

	int line_width = _line_width;

	_line_width = 1;

	double difr = (double) (dcolor.GetRed() - sr) / h;
	double difg = (double) (dcolor.GetGreen() - sg) / h;
	double difb = (double) (dcolor.GetBlue() - sb) / h;
	double difa = (double) (dcolor.GetAlpha() - sa) / h;

	for (int i = 0; i < h; i++){
		SetColor(sr + (int)(difr*i), sg + (int)(difg*i), sb + (int)(difb*i), sa + (int)(difa*i));
		surface->DrawLine( surface, x, y + i, x + w - 1, y + i );
	}

	_line_width = line_width;
#endif
}

void Graphics::FillVerticalGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x;
	int h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	int sr = scolor.GetRed(),
			sg = scolor.GetGreen(),
			sb = scolor.GetBlue(),
			sa = scolor.GetAlpha(); 

	int line_width = _line_width;

	_line_width = 1;

	double difr = (double) (dcolor.GetRed() - sr) / h;
	double difg = (double) (dcolor.GetGreen() - sg) / h;
	double difb = (double) (dcolor.GetBlue() - sb) / h;
	double difa = (double) (dcolor.GetAlpha() - sa) / h;

	for (int i = 0; i < w; i++){
		SetColor(sr + (int)(difr*i), sg + (int)(difg*i), sb + (int)(difb*i), sa + (int)(difa*i));
		surface->DrawLine( surface, x + i, y, x + i, y + h - 1);
	}

	_line_width = line_width;
#endif
}

void Graphics::DrawString(std::string text, int xp, int yp)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	OffScreenImage off(_font->GetStringWidth(text), _font->GetAscender() + _font->GetDescender(), SPF_ARGB, _scale.width, _scale.height);

	off.GetGraphics()->SetFont(_font);
	off.GetGraphics()->SetColor(_color);

	IDirectFBSurface *fsurface = (IDirectFBSurface *)(off.GetGraphics()->GetNativeSurface());

	fsurface->DrawString(fsurface, text.c_str(), -1, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	fsurface->DrawString(fsurface, text.c_str(), -1, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

	DrawImage(&off, xp, yp);

	/*
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	if (_radians == 0.0) {
		surface->DrawString(surface, text.c_str(), -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	} else {
		IDirectFBFont *font = NULL;

		if (jgui::GFXHandler::GetInstance()->CreateFont(_font->GetName(), _font->GetVirtualHeight(), &font, _font->_scale.width, _font->_scale.height, _radians) == 0) {
			surface->SetFont(surface, font);
			surface->DrawString(surface, text.c_str(), -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
			surface->SetFont(surface, _font->_font);

			font->Dispose(font);
			font->Release(font);
		}
	}
	*/
#endif
}

void Graphics::DrawGlyph(int symbol, int xp, int yp)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (_font == NULL) {
		return;
	}

	int advance;

	_font->_font->GetGlyphExtents(_font->_font, symbol, NULL, &advance);

	OffScreenImage off(advance, _font->GetAscender() + _font->GetDescender(), SPF_ARGB, _scale.width, _scale.height);

	off.GetGraphics()->SetFont(_font);
	off.GetGraphics()->SetColor(_color);

	IDirectFBSurface *fsurface = (IDirectFBSurface *)(off.GetGraphics()->GetNativeSurface());

	fsurface->DrawGlyph(fsurface, symbol, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	fsurface->DrawGlyph(fsurface, symbol, 0, 0, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

	DrawImage(&off, xp, yp);

	/*
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	if (_radians == 0.0) {
		surface->DrawGlyph(surface, symbol, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
	} else {
		IDirectFBFont *font = NULL;

		if (jgui::GFXHandler::GetInstance()->CreateFont(_font->GetName(), _font->GetVirtualHeight(), &font, _font->_scale.width, _font->_scale.height, _radians) == 0) {
			surface->SetFont(surface, font);
			surface->DrawGlyph(surface, symbol, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
			surface->SetFont(surface, _font->_font);

			font->Dispose(font);
			font->Release(font);
		}
	}
	*/
#endif
}

bool Graphics::DrawImage(std::string img, int xp, int yp)
{
	int iwidth,
			iheight;

	if (GetImageSize(img, &iwidth, &iheight) != false) {
		int wp = SCREEN_TO_SCALE((iwidth), _screen.width, _scale.width),
				hp = SCREEN_TO_SCALE((iheight), _screen.height, _scale.height);

		return Graphics::DrawImage(img, xp, yp, wp, hp);
	}

	return false;
}

bool Graphics::DrawImage(std::string img, int xp, int yp, int wp, int hp)
{
	/*
	int iwidth,
			iheight;

	if (GetImageSize(img, &iwidth, &iheight) != false) {
		return Graphics::DrawImage(img, 0, 0, iwidth, iheight, xp, yp, wp, hp, _color.alpha);
	}

	return false;
	*/

	if (xp < 0 || yp < 0) {
		return false;
	}

	if (wp < 0 || hp < 0) {
		return false;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return false;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	if (_radians != 0.0) {
		OffScreenImage off(wp, hp);
		Graphics *g = off.GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, 0, 0, wp, hp);
		
		RotateImage0(&off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		return true;
	}

	IDirectFBSurface *imgSurface = NULL;
	IDirectFBImageProvider *imgProvider = NULL;
	DFBSurfaceDescription desc;

	GFXHandler *dfb = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)dfb->GetGraphicEngine();

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
#endif

	return true;
}

bool Graphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if (sxp < 0 || syp < 0 || swp < 0 || shp < 0) {
		return false;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return false;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	if (_radians != 0.0) {
		int wp = SCREEN_TO_SCALE((swp), _screen.width, _scale.width),
				hp = SCREEN_TO_SCALE((shp), _screen.height, _scale.height);

		OffScreenImage off(wp, hp);
		Graphics *g = off.GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0);
		
		RotateImage0(&off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		return true;
	}

	IDirectFBSurface *imgSurface = NULL;
	IDirectFBImageProvider *imgProvider = NULL;
	DFBSurfaceDescription desc;

	GFXHandler *dfb = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)dfb->GetGraphicEngine();

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
#endif

	return true;
}

bool Graphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	if (sxp < 0 || syp < 0 || xp < 0 || yp < 0) {
		return false;
	}

	if (swp < 0 || shp < 0 || wp < 0 || hp < 0) {
		return false;
	}

#ifdef DIRECTFB_UI
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
		OffScreenImage off(wp, hp);
		Graphics *g = off.GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0, wp, hp);
		
		RotateImage0(&off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		return true;
	}

	IDirectFBSurface *imgSurface = NULL;
	IDirectFBImageProvider *imgProvider = NULL;
	DFBSurfaceDescription desc;

	GFXHandler *dfb = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)dfb->GetGraphicEngine();

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
#endif

	return true;
}

bool Graphics::DrawImage(OffScreenImage *img, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, img->GetWidth(), img->GetHeight());
}

bool Graphics::DrawImage(OffScreenImage *img, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool Graphics::DrawImage(OffScreenImage *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, sxp, syp, swp, shp, xp, yp, swp, shp);
}

bool Graphics::DrawImage(OffScreenImage *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
#ifdef DIRECTFB_UI
	if ((void *)surface == NULL) {
		return false;
	}

	if ((void *)img == NULL) {
		return false;
	}

	Graphics *g = img->GetGraphics();

	if ((void *)g == NULL) {
		return false;
	}

	int sx = SCALE_TO_SCREEN(sxp, _screen.width, _scale.width),
			sy = SCALE_TO_SCREEN(syp, _screen.height, _scale.height),
			sw = SCALE_TO_SCREEN(swp, _screen.width, _scale.width),
			sh = SCALE_TO_SCREEN(shp, _screen.height, _scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

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

	if (_radians != 0.0) {
		OffScreenImage off(wp, hp);
		
		Graphics *g = off.GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0, wp, hp);

		RotateImage0(&off, -_translate_image.x, -_translate_image.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, _color.GetAlpha());

		return true;
	}

	surface->StretchBlit(surface, g->surface, &srect, &drect);
#endif

	return true;
}

void Graphics::Rotate(double radians)
{
	_radians = fmod(radians, 2*M_PI);
}

void Graphics::Translate(int x, int y)
{
	_translate.x += x;
	_translate.y += y;
}

void Graphics::TranslateImage(int x, int y)
{
	_translate_image.x = x;
	_translate_image.y = y;
}

double Graphics::Rotate()
{
	return _radians;
}

jpoint_t Graphics::Translate()
{
	return _translate;
}

jpoint_t Graphics::TranslateImage()
{
	return _translate_image;
}

void Graphics::GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign)
{
	if (wp < 0 || hp < 0) {
		return;
	}

	jcommon::StringTokenizer token(text, "\n", jcommon::SPLIT_FLAG, false);

	for (int i=0; i<token.GetSize(); i++) {
		std::vector<std::string> words;
		
		std::string line = token.GetToken(i);

		line = jcommon::StringUtils::ReplaceString(line, "\n", "");
		line = jcommon::StringUtils::ReplaceString(line, "\t", "    ");
		
		if (halign == JUSTIFY_HALIGN) {
			jcommon::StringTokenizer line_token(line, " ", jcommon::SPLIT_FLAG, false);

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
			jcommon::StringTokenizer line_token(line, " ", jcommon::SPLIT_FLAG, true);

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

void Graphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
	if (wp < 0 || hp < 0) {
		return;
	}

#ifdef DIRECTFB_UI
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

		if (valign == TOP_VALIGN) {
			line_space = 0;
			line_yinit = 0;
			line_ydiff = 0;
		} else if (valign == CENTER_VALIGN) {
			line_space = 0;
			line_yinit = (hp-nlines*font_height)/2;
			line_ydiff = 0;
		} else if (valign == BOTTOM_VALIGN) {
			line_space = 0;
			line_yinit = hp-nlines*font_height;
			line_ydiff = 0;
		} else if (valign == JUSTIFY_VALIGN) {
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
		int cx = xp,
				cy = yp,
				cw = wp,
				ch = hp;

		if (cx > clip.width) {
			cx = clip.width;
		}

		if (cy > clip.height) {
			cy = clip.height;
		}

		if (cw > (clip.width-cx)) {
			cw = clip.width-cx;
		}

		if (ch > (clip.height-cy)) {
			ch = clip.height-cy;
		}

		SetClip(cx, cy, cw, ch);
	}
	
	if (halign == JUSTIFY_HALIGN) {
		std::string token_trim;
			
		dy = line_yinit;

		for (int i=0; i<(int)lines.size() && i<max_lines; i++) {
			jcommon::StringTokenizer token(lines[i], " ", jcommon::SPLIT_FLAG, false);


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

			if (halign == LEFT_HALIGN) {
				dx = 0;
			} else if (halign == CENTER_HALIGN) {
				dx = (wp-size)/2;
			} else if (halign == RIGHT_HALIGN) {
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
#endif
}

uint32_t Graphics::GetRGB(int xp, int yp, uint32_t pixel)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return 0x00000000;
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
#endif

	return pixel;
}

void Graphics::GetRGBArray(int startxp, int startyp, int wp, int hp, unsigned int **rgb, int offset, int scansize)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int startx = SCALE_TO_SCREEN(startxp, _screen.width, _scale.width); 
	int starty = SCALE_TO_SCREEN(startyp, _screen.height, _scale.height);
	// int w = SCALE_TO_SCREEN(wp, _screen.width, _scale.width);
	// int h = SCALE_TO_SCREEN(hp, _screen.height, _scale.height);
	int w = SCALE_TO_SCREEN((startxp+wp), _screen.width, _scale.width)-startx;
	int h = SCALE_TO_SCREEN((startyp+hp), _screen.height, _scale.height)-starty;

	void *ptr;
	uint32_t *dst;
	int x,
			y,
			depth,
			pitch;
	uint32_t *array = (*rgb);

	depth = (scansize/wp);
	scansize = depth*w;

	if (*rgb == NULL) {
		array = new uint32_t[(w-startx)*(h-starty)*depth];
	}

	surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

	for (y=starty; y<starty+h; y++) {
		for (x=startx; x<startx+w; x++) {
			dst = (uint32_t *)((uint8_t *)ptr + (starty + y) * pitch);
			array[offset + (y-starty)*scansize + (x-startx)] = *(dst + x + startx);
		}
	}

	surface->Unlock(surface);

	(*rgb) = array;
#endif
}

void Graphics::SetRGB(int xp, int yp, uint32_t argb) 
{
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	int r = (argb >> 0x10) & 0xff,
			g = (argb >> 0x08) & 0xff,
			b = (argb >> 0x00) & 0xff,
			a = (argb >> 0x18) & 0xff;

	surface->SetColor(surface, r, g, b, a);
	surface->DrawLine(surface, x, y, x, y);
}

void Graphics::SetRGB(uint32_t *rgb, int x, int y, int w, int h, int scanline) 
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	void *ptr;
	uint32_t *dst,
					 *prgb = rgb;
	int cy,
			step,
			pitch;
	int wmax,
			hmax;

	surface->GetSize(surface, &wmax, &hmax);

	wmax = SCREEN_TO_SCALE(wmax, _screen.width, _scale.width);
	hmax = SCREEN_TO_SCALE(hmax, _screen.height, _scale.height);

	if (w > wmax) {
		w = wmax;
	}

	if (x > w) {
		x = w;
	}

	if (h >= hmax) {
		h = hmax-1;
	}

	if (y > h) {
		y = h;
	}

	wmax = x+w;
	hmax = y+h;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	double d = (double)_screen.height/(double)_scale.height; 

	if (_draw_flags == DF_NOFX) {
		double k;

		step = 0;

		for (int j=y; j<hmax-1; j++) {
			cy = (int)(j*d);//SCALE_TO_SCREEN(j, _screen.height, _scale.height);
			dst = (uint32_t *)((uint8_t *)ptr+cy*pitch);
			prgb = (uint32_t *)(rgb+step);
			step = step+w;

			k = 0.0;

			for (int i=x; i<wmax; i++) {
				k = k+d;//SCALE_TO_SCREEN(i, _screen.width, _scale.width); 

				*(dst+(int)k) = *(prgb+i);
			}
		}
	} else if (_draw_flags == DF_BLEND) {
		// INFO:: BLEND source with destination
		double k;

		for (int j=y; j<hmax; j++) {
			cy = (int)(j*d);//SCALE_TO_SCREEN(j, _screen.height, _scale.height);
			dst = (uint32_t *)((uint8_t *)ptr+cy*pitch);
			step = j*w;
			prgb = (uint32_t *)(rgb+step);

			k = 0.0;

			for (int i=x; i<wmax; i++) {
				k = k+d;//SCALE_TO_SCREEN(i, _screen.width, _scale.width); 

				int argb = *(prgb+i),
						pixel = *(dst+(int)k),
						r = (argb>>0x10)&0xff,
						g = (argb>>0x08)&0xff,
						b = (argb>>0x00)&0xff,
						a = (argb>>0x18)&0xff,
						pr = (pixel>>0x10)&0xff,
						pg = (pixel>>0x08)&0xff,
						pb = (pixel>>0x00)&0xff;

				pr = (int)(pr*(255-a) + r*a) >> 8;
				pg = (int)(pg*(255-a) + g*a) >> 8;
				pb = (int)(pb*(255-a) + b*a) >> 8;

				*(dst+(int)k) = 0xff000000 | (pr << 0x10) | (pg << 0x08) | (pb << 0x00);
			}
		}
	} else if (_draw_flags == DF_XOR) {
		double k;

		for (int j=y; j<hmax; j++) {
			cy = (int)(j*d);//SCALE_TO_SCREEN(j, _screen.height, _scale.height);
			dst = (uint32_t *)((uint8_t *)ptr+cy*pitch);
			step = j*w;

			k = 0.0;

			for (int i=x; i<wmax; i++) {
				k = k+d;//SCALE_TO_SCREEN(i, _screen.width, _scale.width); 

				*(dst+(int)k) ^= rgb[step+i];
			}
		}
	}

	surface->Unlock(surface);
#endif
}

void Graphics::Reset()
{
	// _font = Font::GetDefaultFont();

	_color = Color(0x00, 0x00, 0x00, 0x00);

	_radians = 0.0;

	_translate_image.x = 0;
	_translate_image.y = 0;

	_line_width = 1;
	_line_join = BEVEL_JOIN;
	_line_style = SOLID_LINE;

	SetDrawingFlags(DF_BLEND);
	SetBlittingFlags(BF_ALPHACHANNEL);
	SetPorterDuffFlags(PDF_SRC_OVER);
}

void Graphics::Lock()
{
	graphics_mutex.Lock();
}

void Graphics::Unlock()
{
	graphics_mutex.Unlock();
}

#ifdef DIRECTFB_UI
void Graphics::InsertEdge(edge_t *list, edge_t *edge)
{
	jcommon::Object::SetClassName("jgui::Graphics");

	if (list == NULL || edge == NULL) {
		return;
	}

	edge_t *p;
	edge_t *q = list;

	p = q->next;

	while (p != NULL) {
		if (edge->xIntersect < p->xIntersect) {
			p = NULL;
		} else {
			q = p;
			p = p->next;
		}
	}

	edge->next=q->next;
	q->next=edge;
}

void Graphics::MakeEdgeRec(struct jpoint_t lower, struct jpoint_t upper, int yComp, edge_t *edge, edge_t *edges[])
{
	edge->dxPerScan = ((double)(upper.x-lower.x)/(double)(upper.y-lower.y));
	edge->xIntersect = lower.x;

	if(upper.y<yComp) {
		edge->yUpper = (upper.y-1);
	} else {
		edge->yUpper = upper.y;
	}

	InsertEdge(edges[(int)lower.y], edge);
}

void Graphics::FillScan(int scan, edge_t *active)
{
	if (active == NULL) {
		return;
	}

	edge_t *p1,
				 *p2;
	int count;

	p1 = active->next;

	while (p1 != NULL) {
		p2 = p1->next;

		for (count=(int)p1->xIntersect; p2!=NULL && count<=(int)p2->xIntersect; count++) {
			surface->DrawLine(surface, count, scan, count, scan);
		}

		if (p2 != NULL) {
			p1=p2->next;
		} else {
			break;
		}
	}
}

int Graphics::YNext(int k, int cnt, jpoint_t pts[])
{
	int j;

	if ((k+1) > (cnt-1)) {
		j = 0;
	} else {
		j = (k+1);
	}

	while (pts[k].y == pts[j].y) {
		if ((j+1) > (cnt-1)) {
			j = 0;
		} else {
			j++;
		}
	}

	return pts[j].y;
}


void Graphics::BuildEdgeList(int cnt, jpoint_t pts[], edge_t *edges[])
{
	edge_t *edge;
	jpoint_t v1,
					 v2;

	int yPrev = (pts[cnt-2].y);
	int count;

	v1.x = pts[cnt-1].x;
	v1.y = pts[cnt-1].y;

	for (count=0; count<cnt; count++) {
		v2 = pts[count];

		if (v1.y != v2.y) {
			edge = (edge_t *) malloc (sizeof(edge_t));

			if (v1.y<v2.y) {
				MakeEdgeRec(v1, v2, YNext(count,cnt,pts), edge,edges);
			} else {
				MakeEdgeRec(v2, v1, yPrev, edge, edges);
			}
		}

		yPrev = v1.y;
		v1 = v2;
	}

}

void Graphics::UpdateActiveList(int scan, edge_t *active)
{
	edge_t *q = active,
				 *p = active->next,
				 *tmp;

	while (p) {
		if (scan >= p->yUpper) {
			p = p->next;

			tmp = q->next;
			q->next = tmp->next;

			free(tmp);
		} else {
			p->xIntersect = (p->xIntersect+p->dxPerScan);
			q = p;
			p = p->next;
		}
	}
}

void Graphics::FillPolygon0(int n, int ppts[])
{
	const int max_points = 10*1024;

	struct edge_t *edges[max_points],
								*active;
	int count_1,
			count_2,
			count_3;

	jpoint_t *pts = new jpoint_t[n];

	for (count_1=0; count_1<n; count_1++) {
		pts[count_1].x = (ppts[(count_1*2)]);
		pts[count_1].y = (ppts[((count_1*2)+1)]);
	}

	for (count_2=0; count_2<max_points; count_2++) {
		edges[count_2] = new edge_t;
		edges[count_2]->next = NULL;
	}

	BuildEdgeList(n, pts, edges);

	active = new edge_t;
	active->next = NULL;

	for (count_3=0; count_3<max_points; count_3++) {
		// build active list
		edge_t *p,
					 *q;

		p=edges[count_3]->next;

		while(p) {
			q = p->next;

			InsertEdge(active, p);

			p = q;
		}

		if (active->next) {
			FillScan(count_3, active);
			UpdateActiveList(count_3, active);

			// resort active list
			edge_t *q,
						 *p = active->next;

			active->next = NULL;

			while (p) {
				q = p->next;

				InsertEdge(active, p);

				p = q;
			}
		}
	}

	// draw edges
	/*
	if (n >= 2) {
		surface->DrawLine(surface, ppts[0], ppts[1], ppts[2], ppts[3]);

		for (int count=1; count<(n-1); count++) {
			surface->DrawLine(surface, ppts[(count*2)], ppts[((count*2)+1)], ppts[((count+1)*2)], ppts[(((count+1)*2)+1)]);
		}
	}
	*/

	for (count_2=0; count_2<max_points; count_2++) {
		delete edges[count_2];
	}

	delete [] pts;
}

Vector2d::Vector2d(float x,float y)
{
	Set(x,y);
}

float Vector2d::GetX(void) const 
{ 
	return mX; 
}

float Vector2d::GetY(void) const 
{
	return mY; 
}

void  Vector2d::Set(float x,float y)
{
	mX = x;
	mY = y;
}

bool Triangulate::Snip(const Vector2dVector &contour,int u,int v,int w,int n,int *V)
{
	float Ax, Ay, Bx, By, Cx, Cy, Px, Py;
	int p;

	Ax = contour[V[u]].GetX();
	Ay = contour[V[u]].GetY();

	Bx = contour[V[v]].GetX();
	By = contour[V[v]].GetY();

	Cx = contour[V[w]].GetX();
	Cy = contour[V[w]].GetY();

	if (EPSILON > (((Bx-Ax)*(Cy-Ay))-((By-Ay)*(Cx-Ax)))) {
		return false;
	}

	for (p=0;p<n;p++)
	{
		if( (p == u) || (p == v) || (p == w) ) continue;
		Px = contour[V[p]].GetX();
		Py = contour[V[p]].GetY();
		if (InsideTriangle(Ax,Ay,Bx,By,Cx,Cy,Px,Py)) return false;
	}

	return true;
}

// triangulate a contour/polygon, places results in STL vector as series of triangles
bool Triangulate::Process(const Vector2dVector &contour, Vector2dVector &result)
{
	// allocate and initialize list of Vertices in polygon
	int n = contour.size();

	if (n < 3) {
		return false;
	}

	int *V = new int[n];

	// we want a counter-clockwise polygon in V 
	if ( 0.0f < Area(contour) )
		for (int v=0; v<n; v++) V[v] = v;
	else
		for(int v=0; v<n; v++) V[v] = (n-1)-v;

	int nv = n;

	//  remove nv-2 Vertices, creating 1 triangle every time 
	int count = 2*nv;

	for(int m=0, v=nv-1; nv>2; ) {
		// if we loop, it is probably a non-simple polygon 
		if (0 >= (count--)) {
			// Triangulate: ERROR - probable bad polygon!
			return false;
		}

		// three consecutive vertices in current polygon, <u,v,w> 
		int u = v  ; if (nv <= u) u = 0;	// previous 
		v = u+1; if (nv <= v) v = 0;		// new v    
		int w = v+1; if (nv <= w) w = 0;	// next     

		if ( Snip(contour,u,v,w,nv,V) ) {
			int a,b,c,s,t;

			// true names of the vertices 
			a = V[u]; b = V[v]; c = V[w];

			// output Triangle 
			result.push_back( contour[a] );
			result.push_back( contour[b] );
			result.push_back( contour[c] );

			m++;

			// remove v from remaining polygon 
			for(s=v,t=v+1;t<nv;s++,t++) V[s] = V[t]; nv--;

			// resest error detection counter 
			count = 2*nv;
		}
	}

	delete V;

	return true;
}

// compute area of a contour/polygon
float Triangulate::Area(const Vector2dVector &contour)
{
	float A = 0.0f;
	int n = contour.size();

	for(int p=n-1,q=0; q<n; p=q++) {
		A+= contour[p].GetX()*contour[q].GetY() - contour[q].GetX()*contour[p].GetY();
	}

	return A*0.5f;
}

// decide if point Px/Py is inside triangle defined by (Ax,Ay) (Bx,By) (Cx,Cy)
bool Triangulate::InsideTriangle(float Ax, float Ay, float Bx, float By, float Cx, float Cy, float Px, float Py)
{
	float ax, 
				ay, 
				bx, 
				by, 
				cx, 
				cy, 
				apx, 
				apy, 
				bpx, 
				bpy, 
				cpx, 
				cpy,
				cCROSSap, 
				bCROSScp, 
				aCROSSbp;

	ax = Cx - Bx;  ay = Cy - By;
	bx = Ax - Cx;  by = Ay - Cy;
	cx = Bx - Ax;  cy = By - Ay;
	apx= Px - Ax;  apy= Py - Ay;
	bpx= Px - Bx;  bpy= Py - By;
	cpx= Px - Cx;  cpy= Py - Cy;

	aCROSSbp = ax*bpy - ay*bpx;
	cCROSSap = cx*apy - cy*apx;
	bCROSScp = bx*cpy - by*cpx;

	return ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f));
}

double Graphics::EvaluateBezier0(double *data, int ndata, double t) 
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

void Graphics::DrawRectangle0(int xp, int yp, int wp, int hp, int dx, int dy, jline_join_t join, int size)
{ 
	if (wp <= 0 || hp <= 0) {
		return;
	}

	if (size == 0) {
		return;
	}

	if (surface == NULL) {
		return;
	}

	int line_width = _line_width;

	bool close = false;

	if (join == MITER_JOIN) {
		if (size < 0) {
			size = -size;

			if (size > (std::min(wp, hp)/2)) {
				size = std::min(wp, hp)/2;
	
				close = true;
			}

			xp = xp + size;
			yp = yp + size;
			wp = wp - 2*size;
			hp = hp - 2*size;
		}

		FillRectangle(xp-size+1, yp-size+1, wp+2*size-1, size);
		FillRectangle(xp-size+1, yp+hp, wp+2*size-1, size);
		FillRectangle(xp-size+1, yp+1, size, hp-1);
		FillRectangle(xp+wp, yp+1, size, hp-1);
	} else if (join == BEVEL_JOIN) {
		if (size < 0) {
			size = -size;

			if (dx > wp/2) {
				dx = wp/2;
			}

			if (dy > hp/2) {
				dy = hp/2;
			}

			if (size > (std::min(wp, hp)/2-1)) {
				size = std::min(wp, hp)/2-1;

				close = true;
			}

			if (size <= (std::max(dx, dy))) {
				if (size == 1) {
					DrawLine(xp+dx, yp, xp, yp+dy);
					DrawLine(xp+wp-dx-1, yp, xp+wp-1, yp+dy);
					DrawLine(xp+wp-1, yp+hp-dy, xp+wp-dx-1, yp+hp);
					DrawLine(xp, yp+hp-dy, xp+dx, yp+hp);
				} else {
					FillTriangle(xp+dx, yp, xp, yp+dy, xp+size, yp+dy);
					FillTriangle(xp+size, yp+dy, xp+dx, yp, xp+dx, yp+size);

					FillTriangle(xp+wp-dx, yp, xp+wp-dx, yp+size, xp+wp, yp+dy);
					FillTriangle(xp+wp-dx, yp+size, xp+wp-size, yp+dy,  xp+wp, yp+dy);

					FillTriangle(xp+wp-size, yp+hp-dy, xp+wp, yp+hp-dy, xp+wp-dx, yp+hp);
					FillTriangle(xp+wp-size, yp+hp-dy, xp+wp-dx, yp+hp, xp+wp-dx, yp+hp-size);

					FillTriangle(xp, yp+hp-dy, xp+size, yp+hp-dy, xp+dx, yp+hp);
					FillTriangle(xp+size, yp+hp-dy, xp+dx, yp+hp-size, xp+dx, yp+hp);
				} 

				FillRectangle(xp+dx, yp, wp-2*dx, size);
				FillRectangle(xp+dx, yp+hp-size, wp-2*dx, size);
				FillRectangle(xp, yp+dy, size, hp-2*dy);
				FillRectangle(xp+wp-size, yp+dy, size, hp-2*dy);
			} else {
				FillTriangle(xp+dx, yp, xp, yp+dy, xp+dx, yp+dy);
				FillTriangle(xp+wp-dx, yp, xp+wp, yp+dy, xp+wp-dx, yp+dy);
				FillTriangle(xp, yp+hp-dy, xp+dx, yp+hp-dy, xp+dx, yp+hp);
				FillTriangle(xp+wp-dx, yp+hp-dy, xp+wp, yp+hp-dy, xp+wp-dx, yp+hp);

				FillRectangle(xp+dx, yp, wp-2*dx, dy);
				FillRectangle(xp+dx, yp+hp-dy, wp-2*dx, dy);
				FillRectangle(xp, yp+dy, dx, hp-2*dy);
				FillRectangle(xp+wp-dx, yp+dy, dx, hp-2*dy);

				DrawRectangle0(xp+dx-1, yp+dy-1, wp-2*dx+1, hp-2*dy+1, 0, 0, MITER_JOIN, -size+std::max(dx, dy)-1);
				
				if (close == true) {
					int x = xp+size+1, 
							y = yp+size+1, 
							w = wp-2*(size+1), 
							h = hp-2*(size+1);
					
					_line_width = 1;

					if (w > 0 && h > 0) {
						DrawLine(x, y, x+w-1, y+h-1);
					}
				}
			}
		}
	} else if (join == ROUND_JOIN) {
		if (size < 0) {
			_line_width = size;

			DrawArc(xp+dx, yp+dy, dx, dy, M_PI_2, M_PI);
			DrawArc(xp+dx, yp+hp-dy, dx, dy, M_PI, M_PI+M_PI_2);
			DrawArc(xp+wp-dx, yp+hp-dy, dx, dy, M_PI+M_PI_2, 2*M_PI);
			DrawArc(xp+wp-dx, yp+dy, dx, dy, 0.0, M_PI_2);
			
			size = -size;

			if (dx > wp/2) {
				dx = wp/2;
			}

			if (dy > hp/2) {
				dy = hp/2;
			}

			if (size > (std::min(wp, hp)/2-1)) {
				size = std::min(wp, hp)/2-1;

				close = true;
			}

			if (size <= (std::max(dx, dy))) {
				FillRectangle(xp+dx, yp, wp-2*dx, size);
				FillRectangle(xp+dx, yp+hp-size, wp-2*dx, size);
				FillRectangle(xp, yp+dy, size, hp-2*dy);
				FillRectangle(xp+wp-size, yp+dy, size, hp-2*dy);
			} else {
				FillRectangle(xp+dx, yp, wp-2*dx, dy);
				FillRectangle(xp+dx, yp+hp-dy, wp-2*dx, dy);
				FillRectangle(xp, yp+dy, dx, hp-2*dy);
				FillRectangle(xp+wp-dx, yp+dy, dx, hp-2*dy);
				
				DrawRectangle0(xp+dx-1, yp+dy-1, wp-2*dx+1, hp-2*dy+1, 0, 0, MITER_JOIN, -size+std::max(dx, dy)-1);
				
				if (close == true) {
					int x = xp+size+1, 
							y = yp+size+1, 
							w = wp-2*(size+1), 
							h = hp-2*(size+1);
					
					_line_width = 1;

					if (w > 0 && h > 0) {
						DrawLine(x, y, x+w-1, y+h-1);
					}
				}
			}
		}
	}

	_line_width = line_width;
}

void Graphics::DrawArc0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size, int quadrant)
{
	if (size == 0) {
		return;
	}

	if (surface == NULL) {
		return;
	}

	double line_width = (double)size;
	
	if (line_width > std::max(rx, ry)) {
		line_width = std::max(rx, ry);
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
						if (xi < 0.0) {
						xi = x;
					}

					xf = x;
				}
			}
		}

		if  (xi < 0.0 || xf < 0.0) {
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
			
	surface->DrawLines(surface, lines, k);
	
	if (_antialias_enabled == true) {
		int cx = 0,
				cy = 0;

		if (quadrant == 0 || quadrant == 3) {
			for (int y=1; y<k; y++) {
				if (lines[y].x2 < lines[y-1].x2) {
					cx = 1;
					cy = lines[y-1].x2-lines[y].x2;
				} else if (lines[y].x2 == lines[y-1].x2) {
					if (cx > 0) {
						cx = cx + 1;
						cy = 0;
					}
				}

				if (cx > 0) {
					surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*cx));
					surface->DrawLine(surface, lines[y].x2+1, lines[y].y1, lines[y].x2+1, lines[y].y1);

					if (cy > 1) {
						for (int i=1; i<cy; i++) {
							surface->SetColor(surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*i));
							surface->DrawLine(surface, lines[y].x2+i+1, lines[y].y1, lines[y].x2+i+1, lines[y].y1);
						}
					}
				}
			}
		} else if (quadrant == 1 || quadrant == 2) {
			for (int y=1; y<k; y++) {
				if (lines[y].x1 > lines[y-1].x1) {
					cx = 1;
					cy = lines[y].x1-lines[y-1].x1;
				} else if (lines[y].x1 == lines[y-1].x1) {
					if (cx > 0) {
						cx = cx + 1;
						cy = 0;
					}
				}

				if (cx > 0) {
					surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*cx));
					surface->DrawLine(surface, lines[y].x1-1, lines[y].y1, lines[y].x1-1, lines[y].y1);

					if (cy > 1) {
						for (int i=1; i<cy; i++) {
							surface->SetColor(surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*i));
							surface->DrawLine(surface, lines[y].x1-i-1, lines[y].y1, lines[y].x1-i-1, lines[y].y1);
						}
					}
				}
			}
		}

		surface->SetColor(surface, _color.GetRed(), _color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
	}
}

void Graphics::DrawEllipse0(int xc, int yc, int rx, int ry, int size)
{
	if (size == 0) {
		return;
	}

	if (surface == NULL) {
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
	DFBRegion lines[4*(int)max_ry];
	int old_x = max_rx,
			old_y = 0;
	int k = 0;

	for (double y=0.0; y<max_ry; y+=1.0) {
		double xi = -1.0,
					 xf = -1.0;

		for (double x=0.0; x<max_rx; x+=1.0) {
			/* circle
			double dsqrt = (x*x+y*y);

			if (dsqrt >= (min_rx*min_rx) && dsqrt <= (max_rx*max_rx)) {
				if (xi < 0.0) {
					xi = x;
				}

				xf = x;
			}
			*/

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
				if (xi < 0.0) {
					xi = x;
				}

				xf = x;
			}
		}

		if (line_width <= 1) {
			// quadrante 0
			lines[k].x1 = (int)(xc+old_x);
			lines[k].y1 = (int)(yc-y);
			lines[k].x2 = (int)(xc+xi);
			lines[k].y2 = (int)(yc-y);

			k++;

			// quadrante 1
			lines[k].x1 = (int)(xc-old_x-1);
			lines[k].y1 = (int)(yc-y);
			lines[k].x2 = (int)(xc-xi-1);
			lines[k].y2 = (int)(yc-y);

			k++;

			// quadrante 2
			lines[k].x1 = (int)(xc-old_x-1);
			lines[k].y1 = (int)(yc+y+1);
			lines[k].x2 = (int)(xc-xi-1);
			lines[k].y2 = (int)(yc+y+1);

			k++;

			// quadrante 3
			lines[k].x1 = (int)(xc+old_x);
			lines[k].y1 = (int)(yc+y+1);
			lines[k].x2 = (int)(xc+xi);
			lines[k].y2 = (int)(yc+y+1);

			k++;

			old_x = xi;
			old_y = y;
		} else {
			// quadrante 0
			lines[k].x1 = (int)(xc+xi);
			lines[k].y1 = (int)(yc-y);
			lines[k].x2 = (int)(xc+xf);
			lines[k].y2 = (int)(yc-y);

			k++;

			// quadrante 1
			lines[k].x1 = (int)(xc-xf-1);
			lines[k].y1 = (int)(yc-y);
			lines[k].x2 = (int)(xc-xi-1);
			lines[k].y2 = (int)(yc-y);

			k++;

			// quadrante 2
			lines[k].x1 = (int)(xc-xf-1);
			lines[k].y1 = (int)(yc+y+1);
			lines[k].x2 = (int)(xc-xi-1);
			lines[k].y2 = (int)(yc+y+1);

			k++;

			// quadrante 3
			lines[k].x1 = (int)(xc+xi);
			lines[k].y1 = (int)(yc+y+1);
			lines[k].x2 = (int)(xc+xf);
			lines[k].y2 = (int)(yc+y+1);

			k++;
		}
	}
	
	surface->DrawLines(surface, lines, k);

	if (_antialias_enabled == true) {
		int cx = 0,
				cy = 0;

		// quadrant 0, 1
		for (int y=4; y<k; y+=4) {
			if (lines[y+1].x1 > lines[y-4+1].x1) {
				cx = 1;
				cy = lines[y+1].x1-lines[y-4+1].x1;
			} else if (lines[y+1].x1 == lines[y-4+1].x1) {
				if (cx > 0) {
					cx = cx + 1;
					cy = 0;
				}
			}

			if (cx > 0) {
				surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*cx));
				surface->DrawLine(surface, lines[y+1].x1-1, lines[y+1].y1, lines[y+1].x1-1, lines[y+1].y1);
				surface->DrawLine(surface, lines[y+0].x2+1, lines[y+0].y2, lines[y+0].x2+1, lines[y+0].y2);

				if (cy > 1) {
					for (int i=1; i<cy; i++) {
						surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*i));
						surface->DrawLine(surface, lines[y+1].x1-i-1, lines[y+1].y1, lines[y+1].x1-i-1, lines[y+1].y1);
						surface->DrawLine(surface, lines[y+0].x2+i+1, lines[y+0].y2, lines[y+0].x2+i+1, lines[y+0].y2);
					}
				}
			}
		}

		// quadrant 2, 3
		for (int y=4; y<k; y+=4) {
			if (lines[y+2].x1 > lines[y-4+2].x1) {
				cx = 1;
				cy = lines[y+1].x1-lines[y-4+1].x1;
			} else if (lines[y+2].x1 == lines[y-4+2].x1) {
				if (cx > 0) {
					cx = cx + 1;
					cy = 0;
				}
			}

			if (cx > 0) {
				surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*cx));
				surface->DrawLine(surface, lines[y+2].x1-1, lines[y+2].y1, lines[y+2].x1-1, lines[y+2].y1);
				surface->DrawLine(surface, lines[y+3].x2+1, lines[y+3].y2, lines[y+3].x2+1, lines[y+3].y2);

				if (cy > 1) {
					for (int i=1; i<cy; i++) {
						surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha()/(2*i));
						surface->DrawLine(surface, lines[y+2].x1-i-1, lines[y+2].y1, lines[y+2].x1-i-1, lines[y+2].y1);
						surface->DrawLine(surface, lines[y+3].x2+i+1, lines[y+3].y2, lines[y+3].x2+i+1, lines[y+3].y2);
					}
				}
			}
		}

		surface->SetColor(surface, _color.GetRed(),_color.GetGreen(), _color.GetBlue(), _color.GetAlpha());
	}
}

void Graphics::DrawChord0(int xc, int yc, int rx, int ry, double arc0, double arc1, int size)
{
	if (size == 0) {
		return;
	}

	if (surface == NULL) {
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
	int xi,
			xf,
			k = 0;

	for (double y=-max_ry; y<max_ry; y+=1.0) {
		xi = -1;
		xf = -1;

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
						// surface->DrawLine(surface, xc+x, yc+y, xc+x, yc+y);
						 
						if (xi < 0) {
							xi = xc+x;
						}

						xf = xc+x;
					} else if (min_ellipse < 1.0) {
						if (xi > 1) {
							lines[k].x1 = (int)(xi);
							lines[k].y1 = (int)(yc+y);
							lines[k].x2 = (int)(xc+x);
							lines[k].y2 = (int)(yc+y);

							xi = -1;

							k++;
						}
					}
				}
			}
		}

		if (xi > 0) {
			lines[k].x1 = (int)(xi);
			lines[k].y1 = (int)(yc+y);
			lines[k].x2 = (int)(xf);
			lines[k].y2 = (int)(yc+y);

			k++;
		}
	}
	
	surface->DrawLines(surface, lines, k);
}

#endif

void Graphics::RotateImage0(OffScreenImage *img, int xcp, int ycp, int xp, int yp, int wp, int hp, double angle, uint8_t alpha)
{
#ifdef DIRECTFB_UI
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

							if (_blit_flags & BF_ALPHACHANNEL) {
								if (_porter_duff_flags == PDF_NONE) {
									dr = (gr*ga + sr*sa);
									dg = (gg*ga + sg*sa);
									db = (gb*ga + sb*sa);
								} else if (_porter_duff_flags == PDF_CLEAR) {
									dr = 0;
									dg = 0;
									db = 0;
								} else if (_porter_duff_flags == PDF_SRC) {
									dr = gr*0xff;
									dg = gg*0xff;
									db = gb*0xff;
								} else if (_porter_duff_flags == PDF_DST) {
									dr = sr*0xff;
									dg = sg*0xff;
									db = sb*0xff;
								} else if (_porter_duff_flags == PDF_SRC_OVER) {
									dr = (gr*0xff + sr*sa);
									dg = (gg*0xff + sg*sa);
									db = (gb*0xff + sb*sa);
								} else if (_porter_duff_flags == PDF_DST_OVER) {
									dr = (gr*ga + sr*0xff);
									dg = (sg*ga + sg*0xff);
									db = (sb*ga + sb*0xff);
								} else if (_porter_duff_flags == PDF_SRC_IN) {
									dr = gr*sa;
									dg = gg*sa;
									db = gb*sa;
								} else if (_porter_duff_flags == PDF_DST_IN) {
									dr = sr*ga;
									dg = sg*ga;
									db = sb*ga;
								} else if (_porter_duff_flags == PDF_SRC_OUT) {
									dr = gr*ga;
									dg = gg*ga;
									db = gb*ga;
								} else if (_porter_duff_flags == PDF_DST_OUT) {
									dr = sr*sa;
									dg = sg*sa;
									db = sb*sa;
								} else if (_porter_duff_flags == PDF_SRC_ATOP) {
									dr = (gr*sa + sr*sa);
									dg = (sg*sa + sg*sa);
									db = (sb*sa + sb*sa);
								} else if (_porter_duff_flags == PDF_DST_ATOP) {
									dr = (gr*ga + sr*ga);
									dg = (sg*ga + sg*ga);
									db = (sb*ga + sb*ga);
								} else if (_porter_duff_flags == PDF_ADD) {
									dr = sr*0xff + ga*0xff;
									dg = sg*0xff + ga*0xff;
									db = sb*0xff + ga*0xff;
									da = 0xff;
								} else if (_porter_duff_flags == PDF_XOR) {
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
#endif
}

}
