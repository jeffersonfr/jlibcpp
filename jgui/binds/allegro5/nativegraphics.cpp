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

#include <allegro5/allegro.h>
#include <allegro5/events.h>

#define M_2PI	(2*M_PI)

namespace jgui {

extern ALLEGRO_DISPLAY *_display;

NativeGraphics::NativeGraphics(NativeHandler *handler, void *surface, cairo_t *cairo_context, jpixelformat_t pixelformat, int wp, int hp):
	GenericGraphics(surface, cairo_context, pixelformat, wp, hp)
{
	jcommon::Object::SetClassName("jgui::NativeGraphics");

	_handler = handler;
	_surface = surface;
	_is_first = true;
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
		cairo_surface_t *cairo_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, wp, hp);

		_cairo_context = cairo_create(cairo_surface);
	
		// cairo_surface_destroy(cairo_surface);
	}
	
	_is_first = true;
}

void NativeGraphics::Flip()
{
	_has_bounds = false;

	_handler->RequestFlip();

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true && _is_first == false) {
		try {
			_sem.Wait(1000000);
		} catch (jthread::SemaphoreException) {
		} catch (jthread::SemaphoreTimeoutException) {
		}
	}
}

void NativeGraphics::Flip(int xp, int yp, int wp, int hp)
{
	Flip(); return;

	if (wp <= 0 || hp <= 0) {
		return;
	}

	_region.x = xp;
	_region.y = yp;
	_region.width = wp;
	_region.height = hp;

	_has_bounds = true;

	_handler->RequestFlip();

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true && _is_first == false) {
		try {
			_sem.Wait(1000000);
		} catch (jthread::SemaphoreException) {
		} catch (jthread::SemaphoreTimeoutException) {
		}
	}
}

void NativeGraphics::InternalFlip(void *surface)
{
	ALLEGRO_BITMAP *_surface = (ALLEGRO_BITMAP *)surface;

	/*
	if (_surface == NULL) {
		_has_bounds = false;

		_sem.Notify();

		return;
	}
	*/

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

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		_has_bounds = false;

		_sem.Notify();

		return;
	}

	_has_bounds = false;

	ALLEGRO_LOCKED_REGION *lock = al_lock_bitmap(_surface, ALLEGRO_PIXEL_FORMAT_ARGB_8888, ALLEGRO_LOCK_WRITEONLY);

	int size = dw*dh;
	uint8_t *src = data;
	uint8_t *dst = (uint8_t *)lock->data;

	for (int i=0; i<size; i++) {
		dst[3] = src[3];
		dst[2] = src[2];
		dst[1] = src[1];
		dst[0] = src[0];

		src = src + 4;
		dst = dst + 4;
	}

	al_unlock_bitmap(_surface);
	
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_draw_bitmap(_surface, 0, 0, 0);
	al_flip_display();

	_is_first = false;

	_sem.Notify();
}

void NativeGraphics::ReleaseFlip()
{
	_is_first = true;
}

}
