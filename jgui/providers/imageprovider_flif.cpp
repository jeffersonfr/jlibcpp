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
#include "providers/include/imageprovider_flif.h"

// #include "config.h"
#include "flif-enc.hpp"
#include "flif-dec.hpp"

namespace jgui {

cairo_surface_t * create_flif_surface_from_data(uint8_t *data, int size)
{
  if (memcmp(data, "FLIF", 4) != 0) {
    return nullptr;
  }

  BlobReader 
    fio(data, size);
  FLIF_INFO 
    info;
  Images 
    images;
  metadata_options 
    md;
  flif_options 
    options = FLIF_DEFAULT_OPTIONS;
  
  md.icc = options.color_profile;
  md.xmp = options.metadata;
  md.exif = options.metadata;
  
  if (flif_decode(fio, images, nullptr, nullptr, 0, images, options, md, &info) == false) {
    return nullptr;
  }

  int sw = info.width;
  int sh = info.height;
  int ch = info.channels;
  int depth = info.bit_depth;

  if ((ch != 3 and ch != 4) or depth != 8) {
    return nullptr;
  }

  const Image &image = images[0];
  const GeneralPlane &plane = image.getPlane(0);

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

  if (ch == 3) {
    for (int j=0; j<sh; j++) {
      for (int i=0; i<sw; i++) {
        ColorVal c = plane.get(j, i);

        dst[0] = (c >> 0x10) & 0xff;
        dst[1] = (c >> 0x08) & 0xff;
        dst[2] = (c >> 0x00) & 0xff;
        dst[3] = 0xff;

        dst = dst + 4;
      }
    }
  } else if (ch == 4) {
    for (int j=0; j<sh; j++) {
      for (int i=0; i<sw; i++) {
        ColorVal c = plane.get(j, i);

        dst[0] = (c >> 0x10) & 0xff;
        dst[1] = (c >> 0x08) & 0xff;
        dst[2] = (c >> 0x00) & 0xff;
        dst[3] = (c >> 0x18) & 0xff;

        dst = dst + 4;
      }
    }
  }

	cairo_surface_mark_dirty(surface);

  return nullptr;
}

}
