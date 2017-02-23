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
#include "jsemaphoreexception.h"
#include "jsemaphoretimeoutexception.h"

#include <X11/Xutil.h>

#define M_2PI	(2*M_PI)

namespace jgui {

NativeGraphics::NativeGraphics(NativeHandler *handler, void *surface, cairo_t *cairo_context, jpixelformat_t pixelformat, int wp, int hp):
	GenericGraphics(surface, cairo_context, pixelformat, wp, hp)
{
	jcommon::Object::SetClassName("jgui::NativeGraphics");

	_handler = handler;
	_surface = surface;
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
}

void NativeGraphics::Flip()
{
	::Display *display = (::Display *)_handler->GetDisplay();

	XEvent event;

	event.xexpose.type = Expose;
	event.xexpose.serial = 0;
	event.xexpose.send_event = True;
	event.xexpose.display = display;
	event.xexpose.window = *(::Window *)_surface;
	event.xexpose.x = _handler->GetX();
	event.xexpose.y = _handler->GetY();
	event.xexpose.width = _handler->GetWidth();
	event.xexpose.height = _handler->GetHeight();
	event.xexpose.count = 0;

	_has_bounds = false;

	XPutBackEvent(display, &event);

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true) {
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

	::Display *display = (::Display *)_handler->GetDisplay();

	XEvent event;

	event.xexpose.type = Expose;
	event.xexpose.serial = 0;
	event.xexpose.send_event = True;
	event.xexpose.display = display;
	event.xexpose.window = *(::Window *)_surface;
	event.xexpose.x = _handler->GetX();
	event.xexpose.y = _handler->GetY();
	event.xexpose.width = _handler->GetWidth();
	event.xexpose.height = _handler->GetHeight();
	event.xexpose.count = 0;

	_region.x = xp;
	_region.y = yp;
	_region.width = wp;
	_region.height = hp;

	_has_bounds = true;

	XPutBackEvent(display, &event);

	// CHANGE:: if continues to block exit, change to timed semaphore
	if (_handler->IsVisible() == true) {
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

		return;
	}

	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);
	
	if (cairo_surface == NULL) {
		_has_bounds = false;

		return;
	}

	cairo_surface_flush(cairo_surface);

	int dw = cairo_image_surface_get_width(cairo_surface);
	int dh = cairo_image_surface_get_height(cairo_surface);
	// int stride = cairo_image_surface_get_stride(cairo_surface);

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		_has_bounds = false;

		return;
	}

	// Create the icon pixmap
	::Display *display = (::Display *)_handler->GetDisplay();
	int screen = DefaultScreen(display);
	Visual *visual = DefaultVisual(display, screen);
	unsigned int depth = DefaultDepth(display, screen);

	XImage *image = XCreateImage(display, visual, depth, ZPixmap, 0, (char *)data, dw, dh, 32, 0);

	if (image == NULL) {
		_has_bounds = false;

		return;
	}

	Pixmap pixmap;

	if (_has_bounds == false) {
		pixmap = XCreatePixmap(display, XRootWindow(display, screen), dw, dh, depth);
	} else {
		pixmap = XCreatePixmap(display, XRootWindow(display, screen), _region.width, _region.height, depth);
	}

	GC gc = XCreateGC(display, pixmap, 0, NULL);
	
	// XClearWindow(*(::Window *)_surface);
	
	// draw image to pixmap
	if (_has_bounds == false) {
		XPutImage(display, pixmap, gc, image, 0, 0, 0, 0, dw, dh);
		XCopyArea(display, pixmap, *(::Window *)_surface, gc, 0, 0, dw, dh, 0, 0);
	} else {
		XPutImage(display, pixmap, gc, image, _region.x, _region.y, 0, 0, dw, dh);
		XCopyArea(display, pixmap, *(::Window *)_surface, gc, 0, 0, dw, dh, _region.x, _region.y);
	}

	// XDestroyImage(image);
	XFreePixmap(display, pixmap);

	// XFlush(display);

	// INFO:: wait x11 process all events
	// True:: discards all events remaing
	// False:: not discards events remaing
	XSync(display, True);
	
	_sem.Notify();
}

}

