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

#include <directfb.h>

#define M_2PI	(2*M_PI)

namespace jgui {

NativeGraphics::NativeGraphics(void *surface, cairo_t *cairo_context, jpixelformat_t pixelformat, int wp, int hp):
	GenericGraphics(surface, cairo_context, pixelformat, wp, hp)
{
	jcommon::Object::SetClassName("jgui::NativeGraphics");
}

NativeGraphics::~NativeGraphics()
{
}

void NativeGraphics::SetNativeSurface(void *surface, int wp, int hp)
{
	_surface = surface;

	cairo_destroy(_cairo_context);

	_cairo_context = NULL;

	if (_surface != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)_surface;

		surface->GetSize(surface, &wp, &hp);

		cairo_surface_t *cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wp, hp);

		_cairo_context = cairo_create(cairo_surface);
		
		// cairo_surface_destroy(cairo_surface);
	}
}

void NativeGraphics::Flip()
{
	if (_surface == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);
	
	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	// int dw = cairo_image_surface_get_width(cairo_surface);
	// int dh = cairo_image_surface_get_height(cairo_surface);
	int stride = cairo_image_surface_get_stride(cairo_surface);

	IDirectFBSurface *surface = (IDirectFBSurface *)_surface;
	DFBRectangle rect;
	// void *ptr;
	int sw;
	int sh;
	// int pitch;

	surface->GetSize(surface, &sw, &sh);

	rect.x = 0;
	rect.y = 0;
	rect.w = sw;
	rect.h = sh;

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	surface->Write(surface, &rect, data, stride);
		
	if (_vertical_sync == false) {
		surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	} else {
		surface->Flip(surface, NULL, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
	}
}

void NativeGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

	if (_surface == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	// int dw = cairo_image_surface_get_width(cairo_surface);
	// int dh = cairo_image_surface_get_height(cairo_surface);
	int stride = cairo_image_surface_get_stride(cairo_surface);

	IDirectFBSurface *surface = (IDirectFBSurface *)_surface;
	int x = xp;
	int y = yp;
	int w = wp;
	int h = hp;

	DFBRegion rgn;

	rgn.x1 = x;
	rgn.y1 = y;
	rgn.x2 = x+w;
	rgn.y2 = y+h;

	DFBRectangle rect;
	// void *ptr;
	int sw;
	int sh;
	// int pitch;

	surface->GetSize(surface, &sw, &sh);

	rect.x = 0;
	rect.y = 0;
	rect.w = sw;
	rect.h = sh;

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	surface->Write(surface, &rect, data, stride);

	if (_vertical_sync == false) {
		surface->Flip(surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_NONE));
	} else {
		surface->Flip(surface, &rgn, (DFBSurfaceFlipFlags)(DSFLIP_BLIT | DSFLIP_WAITFORSYNC));
	}
}

void NativeGraphics::Lock()
{
	_mutex.Lock();
}

void NativeGraphics::Unlock()
{
	_mutex.Unlock();
}

}
