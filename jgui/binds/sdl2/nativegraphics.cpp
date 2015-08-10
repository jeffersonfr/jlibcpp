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

#include <SDL2/SDL.h>

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

void NativeGraphics::SetNativeSurface(void *data, int wp, int hp)
{
	_surface = data;

	cairo_destroy(_cairo_context);

	_cairo_context = NULL;

	if (_surface != NULL) {
		// SDL_Renderer *surface = (SDL_Renderer *)_surface;

		// GetRenderGetLogicalSize(surface, &wp, &hp);
		// GetRendererOutputSize(surface, &wp, &hp);

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

	int dw = cairo_image_surface_get_width(cairo_surface);
	int dh = cairo_image_surface_get_height(cairo_surface);
	// int stride = cairo_image_surface_get_stride(cairo_surface);

	SDL_Renderer *renderer = (SDL_Renderer *)_surface;

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, dw, dh, 32, dw*4, 0, 0, 0, 0);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (texture == NULL) {
		SDL_FreeSurface(surface);

		return;
	}

	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	SDL_RenderPresent(renderer);
}

void NativeGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (_surface == NULL) {
		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);
	
	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	int dw = cairo_image_surface_get_width(cairo_surface);
	int dh = cairo_image_surface_get_height(cairo_surface);
	// int stride = cairo_image_surface_get_stride(cairo_surface);

	SDL_Renderer *renderer = (SDL_Renderer *)_surface;

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, dw, dh, 32, dw*4, 0, 0, 0, 0);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (texture == NULL) {
		SDL_FreeSurface(surface);

		return;
	}

	SDL_Rect src, dst;

	src.x = xp;
	src.y = yp;
	src.w = wp;
	src.h = hp;

	dst.x = xp;
	dst.y = yp;
	dst.w = wp;
	dst.h = hp;

	SDL_RenderCopy(renderer, texture, &src, &dst);
	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	SDL_RenderPresent(renderer);
}

}
