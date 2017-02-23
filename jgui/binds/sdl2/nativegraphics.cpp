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
#include "nativehandler.h"
#include "nativetypes.h"
#include "jsemaphoreexception.h"
#include "jsemaphoretimeoutexception.h"

#include <SDL2/SDL.h>

#define M_2PI	(2*M_PI)

namespace jgui {

NativeGraphics::NativeGraphics(NativeHandler *handler, void *surface, cairo_t *cairo_context, jpixelformat_t pixelformat, int wp, int hp):
	GenericGraphics(surface, cairo_context, pixelformat, wp, hp)
{
	jcommon::Object::SetClassName("jgui::NativeGraphics");

	_handler = handler;
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
	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_FLIP;
	event.user.data1 = this;

	_has_bounds = false;

	SDL_PushEvent(&event);

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true && _handler->IsVerticalSyncEnabled() == true) {
		try {
			_sem.Wait(2000000);
		} catch (jthread::SemaphoreException) {
		} catch (jthread::SemaphoreTimeoutException) {
		}
	}
}

void NativeGraphics::Flip(int xp, int yp, int wp, int hp)
{
	if (wp <= 0 || hp <= 0) {
		return;
	}

	jregion_t *t = new jregion_t;

	t->x = xp;
	t->y = yp;
	t->width = wp;
	t->height = hp;

	SDL_Event event;

	event.type = USER_NATIVE_EVENT_APPLICATION_FLIP;
	event.user.data1 = this;
	event.user.data2 = t;

	_region.x = xp;
	_region.y = yp;
	_region.width = wp;
	_region.height = hp;

	_has_bounds = true;

	SDL_PushEvent(&event);
	
	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true && _handler->IsVerticalSyncEnabled() == true) {
		try {
			_sem.Wait(2000000);
		} catch (jthread::SemaphoreException) {
		} catch (jthread::SemaphoreTimeoutException) {
		}
	}
}

void NativeGraphics::InternalFlip()
{
	if (_surface == NULL) {
		_has_bounds = false;

		_sem.Notify();

		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);
	
	if (cairo_surface == NULL) {
		_has_bounds = false;

		_sem.Notify();

		return;
	}

	cairo_surface_flush(cairo_surface);

	int dw = cairo_image_surface_get_width(cairo_surface);
	int dh = cairo_image_surface_get_height(cairo_surface);
	// int stride = cairo_image_surface_get_stride(cairo_surface);

	SDL_Renderer *renderer = (SDL_Renderer *)_surface;

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		_has_bounds = false;

		_sem.Notify();

		return;
	}

	SDL_Surface *surface = SDL_CreateRGBSurfaceFrom(data, dw, dh, 32, dw*4, 0, 0, 0, 0);
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

	if (texture == NULL) {
		SDL_FreeSurface(surface);

		_has_bounds = false;

		_sem.Notify();

		return;
	}

	if (_has_bounds == false) {
		SDL_Rect dst;

		dst.x = 0;
		dst.y = 0;
		dst.w = dw;
		dst.h = dh;

		SDL_RenderCopy(renderer, texture, NULL, &dst);
	} else {
		SDL_Rect src, dst;

		src.x = _region.x;
		src.y = _region.y;
		src.w = _region.width;
		src.h = _region.height;

		dst.x = _region.x;
		dst.y = _region.y;
		dst.w = _region.width;
		dst.h = _region.height;

		SDL_RenderCopy(renderer, texture, &src, &dst);
	}

	_has_bounds = false;

	SDL_DestroyTexture(texture);
	SDL_FreeSurface(surface);
	SDL_RenderPresent(renderer);

	if (_handler->IsVisible() == true && _handler->IsVerticalSyncEnabled() == true) {
		_sem.Notify();
	}
}

}
