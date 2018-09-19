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
#include "generic/include/genericprovider_bpg.h"

#include <jio/jfile.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

extern "C" {
#include <libbpg.h>
}

namespace jgui {

cairo_surface_t * create_bpg_surface_from_file(const char *filename)
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

  cairo_surface_t *surface = create_bpg_surface_from_data(buffer, count);

  delete [] buffer;

  return surface;
}

cairo_surface_t * create_bpg_surface_from_data(uint8_t *data, int size) 
{
  BPGDecoderContext 
    *img = bpg_decoder_open();

  if (img == nullptr) {
    return nullptr;
  }

  if (bpg_decoder_decode(img, data, size) < 0) {
    bpg_decoder_close(img);

    return nullptr;
  }

  BPGImageInfo 
    info;
  int 
    sw, 
    sh;

  bpg_decoder_get_info(img, &info);

  sw = info.width;
  sh = info.height;

  cairo_surface_t 
    *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
    bpg_decoder_close(img);

    return nullptr;
	}

	uint8_t 
    *dst = cairo_image_surface_get_data(surface);

	if (dst == nullptr) {
    bpg_decoder_close(img);

		return nullptr;
	}

  bpg_decoder_start(img, BPG_OUTPUT_FORMAT_RGBA32);

  uint8_t 
    *line = dst;

  for (int y=0; y<sh; y++) {
    bpg_decoder_get_line(img, line);

    line = line + 4*sw;
  }

	for (int i=0; i<sw*sh; i++) {
    uint8_t p = dst[2];

		dst[2] = dst[0];
		dst[0] = p;

		dst = dst + 4;
	}

	cairo_surface_mark_dirty(surface);

  bpg_decoder_close(img);

  return surface;
}

}

