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
#include "jfont.h"
#include "jgfxhandler.h"

namespace jgui {

Graphics::Graphics():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Graphics");

	_font = NULL;

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = DEFAULT_SCALE_WIDTH;
	_scale.height = DEFAULT_SCALE_HEIGHT;
	
	_radians = 0.0;

	_translate.x = 0;
	_translate.y = 0;
	_translate_image.x = 0;
	_translate_image.y = 0;
}

Graphics::~Graphics()
{
}

void * Graphics::GetNativeSurface()
{
	return NULL;
}

void Graphics::SetNativeSurface(void *addr)
{
}

jregion_t Graphics::ClipRect(int xp, int yp, int wp, int hp)
{
	jregion_t t;
	
	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void Graphics::SetClip(int xp, int yp, int wp, int hp)
{
}

jregion_t Graphics::GetClip()
{
	jregion_t t;
	
	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void Graphics::ReleaseClip()
{
}

void Graphics::SetCompositeFlags(jcomposite_flags_t t)
{
}

void Graphics::SetDrawingFlags(jdrawing_flags_t t)
{
}

void Graphics::SetBlittingFlags(jblitting_flags_t t)
{
}

void Graphics::SetDrawingMode(jdrawing_mode_t t)
{
}

jcomposite_flags_t Graphics::GetCompositeFlags()
{
	return JCF_NONE;
}

jdrawing_flags_t Graphics::GetDrawingFlags()
{
	return JDF_NOFX;
}

jblitting_flags_t Graphics::GetBlittingFlags()
{
	return JBF_NOFX;
}

jdrawing_mode_t Graphics::GetDrawingMode()
{
	return JDM_STROKE;
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

jsize_t Graphics::GetWorkingScreenSize()
{
	return _scale;
}

void Graphics::Clear(int red, int green, int blue, int alpha)
{
}

void Graphics::Idle()
{
}

void Graphics::Flip()
{
}

void Graphics::Flip(int xp, int yp, int wp, int hp)
{
}

Color & Graphics::GetColor()
{
	return _color;
} 

void Graphics::SetColor(const Color &color)
{
	_color = color;
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
}

Font * Graphics::GetFont()
{
	return _font;
}

void Graphics::SetAntialias(bool b)
{
}

void Graphics::SetPixel(int xp, int yp, uint32_t pixel)
{
	SetRGB(pixel, xp+_translate.x, yp+_translate.y);
}

uint32_t Graphics::GetPixel(int xp, int yp)
{
	return GetRGB(xp, yp);
}

void Graphics::SetLineJoin(jline_join_t t)
{
}

void Graphics::SetLineStyle(jline_style_t t)
{
}

void Graphics::SetLineWidth(int size)
{
}

void Graphics::SetLineDash(double *dashes, int ndashes)
{
}

jline_join_t Graphics::GetLineJoin()
{
	return JLJ_MITER;
}

jline_style_t Graphics::GetLineStyle()
{
	return JLS_BUTT;
}

int Graphics::GetLineWidth()
{
	return 0;
}

void Graphics::DrawLine(int xp, int yp, int xf, int yf)
{
}

void Graphics::DrawBezierCurve(jpoint_t *p, int npoints, int interpolation)
{
}

void Graphics::FillRectangle(int xp, int yp, int wp, int hp)
{
}

void Graphics::DrawRectangle(int xp, int yp, int wp, int hp)
{
}

void Graphics::FillBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
}

void Graphics::DrawBevelRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
}

void Graphics::FillRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
}

void Graphics::DrawRoundRectangle(int xp, int yp, int wp, int hp, int dx, int dy, jrect_corner_t corners)
{
}

void Graphics::FillCircle(int xcp, int ycp, int rp)
{
}

void Graphics::DrawCircle(int xcp, int ycp, int rp)
{
}

void Graphics::FillEllipse(int xcp, int ycp, int rxp, int ryp)
{
}

void Graphics::DrawEllipse(int xcp, int ycp, int rxp, int ryp)
{
}

void Graphics::FillChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
}

void Graphics::DrawChord(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
}

void Graphics::FillArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
}

void Graphics::DrawArc(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
}

void Graphics::FillPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
	FillArc(xcp, ycp, rxp, ryp, arc0, arc1);
}

void Graphics::DrawPie(int xcp, int ycp, int rxp, int ryp, double arc0, double arc1)
{
}
		
void Graphics::FillTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
}

void Graphics::DrawTriangle(int x1p, int y1p, int x2p, int y2p, int x3p, int y3p)
{
}

void Graphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool close)
{
}

void Graphics::FillPolygon(int xp, int yp, jpoint_t *p, int npoints, bool even_odd)
{
}

void Graphics::SetGradientStop(double stop, const Color &color)
{
	jgradient_t t;

	t.color = color;
	t.stop = stop;

	_gradient_stops.push_back(t);
}

void Graphics::ResetGradientStop()
{
	_gradient_stops.clear();
}

void Graphics::FillRadialGradient(int xcp, int ycp, int wp, int hp, int x0p, int y0p, int r0p)
{
}

void Graphics::FillLinearGradient(int xp, int yp, int wp, int hp, int x1p, int y1p, int x2p, int y2p)
{
}

void Graphics::DrawString(std::string text, int xp, int yp)
{
}

void Graphics::DrawGlyph(int symbol, int xp, int yp)
{
}

bool Graphics::DrawImage(std::string img, int xp, int yp)
{
	int iwidth,
			iheight;

	if (Image::GetImageSize(img, &iwidth, &iheight) != false) {
		int wp = SCREEN_TO_SCALE((iwidth), _screen.width, _scale.width),
				hp = SCREEN_TO_SCALE((iheight), _screen.height, _scale.height);

		return Graphics::DrawImage(img, xp, yp, wp, hp);
	}

	return false;
}

bool Graphics::DrawImage(std::string img, int xp, int yp, int wp, int hp)
{
	return false;
}

bool Graphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	return false;
}

bool Graphics::DrawImage(std::string img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	return false;
}

bool Graphics::DrawImage(Image *img, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp);
}

bool Graphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	if ((void *)img == NULL) {
		return false;
	}

	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool Graphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	return false;
}

bool Graphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	return false;
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

void Graphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
}

uint32_t Graphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	return 0;
}

void Graphics::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp, int scansize)
{
}

void Graphics::SetRGB(uint32_t argb, int xp, int yp) 
{
}

void Graphics::SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp, int scanline) 
{
}

void Graphics::Reset()
{
	_color = Color(0x00, 0x00, 0x00, 0x00);

	_radians = 0.0;

	_translate_image.x = 0;
	_translate_image.y = 0;
}

}
