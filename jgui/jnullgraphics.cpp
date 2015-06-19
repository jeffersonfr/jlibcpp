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
#include "jnullgraphics.h"
#include "jdebug.h"

namespace jgui {

NullGraphics::NullGraphics():
	Graphics()
{
	jcommon::Object::SetClassName("jgui::NullGraphics");

	JDEBUG(JINFO, "called\n");
}

NullGraphics::~NullGraphics()
{
	JDEBUG(JINFO, "called\n");
}

void * NullGraphics::GetNativeSurface()
{
	return Graphics::GetNativeSurface();
}

void NullGraphics::SetNativeSurface(void *data, int wp, int hp)
{
	Graphics::SetNativeSurface(data, wp, hp);
}

jregion_t NullGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return Graphics::ClipRect(xp, yp, wp, hp);
}

void NullGraphics::SetClip(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetClip(xp, yp, wp, hp);
}

jregion_t NullGraphics::GetClip()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetClip();
}

void NullGraphics::ReleaseClip()
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::ReleaseClip();
}

void NullGraphics::Clear()
{
	JDEBUG(JINFO, "called\n");

	Graphics::Clear();
}

void NullGraphics::Clear(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::Clear(xp, yp, wp, hp);
}

void NullGraphics::Idle()
{
	JDEBUG(JINFO, "called\n");

	Graphics::Idle();
}

void NullGraphics::Flip()
{
	JDEBUG(JINFO, "called\n");

	Graphics::Flip();
}

void NullGraphics::Flip(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::Flip(xp, yp, wp, hp);
}

Color & NullGraphics::GetColor()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetColor();
}

void NullGraphics::SetColor(const Color &color)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetColor(color);
}

void NullGraphics::SetColor(uint32_t color)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetColor(color);
}

void NullGraphics::SetColor(int red, int green, int blue, int alpha)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetColor(red, green, blue, alpha);
} 

bool NullGraphics::HasFont()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::HasFont();
}

void NullGraphics::SetFont(Font *font)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetFont(font);
}

Font * NullGraphics::GetFont()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetFont();
}

void NullGraphics::SetAntialias(jantialias_mode_t mode)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetAntialias(mode);
}

jantialias_mode_t NullGraphics::GetAntialias()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetAntialias();
}

jdrawing_mode_t NullGraphics::GetDrawingMode()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetDrawingMode();
}

jcomposite_flags_t NullGraphics::GetCompositeFlags()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetCompositeFlags();
}

void NullGraphics::SetCompositeFlags(jcomposite_flags_t t)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetCompositeFlags(t);
}

void NullGraphics::SetDrawingMode(jdrawing_mode_t t)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetDrawingMode(t);
}

void NullGraphics::SetLineJoin(jline_join_t t)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetLineJoin(t);
}

void NullGraphics::SetLineStyle(jline_style_t t)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetLineStyle(t);
}

void NullGraphics::SetLineWidth(int size)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetLineWidth(size);
}

void NullGraphics::SetLineDash(double *dashes, int ndashes)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetLineDash(dashes, ndashes);
}

jline_join_t NullGraphics::GetLineJoin()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetLineJoin();
}

jline_style_t NullGraphics::GetLineStyle()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetLineStyle();
}

int NullGraphics::GetLineWidth()
{
	return Graphics::GetLineWidth();
}

void NullGraphics::DrawLine(int xp, int yp, int xf, int yf)
{
	JDEBUG(JINFO, "called\n");

	Graphics::DrawLine(xp, yp, xf, yf);
}

void NullGraphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
	JDEBUG(JINFO, "called\n");

	Graphics::DrawBezierCurve(p, npoints, interpolation);
}

void NullGraphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillRectangle(xp, yp, wp, hp);
}

void NullGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawRectangle(xp, yp, wp, hp);
}

void NullGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::FillBevelRectangle(xp, yp, wp, hp, dx, dy, corners);
}

void NullGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawBevelRectangle(xp, yp, wp, hp, dx, dy, corners);
}

void NullGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillRoundRectangle(xp, yp, wp, hp, dx, dy, corners);
}

void NullGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawRoundRectangle(xp, yp, wp, hp, dx, dy, corners);
}

void NullGraphics::FillCircle(int xp, int yp, int raio)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillCircle(xp, yp, raio);
}

void NullGraphics::DrawCircle(int xp, int yp, int raio)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawCircle(xp, yp, raio);
}

void NullGraphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillEllipse(xcp, ycp, rxp, ryp);
}
		
void NullGraphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawEllipse(xcp, ycp, rxp, ryp);
}
		
void NullGraphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillChord(xcp, ycp, rxp, ryp, arc0, arc1);
}
		
void NullGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawChord(xcp, ycp, rxp, ryp, arc0, arc1);
}
		
void NullGraphics::FillArc(int xc, int yc, int rx, int ry, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillArc(xc, yc, rx, ry, arc0, arc1);
}

void NullGraphics::DrawArc(int xc, int yc, int rx, int ry, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawArc(xc, yc, rx, ry, arc0, arc1);
}

void NullGraphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillPie(xcp, ycp, rxp, ryp, arc0, arc1);
}

void NullGraphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawPie(xcp, ycp, rxp, ryp, arc0, arc1);
}
		
void NullGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillTriangle(x1p, y1p, x2p, y2p, x3p, y3p);
}

void NullGraphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::DrawTriangle(x1p, y1p, x2p, y2p, x3p, y3p);
}

void NullGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillPolygon(xp, yp, p, npoints, even_odd);
}

void NullGraphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool closed)
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::FillPolygon(xp, yp, p, npoints, closed);
}

void NullGraphics::SetGradientStop(double stop, const Color &color)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetGradientStop(stop, color);
}

void NullGraphics::ResetGradientStop()
{
	JDEBUG(JINFO, "called\n");
	
	Graphics::ResetGradientStop();
}

void NullGraphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillRadialGradient(xcp, ycp, wp, hp, x0p, y0p, r0p);
}

void NullGraphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
	JDEBUG(JINFO, "called\n");

	Graphics::FillLinearGradient(xp, yp, wp, hp, x1p, y1p, x2p, y2p);
}

jregion_t NullGraphics::GetStringExtends(std::string text)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

jregion_t NullGraphics::GetGlyphExtends(int symbol)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void NullGraphics::DrawString(std::string s, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::DrawString(s, xp, yp);
}

void NullGraphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
	JDEBUG(JINFO, "called\n");

	Graphics::DrawString(text, xp, yp, wp, hp, halign, valign, clipped);
}

void NullGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::DrawGlyph(symbol, xp, yp);
}

bool NullGraphics::DrawImage(Image *img, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return Graphics::DrawImage(img, xp, yp);
}

bool NullGraphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return Graphics::DrawImage(img, xp, yp, wp, hp);
}

bool NullGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return Graphics::DrawImage(img, sxp, syp, swp, shp, xp, yp);
}

bool NullGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return Graphics::DrawImage(img, sxp, syp, swp, shp, xp, yp, wp, hp);
}

void NullGraphics::Translate(int x, int y)
{
	JDEBUG(JINFO, "called\n");

	Graphics::Translate(x, y);
}

jpoint_t NullGraphics::Translate()
{
	JDEBUG(JINFO, "called\n");

	return Graphics::Translate();
}

uint32_t NullGraphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	JDEBUG(JINFO, "called\n");

	return Graphics::GetRGB(xp, yp, pixel);
}

void NullGraphics::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::GetRGB(rgb, xp, yp, wp, hp);
}

void NullGraphics::SetRGB(uint32_t rgb, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetRGB(rgb, xp, yp);
}

void NullGraphics::SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	Graphics::SetRGB(rgb, xp, yp, wp, hp);
}

void NullGraphics::Reset()
{
	JDEBUG(JINFO, "called\n");

	Graphics::Reset();
}

}

