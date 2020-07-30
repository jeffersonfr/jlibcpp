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
#include "providers/include/imageprovider_avif.h"

#include <fstream>

#include <avif/avif.h>

#include <inttypes.h>
#include <memory.h>
#include <stdio.h>
#include <strings.h>

namespace jgui {

cairo_surface_t * create_jpg_surface_from_data(uint8_t *data, int size)
{
  avifROData raw = AVIF_DATA_EMPTY;
  avifImage *avif = NULL;
  avifRGBImage rgb = {
      .format = AVIF_RGB_FORMAT_RGB,
  };
  avifDecoder *decoder = NULL;
  avifResult result;

  raw->data = data;
  raw->size = size;

  avifBool ok = avifPeekCompatibleFileType(&raw);

  if (!ok) {
    printf(filename);
    goto out;
  }

  decoder = avifDecoderCreate();

  if (decoder == NULL) {
    printf(filename);
    goto out;
  }

  result = avifDecoderParse(decoder, &raw);

  if (result != AVIF_RESULT_OK) {
    printf(avifResultToString(result));
    goto out;
  }

  if (decoder->imageCount > 1) {
    dt_control_log(_("image '%s' has more than one frame!"), filename);
  }

  result = avifDecoderNthImage(decoder, 0);
  
  if (result != AVIF_RESULT_OK) {
    printf(avifResultToString(result));
    goto out;
  }

  avif = decoder->image;

  // This will set the depth from the avif
  avifRGBImageSetDefaults(&rgb, avif);

  rgb.format = AVIF_RGB_FORMAT_RGB;

  avifRGBImageAllocatePixels(&rgb);

  result = avifImageYUVToRGB(avif, &rgb);
  if (result != AVIF_RESULT_OK) {
    printf(avifResultToString(result));
    goto out;
  }

  const size_t width = rgb.width;
  const size_t height = rgb.height;
  // If `> 8', all plane ptrs are 'uint16_t *'
  const size_t bit_depth = rgb.depth;

  // Initialize cached image buffer
  img->width = width;
  img->height = height;

  img->buf_dsc.channels = 4;
  img->buf_dsc.datatype = TYPE_FLOAT;
  img->buf_dsc.cst = iop_cs_rgb;

  float *mipbuf = (float *)dt_mipmap_cache_alloc(mbuf, img);

  if (mipbuf == NULL) {
    printf(filename);
    goto out;
  }

  const float max_channel_f = (float)((1 << bit_depth) - 1);
  const size_t rowbytes = rgb.rowBytes;
  const uint8_t *const restrict in = (const uint8_t *)rgb.pixels;

  switch (bit_depth) {
  case 12:
  case 10: {
#ifdef _OPENMP
#pragma omp parallel for simd default(none) \
  dt_omp_firstprivate(mipbuf, width, height, in, rowbytes, max_channel_f) \
  schedule(simd:static) \
  collapse(2)
#endif
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
          uint16_t *in_pixel = (uint16_t *)&in[(y * rowbytes) + (3 * sizeof(uint16_t) * x)];
          float *out_pixel = &mipbuf[(size_t)4 * ((y * width) + x)];

          // max_channel_f is 255.0f for 8bit
          out_pixel[0] = ((float)in_pixel[0]) * (1.0f / max_channel_f);
          out_pixel[1] = ((float)in_pixel[1]) * (1.0f / max_channel_f);
          out_pixel[2] = ((float)in_pixel[2]) * (1.0f / max_channel_f);
          out_pixel[3] = 0.0f;
      }
    }
    break;
  }
  case 8: {
#ifdef _OPENMP
#pragma omp parallel for simd default(none) \
  dt_omp_firstprivate(mipbuf, width, height, in, rowbytes, max_channel_f) \
  schedule(simd:static) \
  collapse(2)
#endif
    for (size_t y = 0; y < height; y++) {
      for (size_t x = 0; x < width; x++) {
          uint8_t *in_pixel = (uint8_t *)&in[(y * rowbytes) + (3 * sizeof(uint8_t) * x)];
          float *out_pixel = &mipbuf[(size_t)4 * ((y * width) + x)];

          // max_channel_f is 255.0f for 8bit
          out_pixel[0] = (float)(in_pixel[0]) * (1.0f / max_channel_f);
          out_pixel[1] = (float)(in_pixel[1]) * (1.0f / max_channel_f);
          out_pixel[2] = (float)(in_pixel[2]) * (1.0f / max_channel_f);
          out_pixel[3] = 0.0f;
      }
    }
    break;
  }
  default:
    printf(filename);
    goto out;
  }

	cairo_surface_t *surface = nullptr;

  // TODO::

out:
  avifRGBImageFreePixels(&rgb);
  avifDecoderDestroy(decoder);
  avifFree((void *)raw.data);

  return nullptr;
}

}

