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

Graphics::Graphics(void *s):
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Graphics");

	_radians = 0.0;
	_translate.x = 0;
	_translate.y = 0;

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

	_color.red = 0x00;
	_color.green = 0x00;
	_color.blue = 0x00;
	_color.alpha = 0x00;

	_font = NULL;

	_line_type = RECT_LINE;
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
	surface->SetPorterDuff(surface, DSPD_SRC_OVER);

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
		DFBSurfaceDrawingFlags flags = (DFBSurfaceDrawingFlags)DSDRAW_SRC_PREMULTIPLY;

		if (_draw_flags == DF_NOFX) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_NOFX);
		} else if (_draw_flags == DF_BLEND) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_BLEND);
		} else if (_draw_flags == DF_XOR) {
			flags = (DFBSurfaceDrawingFlags)(flags | DSDRAW_XOR);
		}
			
		surface->SetDrawingFlags(surface, flags);
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

		surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)(DSBLIT_SRC_PREMULTIPLY | flags));
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

void Graphics::Clear(int r, int g, int b, int a)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->Clear(surface, r, g, b, a);
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

	int x = SCALE_TO_SCREEN(_translate.x+xp, _screen.width, _scale.width),
			y = SCALE_TO_SCREEN(_translate.y+yp, _screen.height, _scale.height),
			w = SCALE_TO_SCREEN(wp, _screen.width, _scale.width),
			h = SCALE_TO_SCREEN(hp, _screen.height, _scale.height);

	DFBRegion rgn;

	rgn.x1 = x;
	rgn.y1 = y;
	rgn.x2 = x+w;
	rgn.y2 = y+h;

	surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	// surface->Flip(surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_WAITFORSYNC));
#endif
}

struct jcolor_t Graphics::GetColor()
{
	return _color;
} 

void Graphics::SetColor(struct jcolor_t c)
{
	_color.red = c.red;
	_color.green = c.green;
	_color.blue = c.blue;
	_color.alpha = c.alpha;

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->SetColor(surface, _color.red, _color.green, _color.blue, _color.alpha);
#endif
} 

void Graphics::SetColor(uint32_t c)
{
	_color.red = (c >> 0x10) & 0xff;
	_color.green = (c >> 0x08) & 0xff;
	_color.blue = (c >> 0x00) & 0xff;
	_color.alpha = (c >> 0x18) & 0xff;

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->SetColor(surface, _color.red, _color.green, _color.blue, _color.alpha);
#endif
} 

void Graphics::SetColor(int r, int g, int b, int a)
{
	TRUNC_COLOR(r, g, b, a);

	_color.red = r;
	_color.green = g;
	_color.blue = b;
	_color.alpha = a;

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	surface->SetColor(surface, _color.red, _color.green, _color.blue, _color.alpha);
#endif
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

void Graphics::SetPixel(int xp, int yp, uint32_t pixel)
{
	SetRGB(xp, yp, pixel);
}

uint32_t Graphics::GetPixel(int xp, int yp)
{
	return GetRGB(xp, yp);
}

void Graphics::SetLineType(jline_type_t t)
{
	_line_type = t;
}

void Graphics::SetLineStyle(jline_style_t t)
{
	_line_style = t;
}

void Graphics::SetLineWidth(int size)
{
	_line_width = size;

	if (_line_width < 1) {
		_line_width = 1;
	}

	if ((_line_width%2) == 0) {
		_line_width++;
	}
}

jline_type_t Graphics::GetLineType()
{
	return _line_type;
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
		if (xp == xf) {
			if (yp < yf) {
				FillRectangle(xp-_line_width/2, yp, _line_width, yf-yp);
			} else {
				FillRectangle(xp-_line_width/2, yf, _line_width, yp-yf);
			}

			if (_line_type == ROUND_LINE) {
				FillArc(xp, yp, _line_width/2, _line_width/2, 0, 360);
				FillArc(xp, yf, _line_width/2, _line_width/2, 0, 360);
			}
		} else if (yp == yf) {
			if (xp < xf) {
				FillRectangle(xp, yp-_line_width/2, xf-xp, _line_width);
			} else {
				FillRectangle(xf, yp-_line_width/2, xp-xf, _line_width);
			}

			if (_line_type == ROUND_LINE) {
				FillArc(xp, yp, _line_width/2, _line_width/2, 0, 360);
				FillArc(xf, yp, _line_width/2, _line_width/2, 0, 360);
			}
		} else {
			double r = (_line_width),
						 dx = xf-xp,
						 dy = yf-yp,
						 d = sqrt((dx*dx)+(dy*dy));

			if (d < 1.0) {
				d = 1.0;
			}

			// FillTriangle(xp+r*dy/d, yp+r*dx/d, xp+r*dy/d+dx, yp+r*dx/d+dy, xp+r*dy/d, yp-r*dx/d);
			// FillTriangle(xp+r*dy/d+dx, yp+r*dx/d+dy, xp+r*dy/d, yp-r*dx/d, xp+r*dy/d+dx, yp-r*dx/d+dy);

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

			if (_line_type == ROUND_LINE) {
				FillArc(xp-1, yp-1, r1, r1, 0, 360);
				FillArc((int)(xp+dx), (int)(yp+dy), r1, r1, 0, 360);
			}
		}
	}
#endif
}

void Graphics::DrawBezierCurve(jpoint_t *points, int n_points)
{
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

	if (w <= 0) {
		w = 1;
	}

	if (h <= 0) {
		h = 1;
	}

	surface->FillRectangle(surface, x, y ,w, h);
#endif
}

void Graphics::DrawRectangle(int xp, int yp, int wp, int hp)
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

	surface->DrawRectangle(surface, x, y, w, h);

	if (_line_width >= 1) {
		for (int i=1; i<_line_width/2; i++) {
			surface->DrawRectangle(surface, x+i, y+i , w-2*i, h-2*i);
			surface->DrawRectangle(surface, x-i, y-i , w+2*i, h+2*i);
		}
	}
#endif
}

void Graphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (wp < 2*dx) {
		dx = wp/2;
	}

	if (hp < 2*dy) {
		dy = hp/2;
	}

	FillRectangle(xp+dx, yp, wp-2*dx, hp);
	FillRectangle(xp, yp+dy, dx, hp-2*dy);
	FillRectangle(xp+wp-dx, yp+dy, dx, hp-2*dy);

	FillTriangle(xp+dx, yp, xp+dx, yp+dy, xp, yp+dy);
	FillTriangle(xp+wp-dx, yp, xp+wp, yp+dy, xp+wp-dx, yp+dy);
	FillTriangle(xp, yp+hp-dy, xp+dx, yp+hp-dy, xp+dx, yp+hp);
	FillTriangle(xp+wp-dx, yp+hp, xp+wp-dx, yp+hp-dy, xp+wp, yp+hp-dy);
#endif
}

void Graphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (wp < 2*dx) {
		dx = wp/2;
	}

	if (hp < 2*dy) {
		dy = hp/2;
	}

	int r = _line_width/2;

	DrawLine(xp+dx, yp, xp+wp-dx, yp);
	DrawLine(xp+dx, yp+hp, xp+wp-dx, yp+hp);
	DrawLine(xp, yp+dy, xp, yp+hp-dy);
	DrawLine(xp+wp, yp+dy, xp+wp, yp+hp-dy);

	if (_line_width == 1) {
		DrawLine(xp+dx, yp, xp, yp+dy);
		DrawLine(xp+wp-dx, yp, xp+wp, yp+dy);
		DrawLine(xp, yp+hp-dy, xp+dx, yp+hp);
		DrawLine(xp+wp-dx, yp+hp, xp+wp, yp+hp-dy);
	} else {
		jgui::jpoint_t p1[] = {
			{0, dy+r},
			{dx+r, 0},
			{dx+r, 2*r},
			{2*r, dy+r}
		};

		FillPolygon(xp-r, yp-r, p1, 4);

		jgui::jpoint_t p2[] = {
			{0, 0},
			{dx+r, dy+r},
			{dx-r, dy+r},
			{0, 2*r}
		};

		FillPolygon(xp+wp-dx, yp-r, p2, 4);

		jgui::jpoint_t p3[] = {
			{0, 0},
			{2*r, 0},
			{r+dx, dy-r},
			{r+dx, r+dy}
		};

		FillPolygon(xp-r, yp+hp-dy, p3, 4);

		jgui::jpoint_t p4[] = {
			{dx-r, 0},
			{dx+r, 0},
			{0, dy+r},
			{0, dy-r}
		};

		FillPolygon(xp+wp-dx-2, yp+hp-dy-2, p4, 4);
	}
#endif
}

void Graphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int raio)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (wp < 2*raio) {
		raio = wp/2;
	}

	if (hp < 2*raio) {
		raio = hp/2;
	}

	jline_type_t type = _line_type;
	jline_style_t style = _line_style;

	_line_type = RECT_LINE;
	_line_style = SOLID_LINE;

	int line_width = _line_width;

	_line_width = 1;

	// INFO:: draw top
	{
		int x1, 
				x2,
				l = 0;

		for (int y1=yp-raio; y1<=yp; y1++) {
			double r = raio,
						 d = sqrt(r*r-(y1-yp)*(y1-yp));

			x1 = (int)(xp - d);
			x2 = (int)(xp + d);

			int j = SCALE_TO_SCREEN((_translate.y+y1+raio), _screen.height, _scale.height);

			if (l != j) {
				l = j;

				int px1 = SCALE_TO_SCREEN((_translate.x+x1+raio), _screen.width, _scale.width),
						px2 = SCALE_TO_SCREEN((_translate.x+x2+wp-raio), _screen.width, _scale.width);

				surface->DrawLine(surface, px1, l, px2, l);
			}
		}

		// INFO:: draw center
		{
			// INFO:: verifica se a ultima linha estah sobrepondo a primeira linha do retangulo
			int dy = yp+raio+1,
					dh = hp-2*raio-1,
					j = SCALE_TO_SCREEN((_translate.y+dy), _screen.height, _scale.height);

			if (l == j) {
				dy = dy + 1;
				dh = dh - 1;
			}

			FillRectangle(xp, dy, wp, dh);

			// FillRectangle(xp, yp+raio+1, wp, hp-2*raio-1);
		}

		// INFO:: draw bottop

		int dy = yp+hp-2*raio;

		for (int y1=dy; y1<=dy+raio; y1++) {
			double r = raio,
						 d = sqrt(r*r-(y1-dy)*(y1-dy));

			x1 = (int)(xp - d);
			x2 = (int)(xp + d);

			int j = SCALE_TO_SCREEN((_translate.y+y1+raio), _screen.height, _scale.height);

			if (l != j) {
				l = j;

				int px1 = SCALE_TO_SCREEN((_translate.x+x1+raio), _screen.width, _scale.width),
						px2 = SCALE_TO_SCREEN((_translate.x+x2+wp-raio), _screen.width, _scale.width);

				surface->DrawLine(surface, px1, l, px2, l);
			}
		}
	}

	_line_width = line_width;

	_line_type = type;
	_line_style = style;
#endif
}

void Graphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int raio)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (_line_width > raio) {
		raio = _line_width;
	}

	if (wp < 2*raio) {
		raio = wp/2;
	}

	if (hp < 2*raio) {
		raio = hp/2;
	}

	jline_type_t type = _line_type;
	jline_style_t style = _line_style;

	_line_type = RECT_LINE;
	_line_style = SOLID_LINE;

	DrawLine(xp, yp+raio+1, xp, yp+hp-raio);
	DrawLine(xp+wp, yp+raio+1, xp+wp, yp+hp-raio);

	{
		int mw = _line_width;

		if (mw > raio) {
			mw = raio;
		}

		if (mw > 1) {
			int l = 0;

			mw = mw/2;

			int min_y = SCALE_TO_SCREEN((_translate.y+yp+mw), _screen.height, _scale.height),
					max_y = SCALE_TO_SCREEN((_translate.y+yp+hp-mw), _screen.height, _scale.height);

			for (int y1=yp-raio-mw; y1<=yp; y1++) {
				double max_d = sqrt((raio+mw)*(raio+mw)-(y1-yp)*(y1-yp)),
							 min_d = (raio-mw)*(raio-mw)-(y1-yp)*(y1-yp);

				if (min_d < 0) {
					min_d = 0.0;
				} else {
					min_d = sqrt(min_d);
				}

				int max_x1 = (int)(xp-max_d),
						max_x2 = (int)(xp+max_d),
						min_x1 = (int)(xp-min_d),
						min_x2 = (int)(xp+min_d);

				int j = SCALE_TO_SCREEN((_translate.y+y1+raio), _screen.height, _scale.height);

				if (l != j) {
					l = j;

					int max_px1 = SCALE_TO_SCREEN((_translate.x+max_x1+raio), _screen.width, _scale.width),
							max_px2 = SCALE_TO_SCREEN((_translate.x+max_x2+wp-raio), _screen.width, _scale.width),
							min_px1 = SCALE_TO_SCREEN((_translate.x+min_x1+raio), _screen.width, _scale.width),
							min_px2 = SCALE_TO_SCREEN((_translate.x+min_x2+wp-raio), _screen.width, _scale.width);

					if (l <= min_y) {
						surface->DrawLine(surface, max_px1, l, max_px2, l);
					} else {
						surface->DrawLine(surface, max_px1, l, min_px1, l);
						surface->DrawLine(surface, max_px2, l, min_px2, l);
					}
				}
			}

			yp = yp + hp;

			for (int y1=yp; y1<=yp+raio+mw; y1++) {
				double max_d = sqrt((raio+mw)*(raio+mw)-(y1-yp)*(y1-yp)),
							 min_d = (raio-mw)*(raio-mw)-(y1-yp)*(y1-yp);

				if (min_d < 0) {
					min_d = 0.0;
				} else {
					min_d = sqrt(min_d);
				}

				int max_x1 = (int)(xp-max_d),
						max_x2 = (int)(xp+max_d),
						min_x1 = (int)(xp-min_d),
						min_x2 = (int)(xp+min_d);

				int j = SCALE_TO_SCREEN((_translate.y+y1-raio), _screen.height, _scale.height);

				if (l != j) {
					l = j;

					int max_px1 = SCALE_TO_SCREEN((_translate.x+max_x1+raio), _screen.width, _scale.width),
							max_px2 = SCALE_TO_SCREEN((_translate.x+max_x2+wp-raio), _screen.width, _scale.width),
							min_px1 = SCALE_TO_SCREEN((_translate.x+min_x1+raio), _screen.width, _scale.width),
							min_px2 = SCALE_TO_SCREEN((_translate.x+min_x2+wp-raio), _screen.width, _scale.width);

					if (l >= max_y) {
						surface->DrawLine(surface, max_px1, l, max_px2, l);
					} else {
						surface->DrawLine(surface, max_px1, l, min_px1, l);
						surface->DrawLine(surface, max_px2, l, min_px2, l);
					}
				}
			}
		} else {
			int l = 0;

			mw = 0;

			int old_max_px1 = -1,
					old_max_px2 = -1,
					old_l = -1;
			int limit = yp;

			for (int y1=yp-raio-mw; y1<=limit; y1++) {
				double min_d = sqrt((raio-mw)*(raio-mw)-(y1-yp)*(y1-yp));

				int max_x1 = (int)(xp-min_d),
						max_x2 = (int)(xp+min_d);

				int j = SCALE_TO_SCREEN((_translate.y+y1+raio), _screen.height, _scale.height);

				if (l != j || y1 == limit) {
					l = j;

					int max_px1 = SCALE_TO_SCREEN((_translate.x+max_x1+raio), _screen.width, _scale.width),
							max_px2 = SCALE_TO_SCREEN((_translate.x+max_x2+wp-raio), _screen.width, _scale.width);

					if (old_l > 0) {
						surface->DrawLine(surface, old_max_px1, old_l, max_px1, l);
						surface->DrawLine(surface, old_max_px2, old_l, max_px2, l);
					} else {
						surface->DrawLine(surface, max_px1, l, max_px2, l);
					}

					old_max_px1 = max_px1;
					old_max_px2 = max_px2;
					old_l = l;
				}
			}

			yp = yp+hp-2*raio;

			old_max_px1 = -1;
			old_max_px2 = -1;
			old_l = -1;
			limit = yp+raio;

			for (int y1=yp; y1<=limit; y1++) {
				double min_d = sqrt((raio-mw)*(raio-mw)-(y1-yp)*(y1-yp));

				int max_x1 = (int)(xp-min_d),
						max_x2 = (int)(xp+min_d);

				int j = SCALE_TO_SCREEN((_translate.y+y1+raio), _screen.height, _scale.height);

				if (l != j || y1 == limit) {
					l = j;

					int max_px1 = SCALE_TO_SCREEN((_translate.x+max_x1+raio), _screen.width, _scale.width),
							max_px2 = SCALE_TO_SCREEN((_translate.x+max_x2+wp-raio), _screen.width, _scale.width);

					if (y1 == limit) {
						surface->DrawLine(surface, old_max_px1, l, old_max_px2, l);
					} else if (old_l > 0) {
						surface->DrawLine(surface, old_max_px1, old_l, max_px1, l);
						surface->DrawLine(surface, old_max_px2, old_l, max_px2, l);
					}

					old_max_px1 = max_px1;
					old_max_px2 = max_px2;
					old_l = l;
				}
			}

		}
	}

	_line_type = type;
	_line_style = style;
#endif
}

void Graphics::FillCircle(int xp, int yp, int raio)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x1, 
			x2,
			l = 0,
			k = 0,
			nlines = 2*raio;

	DFBRegion lines[nlines];

	for (int y1=yp-raio; y1<=yp+raio; y1++) {
		double d = sqrt(raio*raio-(y1-yp)*(y1-yp));

		x1 = (int)(xp - d);
		x2 = (int)(xp + d);

		int j = SCALE_TO_SCREEN((_translate.y+y1), _screen.height, _scale.height);

		if (l != j) {
			l = j;

			int px1 = SCALE_TO_SCREEN((_translate.x+x1), _screen.width, _scale.width),
					px2 = SCALE_TO_SCREEN((_translate.x+x2), _screen.width, _scale.width);

			lines[k].x1 = px1;
			lines[k].y1 = l;
			lines[k].x2 = px2;
			lines[k].y2 = l;

			k++;
		}
	}
	
	surface->DrawLines(surface, lines, k-1);
#endif
}

void Graphics::DrawCircle(int xp, int yp, int raio)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int mw = _line_width;

	if (mw > raio) {
		mw = raio;
	}

	if (mw > 1) {
		int l = 0;

		mw = mw/2;

		int min_y = SCALE_TO_SCREEN((_translate.y+yp-raio+mw), _screen.height, _scale.height),
				max_y = SCALE_TO_SCREEN((_translate.y+yp+raio-mw), _screen.height, _scale.height);

		for (int y1=yp-raio-mw; y1<=yp+raio+mw; y1++) {
			double max_d = sqrt((raio+mw)*(raio+mw)-(y1-yp)*(y1-yp)),
						 min_d = (raio-mw)*(raio-mw)-(y1-yp)*(y1-yp);

			if (min_d < 0) {
				min_d = 0;
			} else {
				min_d = sqrt(min_d);
			}

			int max_x1 = (int)(xp-max_d),
					max_x2 = (int)(xp+max_d),
					min_x1 = (int)(xp-min_d),
					min_x2 = (int)(xp+min_d);

			int j = SCALE_TO_SCREEN((_translate.y+y1), _screen.height, _scale.height);

			if (l != j) {
				l = j;

				int max_px1 = SCALE_TO_SCREEN((_translate.x+max_x1), _screen.width, _scale.width),
						max_px2 = SCALE_TO_SCREEN((_translate.x+max_x2), _screen.width, _scale.width),
						min_px1 = SCALE_TO_SCREEN((_translate.x+min_x1), _screen.width, _scale.width),
						min_px2 = SCALE_TO_SCREEN((_translate.x+min_x2), _screen.width, _scale.width);

				if (l <= min_y || l >= max_y) {
					surface->DrawLine(surface, max_px1, l, max_px2, l);
				} else {
					surface->DrawLine(surface, max_px1, l, min_px1, l);
					surface->DrawLine(surface, max_px2, l, min_px2, l);
				}
			}
		}
	} else {
		int l = 0;

		mw = 0;

		int old_max_px1 = -1,
				old_max_px2 = -1,
				old_l = -1;
		int limit = yp+raio+mw;

		for (int y1=yp-raio-mw; y1<=limit; y1++) {
			double min_d = sqrt((raio-mw)*(raio-mw)-(y1-yp)*(y1-yp));

			int max_x1 = (int)(xp-min_d),
					max_x2 = (int)(xp+min_d);

			int j = SCALE_TO_SCREEN((_translate.y+y1), _screen.height, _scale.height);

			if (l != j || y1 == limit) {
				l = j;

				int max_px1 = SCALE_TO_SCREEN((_translate.x+max_x1), _screen.width, _scale.width),
						max_px2 = SCALE_TO_SCREEN((_translate.x+max_x2), _screen.width, _scale.width);

				if (old_l > 0) {
					surface->DrawLine(surface, old_max_px1, old_l, max_px1, l);
					surface->DrawLine(surface, old_max_px2, old_l, max_px2, l);
				}

				old_max_px1 = max_px1;
				old_max_px2 = max_px2;
				old_l = l;
			}
		}
	}
#endif
}

void Graphics::FillArc(int xc, int yc, int rx, int ry, double start_angle, double end_angle)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (start_angle > 360 || start_angle < 360) {
		start_angle = start_angle-((int)start_angle/360)*360;
	}

	if (end_angle > 360 || end_angle < 360) {
		end_angle = end_angle-((int)end_angle/360)*360;
	}

	if (start_angle < 0) {
		start_angle = 360+start_angle;
	}

	if (end_angle < 0) {
		end_angle = 360+end_angle;
	}

	jline_type_t type = _line_type;
	jline_style_t style = _line_style;

	_line_type = RECT_LINE;
	_line_style = SOLID_LINE;

	int line_width = _line_width;

	_line_width = 1;

	if (start_angle <= end_angle) {
		int r = _line_width;

		if (r > rx) {
			r = rx;
		}

		if (r > ry) {
			r = ry;
		}

		rx = rx + r/2; // + line_width/2;
		ry = ry + r/2; // + line_width/2;

		// while (r-- > 0) {
		double angle = start_angle*(M_PI/180),
					 range = end_angle*(M_PI/180),
					 x = (rx*cos(angle)),
					 y = (ry*sin(angle));
		double x1 = 0.0,
					 y1 = 0.0,
					 old_x1,
					 old_y1;

		old_x1 = (xc+x);
		old_y1 = (yc-y);

		do {
			x1 = (xc+x);
			y1 = (yc-y);

			FillTriangle((int)old_x1, (int)old_y1, (int)x1, (int)y1, (int)xc, (int)yc);

			old_x1 = x1;
			old_y1 = y1;

			angle += 0.01;

			x=(rx*cos(angle));
			y=(ry*sin(angle));
		} while(angle<range);

		x1 = (xc+x);
		y1 = (yc-y);

		FillTriangle((int)old_x1, (int)old_y1, (int)x1, (int)y1, (int)xc, (int)yc);

		// rx--;
		// ry--;
		// }
	} else {
		FillArc(xc, yc, rx, ry, start_angle, 360);
		FillArc(xc, yc, rx, ry, 0, end_angle);
	}

	_line_width = line_width;

	_line_type = type;
	_line_style = style;
#endif
}

void Graphics::DrawArc(int xc, int yc, int rx, int ry, double start_angle, double end_angle)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	if (start_angle > 360 || start_angle < 360) {
		start_angle = start_angle-((int)start_angle/360)*360;
	}

	if (end_angle > 360 || end_angle < 360) {
		end_angle = end_angle-((int)end_angle/360)*360;
	}

	if (start_angle < 0) {
		start_angle = 360-start_angle;
	}

	if (end_angle < 0) {
		end_angle = 360-end_angle;
	}

	jline_type_t type = _line_type;
	jline_style_t style = _line_style;

	_line_type = RECT_LINE;
	_line_style = SOLID_LINE;

	if (_line_width > 1) {
		int line_width = _line_width;

		_line_width = 1;

		if (start_angle <= end_angle) {
			int r = line_width;

			if (r > rx) {
				r = rx;
			}

			if (r > ry) {
				r = ry;
			}

			// while (r-- > 0) {
			double angle = start_angle*(M_PI/180),
						 range = end_angle*(M_PI/180),
						 step_angle = 0.1,
						 x = ((rx+r/2)*cos(angle)),
						 y = ((ry+r/2)*sin(angle)),
						 mxc = ((rx-r/2)*cos(angle)),
						 myc = ((ry-r/2)*sin(angle));
			double x1 = 0.0,
						 y1 = 0.0,
						 old_x1,
						 old_y1;

			old_x1 = (xc+x);
			old_y1 = (yc-y);

			do {
				x1 = (xc+x);
				y1 = (yc-y);
				mxc = (xc+mxc);
				myc = (yc-myc);

				FillTriangle((int)old_x1, (int)old_y1, (int)x1, (int)y1, (int)mxc, (int)myc);

				angle += step_angle;

				{
					double nxc=((rx-r/2)*cos(angle)),
								 nyc=((ry-r/2)*sin(angle));

					nxc = (xc+nxc);
					nyc = (yc-nyc);

					FillTriangle((int)mxc, (int)myc, (int)x1, (int)y1, (int)nxc, (int)nyc);
				}

				old_x1 = x1;
				old_y1 = y1;

				x=((rx+r/2)*cos(angle));
				y=((ry+r/2)*sin(angle));
				mxc=((rx-r/2)*cos(angle));
				myc=((ry-r/2)*sin(angle));
			} while(angle<range);

			x1 = (xc+x);
			y1 = (yc-y);
			mxc = (xc+mxc);
			myc = (yc-myc);

			FillTriangle((int)old_x1, (int)old_y1, (int)x1, (int)y1, (int)mxc, (int)myc);

			// rx--;
			// ry--;
			// }
		} else {
			DrawArc(xc, yc, rx, ry, start_angle, 360);
			DrawArc(xc, yc, rx, ry, 0, end_angle);
		}

		_line_width = line_width;
	} else {
		if (start_angle <= end_angle) {
			double angle = start_angle*(M_PI/180),
						 range = end_angle*(M_PI/180),
						 x = (rx*cos(angle)),
						 y = (ry*sin(angle));
			double x1 = 0.0,
						 y1 = 0.0,
						 old_x1,
						 old_y1;

			old_x1 = (xc+x);
			old_y1 = (yc-y);

			do {
				x1 = (xc+x);
				y1 = (yc-y);

				DrawLine((int)old_x1, (int)old_y1, (int)x1, (int)y1);

				old_x1 = x1;
				old_y1 = y1;

				angle += 0.1;

				x=(rx*cos(angle));
				y=(ry*sin(angle));
			} while(angle<=range);

			x1 = (xc+x);
			y1 = (yc-y);

			DrawLine((int)old_x1, (int)old_y1, (int)x1, (int)y1);
		} else {
			DrawArc(xc, yc, rx, ry, start_angle, 360);
			DrawArc(xc, yc, rx, ry, 0, end_angle);
		}
	}

	_line_type = type;
	_line_style = style;
#endif
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
	if (surface == NULL) {
		return;
	}

	DrawLine(x1p, y1p, x2p, y2p);
	DrawLine(x2p, y2p, x3p, y3p);
	DrawLine(x3p, y3p, x1p, y1p);
#endif
}

void Graphics::DrawPolygon(int x, int y, jpoint_t *p, int num, bool close)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int *xpoints = new int[num], 
			*ypoints = new int[num];

	for (int i=0; i<num; i++) {
		xpoints[i] = p[i].x;
		ypoints[i] = p[i].y;
	}

	int ox, oy, tx, ty;

	ox = xpoints[0] + x;
	oy = ypoints[0] + y;

	for (int i=1; i<num; i++) {
		tx = xpoints[i] + x;
		ty = ypoints[i] + y;

		DrawLine(ox, oy, tx, ty);

		ox = tx;
		oy = ty;
	}

	if (close) {
		DrawLine(ox, oy, xpoints[0] + x, ypoints[0] + y);
	}

	delete xpoints;
	delete ypoints;
#endif
}

void Graphics::FillPolygon(int x, int y, jpoint_t *p, int num)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int points[2*num];
	int i,
			j=0;

	for (i=0; i<num; i++) {
		points[j++] = p[i].x + x;
		points[j++] = p[i].y + y;
	}

	jline_type_t type = _line_type;
	jline_style_t style = _line_style;

	_line_type = RECT_LINE;
	_line_style = SOLID_LINE;

	int line_width = _line_width;

	_line_width = 1;

	Fill_polygon(num, points);

	_line_width = line_width;

	_line_type = type;
	_line_style = style;
#endif
}

void Graphics::FillGradientRectangle(int xp, int yp, int wp, int hp, jcolor_t scolor, jcolor_t dcolor, bool horizontal)
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

	int r1 = scolor.red; 
	int g1 = scolor.green; 
	int b1 = scolor.blue; 
	int a1 = scolor.alpha; 

	int line_width = _line_width;

	_line_width = 1;

	if (horizontal){
		double difr = (double) (dcolor.red - scolor.red) / h;
		double difg = (double) (dcolor.green - scolor.green) / h;
		double difb = (double) (dcolor.blue - scolor.blue) / h;
		double difa = (double) (dcolor.alpha - scolor.alpha) / h;
		for (int i = 0; i < h; i++){
			SetColor(r1 + (int) (difr*i), g1 + (int) (difg*i), b1 + (int) (difb*i), a1 + (int) (difa*i) );
			surface->DrawLine( surface, x, y + i, x + w - 1, y + i );
		}
	}else{
		double difr = (double) (dcolor.red - scolor.red) / w;
		double difg = (double) (dcolor.green - scolor.green) / w;
		double difb = (double) (dcolor.blue - scolor.blue) / w;
		double difa = (double) (dcolor.alpha - scolor.alpha) / w;
		for (int i = 0; i < w; i++){
			SetColor(r1 + (int) (difr*i), g1 + (int) (difg*i), b1 + (int) (difb*i), a1 + (int) (difa*i) );
			surface->DrawLine( surface, x + i, y, x + i, y + h - 1);
		}
	}

	_line_width = line_width;
#endif
}

void Graphics::DrawString(std::string s, int xp, int yp)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	// surface->DrawString(surface, s.c_str(), -1, x, y+_font->GetHeight(), (DFBSurfaceTextFlags)(DSTF_LEFT));
	surface->DrawString(surface, s.c_str(), -1, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));
#endif
}

void Graphics::DrawGlyph(int symbol, int xp, int yp)
{
#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	IDirectFBFont *font = (IDirectFBFont *)_font->GetFont();
	DFBRectangle glyphrect;
	DFBTextEncodingID enc;
	int glyphindex,
			glyphadvance;

	font->GetGlyphExtents(font, symbol, &glyphrect, &glyphadvance);
	font->FindEncoding(font, "UTF8", &enc);
	font->SetEncoding(font, enc);

	surface->SetFont(surface, NULL);      
	surface->SetFont(surface, font);

	surface->DrawGlyph(surface, symbol, x, y, (DFBSurfaceTextFlags)(DSTF_LEFT | DSTF_TOP));

	font->FindEncoding(font, "Latin1", &enc);
	font->SetEncoding(font, enc);

	surface->SetFont(surface, NULL);      
	surface->SetFont(surface, font);

	glyphindex = 0;
#endif
}


bool Graphics::DrawImage(std::string img, int xp, int yp, int alpha)
{
	int iwidth,
			iheight;

	if (GetImageSize(img, &iwidth, &iheight) != false) {
		return Graphics::DrawImage(img, 0, 0, iwidth, iheight, xp, yp, iwidth, iheight, alpha);
	}

	return false;
}

bool Graphics::DrawImage(std::string img, int xp, int yp, int wp, int hp, int alpha)
{
	int iwidth,
			iheight;

	if (GetImageSize(img, &iwidth, &iheight) != false) {
		return Graphics::DrawImage(img, 0, 0, iwidth, iheight, xp, yp, wp, hp, alpha);
	}

	return false;
}

bool Graphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int alpha)
{
	int iwidth,
			iheight;

	if (GetImageSize(img, &iwidth, &iheight) != false) {
		return Graphics::DrawImage(img, sxp, syp, swp, shp, xp, yp, swp, shp, alpha);
	}

	return false;
}

bool Graphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp, int alpha)
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

	int sx = SCALE_TO_SCREEN(sxp, _screen.width, _scale.width),
			sy = SCALE_TO_SCREEN(syp, _screen.height, _scale.height),
			sw = swp, // SCALE_TO_SCREEN(swp, _screen.width, _scale.width),
			sh = shp; // SCALE_TO_SCREEN(shp, _screen.height, _scale.height);
	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width),
			y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height),
			w = SCALE_TO_SCREEN((_translate.x+xp+wp), _screen.width, _scale.width)-x,
			h = SCALE_TO_SCREEN((_translate.y+yp+hp), _screen.height, _scale.height)-y;

	alpha = (alpha < 0)?0:(alpha > 0xff)?0xff:alpha;

	if (_radians != 0.0) {
		OffScreenImage off(wp, hp);
		Graphics *g = off.GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color.red, _color.green, _color.blue, alpha);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0, wp, hp, 0xff);
		
		RotateImage(&off, -_translate.x, -_translate.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, alpha);

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

	imgSurface->SetBlittingFlags(imgSurface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
	imgSurface->SetDrawingFlags(imgSurface, (DFBSurfaceDrawingFlags)(DSDRAW_NOFX));
	imgSurface->SetPorterDuff(imgSurface, DSPD_NONE);

	imgSurface->Clear(imgSurface, 0x00, 0x00, 0x00, 0x00);
	
	if (imgProvider->RenderTo(imgProvider, imgSurface, NULL) != DFB_OK) {
		imgProvider->Release(imgProvider);
		imgSurface->Release(imgSurface);

		return false;
	}

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

	DFBRectangle srect,
							 drect;

	srect.x = sx;
	srect.y = sy;
	srect.w = w;
	srect.h = h;

	drect.x = x;
	drect.y = y;
	drect.w = w;
	drect.h = h;

	surface->SetColor(surface, _color.red, _color.green, _color.blue, alpha);
	surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)(DSBLIT_DST_PREMULTIPLY | flags));
	surface->StretchBlit(surface, imgSurface, &srect, &drect);
	surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)(DSBLIT_SRC_PREMULTIPLY | flags));
	surface->SetColor(surface, _color.red, _color.green, _color.blue, _color.alpha);

	imgProvider->Release(imgProvider);
	imgSurface->Release(imgSurface);
#endif

	return true;
}

bool Graphics::DrawImage(OffScreenImage *img, int xp, int yp, int alpha)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, img->GetWidth(), img->GetHeight(), alpha);
}

bool Graphics::DrawImage(OffScreenImage *img, int xp, int yp, int wp, int hp, int alpha)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp, alpha);
}

bool Graphics::DrawImage(OffScreenImage *img, int sxp, int syp, int swp, int shp, int xp, int yp, int alpha)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, sxp, syp, swp, shp, xp, yp, swp, shp, alpha);
}

bool Graphics::DrawImage(OffScreenImage *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp, int alpha)
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

	alpha = (alpha < 0)?0:(alpha > 0xff)?0xff:alpha;

	if (_radians != 0.0) {
		OffScreenImage off(wp, hp);
		
		Graphics *g = off.GetGraphics();

		g->SetBlittingFlags(_blit_flags);
		g->SetColor(_color.red, _color.green, _color.blue, alpha);
		g->DrawImage(img, sxp, syp, swp, shp, 0, 0, wp, hp, 0xff);

		RotateImage(&off, -_translate.x, -_translate.y, xp+_translate.x, yp+_translate.y, wp, hp, _radians, alpha);

		return true;
	}

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

	surface->SetColor(surface, _color.red, _color.green, _color.blue, alpha);
	surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)(DSBLIT_DST_PREMULTIPLY | flags));
	surface->StretchBlit(surface, g->surface, &srect, &drect);
	surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)(DSBLIT_SRC_PREMULTIPLY | flags));
	surface->SetColor(surface, _color.red, _color.green, _color.blue, _color.alpha);
#endif

	return true;
}

void Graphics::Rotate(double radians)
{
	_radians = fmod(radians, 2.0*jmath::Math<double>::PI);
}

void Graphics::Translate(int x, int y)
{
	_translate.x += x;
	_translate.y += y;
}

double Graphics::Rotate()
{
	return _radians;
}

jpoint_t Graphics::Translate()
{
	return _translate;
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

void Graphics::SetRGB(int xp, int yp, uint32_t rgb) 
{
	if (xp < 0 || yp < 0) {
		return;
	}

#ifdef DIRECTFB_UI
	if (surface == NULL) {
		return;
	}

	int x = SCALE_TO_SCREEN((_translate.x+xp), _screen.width, _scale.width); 
	int y = SCALE_TO_SCREEN((_translate.y+yp), _screen.height, _scale.height);

	void *ptr;
	uint32_t *dst;
	int pitch;
	int swmax,
			shmax;

	surface->GetSize(surface, &swmax, &shmax);

	if ((x < 0 || x >= swmax) || (y < 0 || y >= shmax)) {
		return;
	}

	// surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &ptr, &pitch);
	surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_WRITE), &ptr, &pitch);

	dst = (uint32_t *)((uint8_t *)ptr + y * pitch);

	if (_draw_flags == DF_NOFX) {
		*(dst+x) = rgb;
	} else if (_draw_flags == DF_BLEND) {
		int pixel = *(dst+x),
				r = (rgb>>0x10)&0xff,
				g = (rgb>>0x08)&0xff,
				b = (rgb>>0x00)&0xff,
				a = (rgb>>0x18)&0xff,
				pr = (pixel>>0x10)&0xff,
				pg = (pixel>>0x08)&0xff,
				pb = (pixel>>0x00)&0xff;

		pr = (pr*(255-a) + r*a) >> 8;
		pg = (pg*(255-a) + g*a) >> 8;
		pb = (pb*(255-a) + b*a) >> 8;

		*(dst+x) = 0xff000000 | (pr << 0x10) | (pg << 0x08) | (pb << 0x00);
	} else if (_draw_flags == DF_XOR) {
		*(dst+x) ^= rgb;
	}

	surface->Unlock(surface);
#endif
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
	_font = NULL;

	_color.red = 0x00;
	_color.green = 0x00;
	_color.blue = 0x00;
	_color.alpha = 0x00;

	_radians = 0.0;
	// _translate.x = 0;
	// _translate.y = 0;
	_line_width = 1;
	_line_type = RECT_LINE;
	_line_style = SOLID_LINE;

	SetDrawingFlags(DF_BLEND);
	SetBlittingFlags((jblitting_flags_t)(BF_ALPHACHANNEL)); // | BF_COLORALPHA));
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
void Graphics::insertEdge(edge_t *list, edge_t *edge)
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

void Graphics::makeEdgeRec(struct jpoint_t lower, struct jpoint_t upper, int yComp, edge_t *edge, edge_t *edges[])
{
	edge->dxPerScan = ((double)(upper.x-lower.x)/(double)(upper.y-lower.y));
	edge->xIntersect = lower.x;

	if(upper.y<yComp) {
		edge->yUpper = (upper.y-1);
	} else {
		edge->yUpper = upper.y;
	}

	insertEdge(edges[(int)lower.y], edge);
}

void Graphics::fillScan(int scan, edge_t *active)
{
	if (active == NULL) {
		return;
	}

	edge_t *p1,
				 *p2;
	uint32_t color;
	int count;

	p1 = active->next;

	color = _color.alpha << 0x18 | _color.red << 0x10 | _color.green << 0x08 | _color.blue << 0x00;

	while (p1 != NULL) {
		p2 = p1->next;

		for (count=(int)p1->xIntersect; p2!=NULL && count<=(int)p2->xIntersect; count++) {
			SetPixel(count, scan, color);
		}

		if (p2 != NULL) {
			p1=p2->next;
		} else {
			break;
		}
	}
}

int Graphics::yNext(int k, int cnt, jpoint_t pts[])
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


void Graphics::buildEdgeList(int cnt, jpoint_t pts[], edge_t *edges[])
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
				makeEdgeRec(v1, v2, yNext(count,cnt,pts), edge,edges);
			} else {
				makeEdgeRec(v2, v1, yPrev, edge, edges);
			}
		}

		yPrev = v1.y;
		v1 = v2;
	}

}

void Graphics::updateActiveList(int scan, edge_t *active)
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

void Graphics::Polygon(int n, int coordinates[])
{
	int count;

	if (n >= 2) {
		DrawLine(coordinates[0],coordinates[1], coordinates[2],coordinates[3]);

		for(count=1;count<(n-1);count++) {
			DrawLine(coordinates[(count*2)], coordinates[((count*2)+1)], coordinates[((count+1)*2)], coordinates[(((count+1)*2)+1)]);
		}
	}
}

void Graphics::Fill_polygon(int n, int ppts[])
{
	const int max_points = 10*1024;

	edge_t *edges[max_points],
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

	buildEdgeList(n, pts, edges);

	active = new edge_t;
	active->next = NULL;

	for (count_3=0; count_3<max_points; count_3++) {
		// build active list
		edge_t *p,
					 *q;

		p=edges[count_3]->next;

		while(p) {
			q = p->next;

			insertEdge(active, p);

			p = q;
		}

		if (active->next) {
			fillScan(count_3, active);
			updateActiveList(count_3, active);

			// resort active list
			edge_t *q,
						 *p = active->next;

			active->next = NULL;

			while (p) {
				q = p->next;

				insertEdge(active, p);

				p = q;
			}
		}
	}

	Polygon(n, ppts);

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
#endif

void Graphics::RotateImage(OffScreenImage *img, int xc, int yc, int x, int y, int width, int height, double angle, uint8_t alpha)
{
#ifdef DIRECTFB_UI
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

	int w2 = width/2,
			h2 = height/2,
			dw = width,
			dh = height,
			size = width;

	if (xc == 0 && yc == 0) {
		if (height > width) {
			dw = height;
		}

		if (width > height) {
			dh = width;
		}

		dw = dw+w2+xc-width;
		dh = dh+h2+yc-height;
	} else {

		if (height > width) {
			size = height;
		}

		dw = size+2*(w2+h2)+xc;
		dh = size+2*(w2+w2)+yc;
	}

	xc = xc + w2;
	yc = yc + h2;

	x = x; // - w2;
	y = y; // - h2;

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
	int scalew = precision*((double)_screen.width/(double)_scale.width),
			scaleh = precision*((double)_screen.height/(double)_scale.height);

	surface->GetSize(surface, &swmax, &shmax);
	simg->GetSize(simg, &iwmax, &ihmax);

	surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &sptr, &spitch);
	simg->Lock(simg, (DFBSurfaceLockFlags)(DSLF_READ | DSLF_WRITE), &gptr, &gpitch);

	int old_x = -1,
			old_y = -1;

	for (j=height-1+2*dh; j>0; j--) {
		int sy = ((y+j-dh)*scaleh)/precision;

		if (sy == old_y) {
			continue;
		}

		old_y = sy;

		jPrime = j - height - dh;

		if (sy >=0 && sy < shmax) {
			sdst = (uint32_t *)((uint8_t *)sptr + sy * spitch);

			for (i=width-1+2*dw; i>0; i--) {
				iPrime = i - width - dw;
			
				iOriginal = width + ((iPrime+xc)*cosTheta - (jPrime+yc)*sinTheta)/precision;
				jOriginal = height + ((iPrime+xc)*sinTheta + (jPrime+yc)*cosTheta)/precision;

				if ((iOriginal >= xc) && ((iOriginal-xc) < width) && (jOriginal >= yc) && ((jOriginal-yc) < height)) {
					int gx = ((iOriginal-xc)*scalew)/precision;
					int gy = ((jOriginal-yc)*scaleh)/precision;

					if ((gx >= 0 && gx < iwmax) && (gy >= 0 && gy < ihmax)) {
						int offset = ((x+i-dw)*scalew)/precision;

						if (offset == old_x) {
							continue;
						}

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

							// INFO:: needed
							sr += 8;
							sg += 8;
							sb += 8;
							sa += 8;
							gr += 8;
							gg += 8;
							gb += 8;
							ga = 0xff;
							
							if (_blit_flags & BF_COLORALPHA) {
								sa = alpha;
								ga = (ga*(0xff-alpha)) >> 8;
							}

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
