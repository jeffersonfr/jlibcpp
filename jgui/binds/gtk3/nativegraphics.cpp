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

#define M_2PI	(2*M_PI)

namespace jgui {

NativeGraphics::NativeGraphics(NativeHandler *handler, void *surface, cairo_t *cairo_context, jpixelformat_t pixelformat, int wp, int hp):
	GenericGraphics(surface, cairo_context, pixelformat, wp, hp)
{
	jcommon::Object::SetClassName("jgui::NativeGraphics");

	_handler = handler;
	_surface = NULL;
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
	}
	
	_is_first = true;
}

void NativeGraphics::Flip()
{
	_handler->RequestDrawing();

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true && _is_first != true) {
		try {
			_sem.Wait(1000000);
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

	_has_bounds = true;

	_handler->RequestDrawing(xp, yp, wp, hp);

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true && _is_first != true) {
		try {
			_sem.Wait(1000000);
		} catch (jthread::SemaphoreException) {
		} catch (jthread::SemaphoreTimeoutException) {
		}
	}
}

void NativeGraphics::InternalFlip(cairo_t *cr)
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

	if (_has_bounds == false) {
		// TODO:: process the both flips
	} else {
	}

	cairo_set_source_surface(cr, cairo_surface, 0, 0);
	cairo_paint(cr);
	
	_has_bounds = false;
	_is_first = false;

	_sem.Notify();
}

void NativeGraphics::ReleaseFlip()
{
	_is_first = true;
}

}
