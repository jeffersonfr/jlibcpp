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
#include "generic/include/genericprovider_flif.h"

#include "jio/jfile.h"

#include <fstream>

// #include <flif.h>

namespace jgui {

cairo_surface_t * create_flif_surface_from_file(const char *filename) 
{
  jio::File *file = jio::File::OpenFile(filename, (jio::jfile_flags_t)(jio::JFF_READ_ONLY | jio::JFF_LARGEFILE));

  if (file == nullptr) {
    return nullptr;
  }

  uint8_t 
    *buffer = new uint8_t[file->GetSize()];
  int 
    length,
    count = 0;

  while ((length = file->Read((char *)buffer + count, 4098)) > 0) {
    count = count + length;
  }

  cairo_surface_t *surface = create_flif_surface_from_data(buffer, count);

  delete [] buffer;

  return surface;
}

cairo_surface_t * create_flif_surface_from_data(uint8_t *data, int size)
{
  /*
  FLIF_DECODER *d = flif_create_decoder();

  if(d == nullptr) {
    return nullptr;
  }

  flif_decoder_set_quality(d, 100);
  flif_decoder_set_scale(d, 1);

  if(flif_decoder_decode_memory(d, data, size) == 0)
    flif_destroy_decoder(d);

    return nullptr;
  }

  FLIF_IMAGE *decoded = flif_decoder_get_image(d, 0);

  if(decoded == 0) {
    flif_destroy_decoder(d);

    return nullptr;
  }

  FLIF_INFO *info = flif_read_info_from_memory(data, size);

  if (info == 0) {
    flif_destroy_decoder(d);

    return nullptr;
  }

  int sw = flif_info_get_width(info);
  int sh = flif_info_get_height(info);
  int ch = flif_info_get_nb_channels(info);
  int depth = flif_info_get_depth(info);

  flif_destroy_info(info);

  if (ch != 3 && ch != 4 && depth != 8) {
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

  if (ch == 3) {
    for (int i=0; i<length; i++) {
      dst[0] = src[2];
      dst[1] = src[1];
      dst[2] = src[0];
      dst[3] = src[3];

      dst = dst + 4;
      src = src + 4;
    }
  } else if (ch == 4) {
    for (int i=0; i<length; i++) {
      dst[0] = src[2];
      dst[1] = src[1];
      dst[2] = src[0];
      dst[3] = src[3];

      dst = dst + 4;
      src = src + 4;
    }
  }

	cairo_surface_mark_dirty(surface);
  flif_destroy_decoder(d);
  */

  return nullptr;
}

}
