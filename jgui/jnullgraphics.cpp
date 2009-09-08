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
#include "jnullgraphics.h"
#include "jcommonlib.h"

namespace jgui {

NullGraphics::NullGraphics():
	Graphics(NULL)
{
	jcommon::Object::SetClassName("jgui::NullGraphics");

	JDEBUG(JINFO, "called\n");
}

NullGraphics::~NullGraphics()
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetCurrentWorkingScreenSize(int width, int height)
{
	JDEBUG(JINFO, "called\n");
}

OffScreenImage * NullGraphics::Create()
{
	JDEBUG(JINFO, "called\n");

	return NULL;
}

void NullGraphics::SetClip(int x1, int y1, int width, int height)
{
	JDEBUG(JINFO, "called\n");
}

jregion_t NullGraphics::GetClipBounds()
{
	JDEBUG(JINFO, "called\n");

	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

int NullGraphics::GetClipX()
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

int NullGraphics::GetClipY()
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

int NullGraphics::GetClipWidth()
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

int NullGraphics::GetClipHeight()
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

void NullGraphics::ReleaseClip()
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Clear(int r, int g, int b, int a)
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

uint32_t NullGraphics::GetColor()
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

void NullGraphics::SetColor(uint32_t color)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetColor(int r, int g, int b, int a)
{
	JDEBUG(JINFO, "called\n");
} 

void NullGraphics::SetFont(Font *font)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetPorterDuffFlags(jporter_duff_flags_t t)
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

void NullGraphics::SetPixel(int xp, int yp, uint32_t pixel)
{
	JDEBUG(JINFO, "called\n");
}

unsigned int NullGraphics::GetPixel(int xp, int yp)
{
	JDEBUG(JINFO, "called\n");

	return 0;
}

void NullGraphics::SetLineType(jline_type_t t)
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

jline_type_t NullGraphics::GetLineType()
{
	JDEBUG(JINFO, "called\n");

	return (jline_type_t)0;
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

void NullGraphics::DrawBezierCurve(jpoint_t *points, int n_points)
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

void NullGraphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int raio)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int raio)
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

void NullGraphics::FillArc(int xc, int yc, int rx, int ry, double start_angle, double end_angle)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawArc(int xc, int yc, int rx, int ry, double start_angle, double end_angle)
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

void NullGraphics::DrawPolygon(int x, int y, jpoint_t *p, int num, bool close)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillPolygon(int x, int y, jpoint_t *p, int num)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::FillGradientRectangle(int xp, int yp, int wp, int hp, int sr, int sg, int sb, int sa, int dr, int dg, int db, int da, bool horizontal)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawString(std::string s, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::DrawGlyph(int symbol, int xp, int yp)
{
	JDEBUG(JINFO, "called\n");
}

bool NullGraphics::GetImageSize(std::string img, int *real_width, int *real_height, int *scaled_width, int *scaled_height)
{
	JDEBUG(JINFO, "called\n");

	if (real_width != NULL) {
		*real_width = -1;
	}

	if (real_height != NULL) {
		*real_height = -1;
	}

	if (scaled_width != NULL) {
		*scaled_width = -1;
	}

	if (scaled_height != NULL) {
		*scaled_height = -1;
	}

	return true;
}

bool NullGraphics::DrawImage(std::string img, int x, int y, int alpha)
{
	JDEBUG(JINFO, "called\n");

	return true;
}

bool NullGraphics::DrawImage(std::string img, int xp, int yp, int wp, int hp, int alpha)
{
	JDEBUG(JINFO, "called\n");

	return true;
}

bool NullGraphics::DrawImage(std::string img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha)
{
	JDEBUG(JINFO, "called\n");

	return true;
}

bool NullGraphics::DrawImage(OffScreenImage *img, int x, int y, int w, int h, int alpha)
{
	JDEBUG(JINFO, "called\n");

	return true;
}

bool NullGraphics::DrawImage(OffScreenImage *img, int sx, int sy, int sw, int sh, int x, int y, int w, int h, int alpha)
{
	JDEBUG(JINFO, "called\n");

	return true;
}

void NullGraphics::Rotate(double radians)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Translate(int x, int y)
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

void NullGraphics::DrawStringJustified(std::string full_text, int x, int y, int width, int height, int align)
{
	JDEBUG(JINFO, "called\n");
}

uint32_t NullGraphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	JDEBUG(JINFO, "called\n");

	return pixel;
}

void NullGraphics::GetRGBArray(int startxp, int startyp, int wp, int hp, unsigned int **rgb, int offset, int scansize)
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetRGB(int xp, int yp, int rgb) 
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::SetRGB(unsigned int *rgb, int x, int y, int w, int h, int scanline) 
{
	JDEBUG(JINFO, "called\n");
}

void NullGraphics::Reset()
{
	JDEBUG(JINFO, "called\n");
}

}

