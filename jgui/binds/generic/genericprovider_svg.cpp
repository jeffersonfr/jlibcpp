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
#include "generic/include/genericprovider_svg.h"

#include <jio/jfile.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <librsvg-2.0/librsvg/rsvg.h>

namespace jgui {

cairo_surface_t * create_svg_surface_from_file(const char *filename)
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

  cairo_surface_t *surface = create_svg_surface_from_data(buffer, count);

  delete [] buffer;

  return surface;
}

cairo_surface_t * create_svg_surface_from_data(uint8_t *data, int size) 
{
  if (memcmp(data, "<?xml", 5) != 0) {
    return nullptr;
  }

  // rsvg_init();

  RsvgHandle 
    *svg = rsvg_handle_new_from_data(data, size, NULL);
    // *svg = svg_new_from_file (file, &err);
  RsvgDimensionData 
    dimensions;

  rsvg_handle_get_dimensions(svg, &dimensions);

  int
    sw = dimensions.width,
    sh = dimensions.height;
  float
    scale = 72.0f; // pick_best_scape(sw, sh, dw, dh);

  sw = ((float)sw * scale);
  sh = ((float)sh * scale);

  cairo_surface_t 
    *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

  if (surface == nullptr) {
    return nullptr;
  }

  cairo_t *cr = cairo_create(surface);

  cairo_scale(cr, scale, scale);
  rsvg_handle_render_cairo(svg, cr);

  cairo_surface_mark_dirty(surface);
  cairo_destroy(cr);

  return surface;
}

}

