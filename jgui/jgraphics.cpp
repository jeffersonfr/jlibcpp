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

#include <math.h>

namespace jgui {

Graphics::Graphics():
	jcommon::Object()
{
	jcommon::Object::SetClassName("jgui::Graphics");

	_font = NULL;

	_translate.x = 0;
	_translate.y = 0;

	_vertical_sync = false;

	_antialias = JAM_NORMAL;
}

Graphics::~Graphics()
{
}

void * Graphics::GetNativeSurface()
{
	return NULL;
}

void Graphics::SetNativeSurface(void *data, int wp, int hp)
{
}

void Graphics::Dump(std::string dir, std::string pre)
{
}

void Graphics::SetVerticalSyncEnabled(bool b)
{
	_vertical_sync = b;
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

void Graphics::SetDrawingMode(jdrawing_mode_t t)
{
}

jcomposite_flags_t Graphics::GetCompositeFlags()
{
	return JCF_CLEAR;
}

jdrawing_mode_t Graphics::GetDrawingMode()
{
	return JDM_STROKE;
}

void Graphics::Clear()
{
}

void Graphics::Clear(int xp, int yp, int wp, int hp)
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

void Graphics::SetAntialias(jantialias_mode_t mode)
{
	_antialias = mode;
}

jantialias_mode_t Graphics::GetAntialias()
{
	return _antialias;
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

void Graphics::DrawPolygon(int xp, int yp, jpoint_t *p, int npoints, bool closed)
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

jregion_t Graphics::GetStringExtends(std::string text)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

jregion_t Graphics::GetGlyphExtends(int symbol)
{
	jregion_t t;

	t.x = 0;
	t.y = 0;
	t.width = 0;
	t.height = 0;

	return t;
}

void Graphics::DrawString(std::string text, int xp, int yp)
{
}

void Graphics::DrawGlyph(int symbol, int xp, int yp)
{
}

bool Graphics::DrawImage(Image *img, int xp, int yp)
{
	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp);
}

bool Graphics::DrawImage(Image *img, int xp, int yp, int wp, int hp)
{
	return DrawImage(img, 0, 0, img->GetWidth(), img->GetHeight(), xp, yp, wp, hp);
}

bool Graphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp)
{
	if ((void *)img == NULL) {
		return false;
	}

	jgui::Image *aux = img->Crop(sxp, syp, swp, shp);

	if (aux == NULL) {
		return false;
	}

	jgui::Image *buffer = jgui::Image::CreateImage(aux->GetPixelFormat(), aux->GetWidth(), aux->GetHeight());

	if (buffer == NULL) {
		return false;
	}

	uint32_t *rgb = NULL;

	aux->GetRGB(&rgb, 0, 0, swp, shp);

	if (rgb != NULL) {
		buffer->GetGraphics()->SetRGB(rgb, _translate.x+xp, _translate.y+yp, swp, shp);

		delete [] rgb;

		DrawImage(buffer, xp, yp);
	}

	delete aux;

	return true;
}

bool Graphics::DrawImage(Image *img, int sxp, int syp, int swp, int shp, int xp, int yp, int wp, int hp)
{
	sleep(1);
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

	jgui::Image *buffer = jgui::Image::CreateImage(scl->GetPixelFormat(), scl->GetWidth(), scl->GetHeight());

	if (buffer == NULL) {
		delete scl;

		return false;
	}

	uint32_t *rgb = NULL;

	scl->GetRGB(&rgb, 0, 0, wp, hp);

	if (rgb != NULL) {
		buffer->GetGraphics()->SetRGB(rgb, _translate.x+xp, _translate.y+yp, wp, hp);

		delete [] rgb;

		DrawImage(buffer, xp, yp);
	}

	delete scl;

	return true;
}

void Graphics::Translate(int x, int y)
{
	_translate.x += x;
	_translate.y += y;
}

jpoint_t Graphics::Translate()
{
	return _translate;
}

void Graphics::DrawString(std::string text, int xp, int yp, int wp, int hp, jhorizontal_align_t halign, jvertical_align_t valign, bool clipped)
{
}

uint32_t Graphics::GetRGB(int xp, int yp, uint32_t pixel)
{
	return 0;
}

void Graphics::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	(*rgb) = NULL;
}

void Graphics::SetRGB(uint32_t argb, int xp, int yp) 
{
}

void Graphics::SetRGB(uint32_t *rgb, int xp, int yp, int wp, int hp) 
{
}

void Graphics::Reset()
{
	_color = Color(0x00, 0x00, 0x00, 0x00);
}

void Graphics::Lock()
{
	_graphics_mutex.Lock();
}

void Graphics::Unlock()
{
	_graphics_mutex.Unlock();
}

}
