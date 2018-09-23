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
#include "providers/include/imageprovider_webp.h"

#include "jio/jfile.h"

#include <fstream>

#include <webp/decode.h>

namespace jgui {

cairo_surface_t * create_webp_surface_from_data(uint8_t *data, int size)
{
  int
    sw,
    sh;

  if (WebPGetInfo(data, size, &sw, &sh) == false) {
    return nullptr;
  }

  if (sw <= 0 || sh <= 0) {
    return nullptr;
  }

  uint8_t 
    *src = WebPDecodeRGBA(data, size, &sw, &sh);

  if (src == nullptr) {
    return nullptr;
  }

  cairo_surface_t 
    *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
		return nullptr;
	}

	uint8_t 
    *dst = cairo_image_surface_get_data(surface);

	if (dst == nullptr) {
		return nullptr;
	}

  int 
    length = sw*sh;

  for (int i=0; i<length; i++) {
    dst[0] = src[2];
    dst[1] = src[1];
    dst[2] = src[0];
    dst[3] = src[3];

    dst = dst + 4;
    src = src + 4;
  }

	cairo_surface_mark_dirty(surface);

  // WebPFree(src);

  return surface;
}

}
