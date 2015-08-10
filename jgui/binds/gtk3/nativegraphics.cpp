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

#include <gtk/gtk.h>

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
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	if (_surface != NULL) {
		gtk_widget_queue_draw((GtkWidget *)_surface);
	}
}

void NativeGraphics::Flip(int xp, int yp, int wp, int hp)
{
	cairo_surface_t *cairo_surface = cairo_get_target(_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	cairo_surface_flush(cairo_surface);

	if (_surface != NULL) {
		gtk_widget_queue_draw((GtkWidget *)_surface);
	}
}

}
