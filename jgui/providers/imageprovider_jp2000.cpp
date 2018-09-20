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
#include "providers/include/imageprovider_jp2000.h"

#include <jio/jfile.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <jasper/jasper.h>

#define JP2_SIGNATURE "\x00\x00\x00\x0C\x6A\x50\x20\x20\x0D\x0A\x87\x0A"
#define JPC_SIGNATURE "\xFF\x4F"

namespace jgui {

static int jasper_refs = 0;

static void init_jasper()
{
  if (++jasper_refs == 1) {
    jas_image_fmtops_t fmtops;
    int                fmtid = 0;

    fmtops.decode = jp2_decode;
    fmtops.encode = jp2_encode;
    fmtops.validate = jp2_validate;
    jas_image_addfmt(fmtid, (char *)"jp2", (char *)"jp2", (char *)"JPEG-2000 JP2 File Format Syntax (ISO/IEC 15444-1)", &fmtops);

    ++fmtid;

    fmtops.decode = jpc_decode;
    fmtops.encode = jpc_encode;
    fmtops.validate = jpc_validate;
    jas_image_addfmt(fmtid, (char *)"jpc", (char *)"jpc", (char *)"JPEG-2000 Code Stream Syntax (ISO/IEC 15444-1)", &fmtops);

    ++fmtid;
  }
}

static void release_jasper()
{
  if (--jasper_refs == 0)
    jas_cleanup();
}     

cairo_surface_t * create_jp2000_surface_from_file(const char *filename)
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

  cairo_surface_t *surface = create_jp2000_surface_from_data(buffer, count);

  delete [] buffer;

  return surface;
}

cairo_surface_t * create_jp2000_surface_from_data(uint8_t *data, int size) 
{
  jas_stream_t
    *stream = nullptr;
  jas_image_t
    *image = nullptr;

  if (!memcmp(data, JP2_SIGNATURE, sizeof(JP2_SIGNATURE) - 1) || !memcmp(data, JPC_SIGNATURE, sizeof(JPC_SIGNATURE) - 1)) {
    // return nullptr;
  }

  init_jasper();

  stream = jas_stream_memopen((char *)data, size);
  // stream = jas_stream_fopen( buffer_data->filename, "rb");

  if (!stream) {
    release_jasper();

    return nullptr;
  }

  image = jas_image_decode(stream, -1, 0);

  jas_stream_close(stream);

  if (!image) {
    release_jasper();

    return nullptr;
  }

  switch (jas_image_numcmpts(image)) {
    case 1:
    case 3:
      break;
    default:
      if (image) {
        jas_image_destroy(image);
      }

      release_jasper();

      return nullptr;
  }

  jas_clrspc_t cm = jas_image_clrspc(image);
  int cmptlut[3];
  int sw, sh;
  int tlx, tly;
  int hs, vs;
  bool mono = false;

  if (jas_image_numcmpts(image) > 1) {
    cmptlut[0] = jas_image_getcmptbytype(image, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_R));
    cmptlut[1] = jas_image_getcmptbytype(image, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_G));
    cmptlut[2] = jas_image_getcmptbytype(image, JAS_IMAGE_CT_COLOR(JAS_CLRSPC_CHANIND_RGB_B));


    if (cm == JAS_CLRSPC_CIELAB) {
      mono = true;
    }
  } else {
    cmptlut[0] = 0;
    cmptlut[1] = 0;
    cmptlut[2] = 0;
    mono = true;
  }

  sw = jas_image_width(image);
  sh = jas_image_height(image);
  tlx = jas_image_cmpttlx(image, 0);
  tly = jas_image_cmpttly(image, 0);
  hs = jas_image_cmpthstep(image, 0);
  vs = jas_image_cmptvstep(image, 0);

#define GET_SAMPLE( n, x, y ) ({ \
    int _s; \
    _s = jas_image_readcmptsample(image, cmptlut[n], x, y); \
    _s >>= jas_image_cmptprec(image, cmptlut[n]) - 8; \
    if (_s > 255) \
    _s = 255; \
    else if (_s < 0) \
    _s = 0; \
    _s; \
    })

  cairo_surface_t 
    *surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sw, sh);

	if (surface == nullptr) {
    if (image) {
      jas_image_destroy(image);
    }

    release_jasper();

    return nullptr;
	}

	uint8_t 
    *dst = cairo_image_surface_get_data(surface);

	if (dst == nullptr) {
    if (image) {
      jas_image_destroy(image);
    }

    release_jasper();

		return nullptr;
	}

  for (int i = 0; i < sh; i++) {
    uint8_t 
      *out = dst + i * sw * 4;
    int  
      x = 0, 
      y = (i - tly) / vs;

    if (y >= 0 && y < sh) {     
      for (int j = 0; j < sw; j++) {
        x = (j - tlx) / hs;

        if (x >= 0 && x < sw) {
          uint32_t r, g, b;

          if (mono) {
            r = g = b = GET_SAMPLE(0, x, y);
          } else {
            if (cm == JAS_CLRSPC_SYCBCR) {
              r = GET_SAMPLE(0, x, y);
              g = GET_SAMPLE(1, x/2, y/2);
              b = GET_SAMPLE(2, x/2, y/2);
            } else {
              r = GET_SAMPLE(0, x, y);
              g = GET_SAMPLE(1, x, y);
              b = GET_SAMPLE(2, x, y);
            }
          }

          if (cm == JAS_CLRSPC_SYCBCR) {
            int Y = r;
            int Cr = g - 128;
            int Cb = b - 128;

            b = Y + 45 * Cr / 32;
            g = Y - (11 * Cb + 23 * Cr) / 32;
            r = Y + 113 * Cb / 64;

            r = (r < 0x00)?0x00:(r > 0xff)?0xff:r;
            g = (g < 0x00)?0x00:(g > 0xff)?0xff:g;
            b = (b < 0x00)?0x00:(b > 0xff)?0xff:b;
          }

          out[0] = b;
          out[1] = g;
          out[2] = r;
          out[3] = 0xff;
        } else {
          out[0] = 0x00;
          out[1] = 0x00;
          out[2] = 0x00;
          out[3] = 0xff;
        }

        out = out + 4;
      }
    } else {
      memset(out, 0, sw*4);
    }
  }

	cairo_surface_mark_dirty(surface);

  return surface;
}

}

