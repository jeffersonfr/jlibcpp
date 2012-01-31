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
	return NULL;
}

void NullGraphics::SetNativeSurface(void *surface)
{
}

void NullGraphics::SetWorkingScreenSize(int width, int height)
{
	JDEBUG(JINFO, "called\n");
}

jsize_t NullGraphics::GetWorkingScreenSize()
{
	JDEBUG(JINFO, "called\n");

	jsize_t t;

	t.width = 0;
	t.height = 0;

	return t;
}

jregion_t NullGraphics::ClipRect(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void NullGraphics::SetClip(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");
}

jregion_t NullGraphics::GetClip()
{
	JDEBUG(JINFO, "called\n");

	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void NullGraphics::ReleaseClip()
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Clear(int red, int green, int blue, int alpha)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Idle()
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Flip()
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Flip(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");
}

Color & NullGraphics::GetColor()
{
	JDEBUG(JINFO, "called\n");

	return _color;
}

void NullGraphics::SetColor(const Color &color)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetColor(uint32_t color)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetColor(int red, int green, int blue, int alpha)
{
	JDEBUG(JINFO, "called\n");
} 

bool NullGraphics::HasFont()
{
	JDEBUG(JINFO, "called\n");

	return false;
}

void NullGraphics::SetFont(Font *font)
{
	JDEBUG(JINFO, "called\n");
}

Font * NullGraphics::GetFont()
{
	JDEBUG(JINFO, "called\n");

	return NULL;
}

void NullGraphics::SetAntiAliasEnabled(bool b)
{
	JDEBUG(JINFO, "called\n");
}

jdrawing_flags_t NullGraphics::GetDrawingFlags()
{
	JDEBUG(JINFO, "called\n");
	
	return JDF_NOFX;
}

jblitting_flags_t NullGraphics::GetBlittingFlags()
{
	JDEBUG(JINFO, "called\n");

	return JBF_NOFX;
}

jdrawing_mode_t NullGraphics::GetDrawingMode()
{
	JDEBUG(JINFO, "called\n");

	return JDM_STROKE;
}

jporterduff_flags_t NullGraphics::GetPorterDuffFlags()
{
	JDEBUG(JINFO, "called\n");

	return JPF_NONE;
}

void NullGraphics::SetPorterDuffFlags(jporterduff_flags_t t)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetDrawingFlags(jdrawing_flags_t t)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetBlittingFlags(jblitting_flags_t t)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetDrawingMode(jdrawing_mode_t t)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetPixel(int xp, int yp, uint32_t pixel)
{
	JDEBUG(JINFO, "called\n");
}

unsigned int NullGraphics::GetPixel(int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

void NullGraphics::SetLineJoin(jline_join_t t)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetLineStyle(jline_style_t t)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetLineWidth(int size)
{
	JDEBUG(JINFO, "called\n");
}

jline_join_t NullGraphics::GetLineJoin()
{
	JDEBUG(JINFO, "called\n");

	return (jline_join_t)0;
}

jline_style_t NullGraphics::GetLineStyle()
{
	JDEBUG(JINFO, "called\n");

	return (jline_style_t)0;
}

int NullGraphics::GetLineWidth()
{
	return 0;
}

void NullGraphics::DrawLine(int xp, int yp, int xf, int yf)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillRectangle(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillCircle(int xp, int yp, int raio)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawCircle(int xp, int yp, int raio)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
	JDEBUG(JINFO, "called\n");
}
		
void NullGraphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
	JDEBUG(JINFO, "called\n");
}
		
void NullGraphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
}
		
void NullGraphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
}
		
void NullGraphics::FillArc(int xc, int yc, int rx, int ry, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawArc(int xc, int yc, int rx, int ry, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	JDEBUG(JINFO, "called\n");
}
		
void NullGraphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool close)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillRadialGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillHorizontalGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillVerticalGradient(int xp, int yp, int wp, int hp, Color &scolor, Color &dcolor)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::GetStringBreak(std::vector<std::string> *lines, std::string text, int wp, int hp, jhorizontal_align_t halign)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawString(std::string s, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawString(std::string full_text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");
}

bool NullGraphics::DrawImage(std::string img, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(std::string img, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(Image *img, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

bool NullGraphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	JDEBUG(JINFO, "called\n");

	return false;
}

void NullGraphics::Rotate(double radians)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Translate(int x, int y)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::TranslateImage(int x, int y)
{
	JDEBUG(JINFO, "called\n");
}

double NullGraphics::Rotate()
{
	JDEBUG(JINFO, "called\n");

	return 0.0;
}

jpoint_t NullGraphics::Translate()
{
	JDEBUG(JINFO, "called\n");

	jpoint_t t;

	t.x = 0;
	t.y = 0;

	return t;
}

jpoint_t NullGraphics::TranslateImage()
{
	JDEBUG(JINFO, "called\n");

	jpoint_t t;

	t.x = 0;
	t.y = 0;

	return t;
}

uint32_t NullGraphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	JDEBUG(JINFO, "called\n");

	return pixel;
}

void NullGraphics::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp, int scansize)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetRGB(uint32_t rgb, int xp, int yp) 
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp, int scanline) 
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Reset()
{
	JDEBUG(JINFO, "called\n");
}

}

