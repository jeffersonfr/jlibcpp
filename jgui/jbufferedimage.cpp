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
#include "jgui/jbufferedimage.h"
#include "jgui/jhslcolorspace.h"
#include "jio/jmemoryinputstream.h"
#include "jio/jfileinputstream.h"
#include "jexception/jnullpointerexception.h"
#include "jexception/jnullpointerexception.h"

#include <memory>

#ifdef BMP_IMAGE
#include "providers/include/imageprovider_bmp.h"
#endif

#ifdef GIF_IMAGE
#include "providers/include/imageprovider_gif.h"
#endif

#ifdef ICO_IMAGE
#include "providers/include/imageprovider_ico.h"
#endif

#ifdef JPG_IMAGE
#include "providers/include/imageprovider_jpg.h"
#endif

#ifdef PCX_IMAGE
#include "providers/include/imageprovider_pcx.h"
#endif

#ifdef PNG_IMAGE
#include "providers/include/imageprovider_png.h"
#endif

#ifdef PPM_IMAGE
#include "providers/include/imageprovider_ppm.h"
#endif

#ifdef TGA_IMAGE
#include "providers/include/imageprovider_tga.h"
#endif

#ifdef XBM_IMAGE
#include "providers/include/imageprovider_xbm.h"
#endif

#ifdef XPM_IMAGE
#include "providers/include/imageprovider_xpm.h"
#endif

#ifdef HEIF_IMAGE
#include "providers/include/imageprovider_heif.h"
#endif

#ifdef WEBP_IMAGE
#include "providers/include/imageprovider_webp.h"
#endif

#ifdef JP2000_IMAGE
#include "providers/include/imageprovider_jp2000.h"
#endif

#ifdef SVG_IMAGE
#include "providers/include/imageprovider_svg.h"
#endif

#ifdef TIF_IMAGE
#include "providers/include/imageprovider_tif.h"
#endif

#ifdef BPG_IMAGE
#include "providers/include/imageprovider_bpg.h"
#endif

#ifdef FLIF_IMAGE
#include "providers/include/imageprovider_flif.h"
#endif

#ifdef MJPEG_IMAGE
#include "providers/include/imageprovider_mjpeg.h"
#endif

#include <string.h>

namespace jgui {

static void NearestNeighborScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
  int x_ratio = (int)((w << 16)/sw) + 1;
  int y_ratio = (int)((h << 16)/sh) + 1;
  int x2, y2;

  for (int i=0; i<sh; i++) {
    y2 = ((i*y_ratio) >> 16);

    uint32_t *t = dst + i*sw;
    uint32_t *p = src + y2*w;
    int rat = 0;

    for (int j=0; j<sw; j++) {
      x2 = (rat >> 16);
      *t++ = p[x2];
      rat += x_ratio;
    }
  }

  /*
  for (int i=0; i<sh; i++) {
    for (int j=0; j<sw; j++) {
      x2 = ((j*x_ratio) >> 16) ;
      y2 = ((i*y_ratio) >> 16) ;
      dst[(i*sw)+j] = src[(y2*w)+x2] ;
    }                
  }
  */
}
    
static void BilinearScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
  int a, b, c, d, x, y, index;
  float x_ratio = ((float)(w-1))/sw;
  float y_ratio = ((float)(h-1))/sh;
  float x_diff, y_diff;
  int blue, red, green, alpha;
  int offset = 0;

  for (int i=0; i<sh; i++) {
    for (int j=0; j<sw; j++) {
      x = (int)(x_ratio * j);
      y = (int)(y_ratio * i);
      x_diff = (x_ratio * j) - x;
      y_diff = (y_ratio * i) - y;
      index = (y*w + x);

      a = src[index + 0*w + 0];
      b = src[index + 0*w + 1];
      c = src[index + 1*w + 0];
      d = src[index + 1*w + 1];

      float m1 = (1-x_diff)*(1-y_diff);
      float m2 = (x_diff)*(1-y_diff);
      float m3 = (y_diff)*(1-x_diff);
      float m4 = (x_diff*y_diff);

      blue = (int)(((a>>0x00) & 0xff)*m1 + ((b>>0x00) & 0xff)*m2 + ((c>>0x00) & 0xff)*m3 + ((d>>0x00) & 0xff)*m4);
      green = (int)(((a>>0x08) & 0xff)*m1 + ((b>>0x08) & 0xff)*m2 + ((c>>0x08) & 0xff)*m3 + ((d>>0x08) & 0xff)*m4);
      red = (int)(((a>>0x10) & 0xff)*m1 + ((b>>0x10) & 0xff)*m2 + ((c>>0x10) & 0xff)*m3 + ((d>>0x10) & 0xff)*m4);
      alpha = (int)(((a>>0x18) & 0xff)*m1 + ((b>>0x18) & 0xff)*m2 + ((c>>0x18) & 0xff)*m3 + ((d>>0x18) & 0xff)*m4);

      dst[offset++] = 
        ((alpha << 0x18) & 0xff000000) | ((red << 0x10) & 0x00ff0000) |
        ((green << 0x08) & 0x0000ff00) | ((blue << 0x00) & 0x000000ff);
    }
  }
}

static float CubicHermite(float A, float B, float C, float D, float t) 
{
  float a = -A/2.0f + (3.0f*B)/2.0f - (3.0f*C)/2.0f + D/2.0f;
  float b = A - (5.0f*B)/2.0f + 2.0f*C - D/2.0f;
  float c = -A/2.0f + C/2.0f;
  float d = B;

  return a*t*t*t + b*t*t + c*t + d;
}

void GetPixelClamped(uint32_t *src, int x, int y, int w, int h, uint8_t temp[4])
{
  if (x < 0) {
    x = 0;
  }

  if (x > (w - 1)) {
    x = w - 1;
  }
  
  if (y < 0) {
    y = 0;
  }

  if (y > (h - 1)) {
    y = h - 1;
  }

  uint32_t p = src[y*w + x];

  temp[0] = (p >> 0x00) & 0xff;
  temp[1] = (p >> 0x08) & 0xff;
  temp[2] = (p >> 0x10) & 0xff;
  temp[3] = (p >> 0x18) & 0xff;
}

static void SampleBicubic(uint32_t *src, int w, int h, float u, float v, uint8_t sample[4]) 
{
  float x = (u*w) - 0.5;
  int xint = (int)x;
  float xfract = x - floor(x);

  float y = (v*h) - 0.5;
  int yint = (int)y;
  float yfract = y - floor(y);

  uint8_t p00[4];
  uint8_t p10[4];
  uint8_t p20[4];
  uint8_t p30[4];

  uint8_t p01[4];
  uint8_t p11[4];
  uint8_t p21[4];
  uint8_t p31[4];

  uint8_t p02[4];
  uint8_t p12[4];
  uint8_t p22[4];
  uint8_t p32[4];

  uint8_t p03[4];
  uint8_t p13[4];
  uint8_t p23[4];
  uint8_t p33[4];

  // 1st row
  GetPixelClamped(src, xint - 1, yint - 1, w, h, p00);
  GetPixelClamped(src, xint + 0, yint - 1, w, h, p10);
  GetPixelClamped(src, xint + 1, yint - 1, w, h, p20);
  GetPixelClamped(src, xint + 2, yint - 1, w, h, p30);

  // 2nd row
  GetPixelClamped(src, xint - 1, yint + 0, w, h, p01);
  GetPixelClamped(src, xint + 0, yint + 0, w, h, p11);
  GetPixelClamped(src, xint + 1, yint + 0, w, h, p21);
  GetPixelClamped(src, xint + 2, yint + 0, w, h, p31);

  // 3rd row
  GetPixelClamped(src, xint - 1, yint + 1, w, h, p02);
  GetPixelClamped(src, xint + 0, yint + 1, w, h, p12);
  GetPixelClamped(src, xint + 1, yint + 1, w, h, p22);
  GetPixelClamped(src, xint + 2, yint + 1, w, h, p32);

  // 4th row
  GetPixelClamped(src, xint - 1, yint + 2, w, h, p03);
  GetPixelClamped(src, xint + 0, yint + 2, w, h, p13);
  GetPixelClamped(src, xint + 1, yint + 2, w, h, p23);
  GetPixelClamped(src, xint + 2, yint + 2, w, h, p33);

  // interpolate bi-cubically!
  for (int i = 0; i < 4; i++) {
    float col0 = CubicHermite(p00[i], p10[i], p20[i], p30[i], xfract);
    float col1 = CubicHermite(p01[i], p11[i], p21[i], p31[i], xfract);
    float col2 = CubicHermite(p02[i], p12[i], p22[i], p32[i], xfract);
    float col3 = CubicHermite(p03[i], p13[i], p23[i], p33[i], xfract);

    float value = CubicHermite(col0, col1, col2, col3, yfract);

    if (value < 0.0f) {
      value = 0.0f;
    }

    if (value > 255.0f) {
      value = 255.0f;
    }

    sample[i] = (uint8_t)value;
  }
}

static void BicubicScale(uint32_t *src, uint32_t *dst, int w, int h, int sw, int sh) 
{
  uint8_t sample[4];
  int y, x;

  for (y=0; y<sh; y++) {
    float v = (float)y/(float)(sh - 1);

    for (x=0; x<sw; x++) {
      float u = (float)x/(float)(sw - 1);

      SampleBicubic(src, w, h, u, v, sample);

      dst[y*sw + x] = sample[3] << 0x18 | sample[2] << 0x10 | sample[1] << 0x08 | sample[0] << 0x00;
    }
  }
}

static void NearesNeighborRotate(uint32_t *src, int w, int h, uint32_t *dst, int dw, int dh, double radians, bool resize)
{
  double angle = fmod(radians, 2*M_PI);
  int precision = 1024;
  int sinTheta = precision*sin(angle);
  int cosTheta = precision*cos(angle);

  int iw = dw;
  int ih = dh;

  memset(dst, 0, iw*ih*sizeof(uint32_t));

  int sxc = w/2;
  int syc = h/2;
  int dxc = iw/2;
  int dyc = ih/2;
  int xo;
  int yo;
  int t1;
  int t2;

  for (int j=0; j<ih; j++) {
    uint32_t *ptr = dst + j*iw;

    t1 = (j-dyc)*sinTheta;
    t2 = (j-dyc)*cosTheta;

    for (int i=0; i<iw; i++) {
      xo = ((i-dxc)*cosTheta - t1)/precision;
      yo = ((i-dxc)*sinTheta + t2)/precision;

      if (xo >= -sxc && xo < sxc && yo >= -syc && yo < syc) {
        *(ptr+i) = *(src + (yo+syc)*w + (xo+sxc));
      }
    }
  }
}

BufferedImage::BufferedImage(jpixelformat_t pixelformat, int width, int height):
  jgui::Image(pixelformat, width, height)
{
  jcommon::Object::SetClassName("jgui::BufferedImage");

  if (width <= 0 || height <= 0) {
    throw jexception::RuntimeException("Image must have width and height greater than 0");
  }

  _pixelformat = pixelformat;
  _size.width = width;
  _size.height = height;

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, _size.width, _size.height);

  _graphics = new Graphics(cairo_create(surface));
}

BufferedImage::BufferedImage(cairo_t *cairo_context):
  jgui::Image(jgui::JPF_UNKNOWN, -1, -1)
{
  jcommon::Object::SetClassName("jgui::BufferedImage");

  cairo_surface_t 
    *surface = cairo_get_target(cairo_context);

  if (surface == nullptr) {
    throw jexception::RuntimeException("Unable to get target surface from cairo context");
  }

  cairo_format_t
    format = cairo_image_surface_get_format(surface);

  if (format == CAIRO_FORMAT_ARGB32) {
    _pixelformat = jgui::JPF_ARGB;
  } else if (format == CAIRO_FORMAT_RGB24) {
    _pixelformat = jgui::JPF_RGB24;
  } else if (format == CAIRO_FORMAT_RGB16_565) {
    _pixelformat = jgui::JPF_RGB16;
  } else {
    _pixelformat = jgui::JPF_UNKNOWN;
  }
  
  _size.width = cairo_image_surface_get_width(surface);
  _size.height = cairo_image_surface_get_height(surface);

  _graphics = new Graphics(cairo_context);
}

BufferedImage::BufferedImage(std::string file):
  jgui::BufferedImage(std::make_shared<jio::FileInputStream>(file).get())
{
  jcommon::Object::SetClassName("jgui::BufferedImage");
}

BufferedImage::BufferedImage(jio::InputStream *stream):
  jgui::Image(JPF_UNKNOWN, -1, -1)
{
  jcommon::Object::SetClassName("jgui::BufferedImage");

  _pixelformat = JPF_UNKNOWN;
  _size.width = -1;
  _size.height = -1;

  if ((void *)stream == nullptr) {
    throw jexception::NullPointerException("Cannot request data from a invalid stream");
  }

  int size = stream->Available();

  if (size <= 0) {
    throw jexception::NullPointerException("Cannot request avaiable data from the stream");
  }

  uint8_t buffer[size];
  int r, count = 0;

  do {
    r = stream->Read((char *)(buffer+count), 4096);

    if (r <= 0) {
      break;
    }

    count = count + r;
  } while (count < size);
  
  cairo_surface_t *cairo_surface = create_png_surface_from_data(buffer, count);

  if (cairo_surface == nullptr) {
#ifdef JPG_IMAGE
    cairo_surface = create_jpg_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef BPG_IMAGE
    cairo_surface = create_bpg_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef SVG_IMAGE
    cairo_surface = create_svg_surface_from_data(buffer, count, -1, -1);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef TIF_IMAGE
    cairo_surface = create_tif_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef JP2000_IMAGE
    cairo_surface = create_jp2000_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef BMP_IMAGE
    cairo_surface = create_bmp_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef PPM_IMAGE
    cairo_surface = create_ppm_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef HEIF_IMAGE
    cairo_surface = create_heif_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef FLIF_IMAGE
    cairo_surface = create_flif_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef WEBP_IMAGE
    cairo_surface = create_webp_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef GIF_IMAGE
    cairo_surface = create_gif_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef ICO_IMAGE
    cairo_surface = create_ico_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef PCX_IMAGE
    cairo_surface = create_pcx_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef TGA_IMAGE
    cairo_surface = create_tga_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef XBM_IMAGE
    cairo_surface = create_xbm_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef XPM_IMAGE
    cairo_surface = create_xpm_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
#ifdef MJPEG_IMAGE
    cairo_surface = create_mjpeg_surface_from_data(buffer, count);
#endif
  }

  if (cairo_surface == nullptr) {
    throw jexception::RuntimeException("Cannot open this image type");
  }

  _pixelformat = JPF_UNKNOWN;

  cairo_format_t format = cairo_image_surface_get_format(cairo_surface);

  if (format == CAIRO_FORMAT_ARGB32) {
    _pixelformat = JPF_ARGB;
  } else if (format == CAIRO_FORMAT_RGB24) {
    _pixelformat = JPF_RGB24;
  } else if (format == CAIRO_FORMAT_RGB16_565) {
    _pixelformat = JPF_RGB16;
  }

  _size.width = cairo_image_surface_get_width(cairo_surface);
  _size.height = cairo_image_surface_get_height(cairo_surface);

  cairo_t *cairo_context = cairo_create(cairo_surface);
  
  // cairo_surface_destroy(cairo_surface);

  _graphics = new Graphics(cairo_context);
}

BufferedImage::~BufferedImage()
{
  if (_graphics != nullptr) {
    delete _graphics;
    _graphics = nullptr;
  }
}

Graphics * BufferedImage::GetGraphics()
{
  return _graphics;
}

Image * BufferedImage::Flip(jflip_flags_t mode)
{
  cairo_surface_t 
    *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    return nullptr;
  }

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, _size.width, _size.height);

  cairo_t 
    *cairo_context = cairo_create(surface);

  cairo_matrix_t ms, mt, m;

  if (mode == JFF_HORIZONTAL) {
    cairo_matrix_init_scale(&ms, -1.0f, 1.0f);
    cairo_matrix_init_translate(&mt, -_size.width, 0.0f);
  } else {
    cairo_matrix_init_scale(&ms, 1.0f, -1.0f);
    cairo_matrix_init_translate(&mt, 0.0f, -_size.height);
  }

  cairo_matrix_multiply(&m, &mt, &ms);
  cairo_set_matrix(cairo_context, &m);

  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
  cairo_paint(cairo_context);

  cairo_destroy(cairo_context);
  // cairo_surface_destroy(surface);

  return new BufferedImage(cairo_context);
}

Image * BufferedImage::Shear(float dx, float dy)
{
  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    return nullptr;
  }

  int 
    tx = _size.width*fabs(dx),
    ty = _size.height*fabs(dy);

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, _size.width + tx, _size.height + ty);

  cairo_t 
    *cairo_context = cairo_create(surface);

  cairo_matrix_t m;

  cairo_matrix_init(&m,
      1.0f, dy,
      dx, 1.0f,
      0.0f, 0.0f
   );

  cairo_transform(cairo_context, &m);
  // cairo_set_matrix(cairo_context, &m);

  if (dx < 0.0f) {
    cairo_translate(cairo_context, tx, 0);
  }

  if (dy < 0.0f) {
    cairo_translate(cairo_context, 0, ty);
  }

  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
  cairo_paint(cairo_context);

  return new jgui::BufferedImage(cairo_context);
}

Image * BufferedImage::Rotate(double radians, bool resize)
{
  cairo_t *src_context = dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext();
  cairo_surface_t *src_surface = cairo_get_target(src_context);

  if (src_surface == nullptr) {
    return nullptr;
  }

  cairo_surface_flush(src_surface);

  double angle = fmod(radians, 2*M_PI);

  int iw = _size.width;
  int ih = _size.height;

  if (resize == true) {
    int precision = 10240;
    int sinTheta = (int)(precision*sin(angle));
    int cosTheta = (int)(precision*cos(angle));

    iw = (abs(_size.width*cosTheta) + abs(_size.height*sinTheta))/precision;
    ih = (abs(_size.width*sinTheta) + abs(_size.height*cosTheta))/precision;
    
    cairo_surface_t 
      *surface = nullptr;
    cairo_format_t 
      format = CAIRO_FORMAT_INVALID;

    if (_pixelformat == JPF_ARGB) {
      format = CAIRO_FORMAT_ARGB32;
    } else if (_pixelformat == JPF_RGB32) {
      format = CAIRO_FORMAT_ARGB32;
    } else if (_pixelformat == JPF_RGB24) {
      format = CAIRO_FORMAT_RGB24;
    } else if (_pixelformat == JPF_RGB16) {
      format = CAIRO_FORMAT_RGB16_565;
    }

    surface = cairo_image_surface_create(format, iw, ih);

    Image 
      *image = new BufferedImage(cairo_create(surface));

    if (GetGraphics()->GetAntialias() == JAM_NONE) {
      uint32_t src[_size.width*_size.height];
      uint32_t dst[iw*ih];

      GetRGBArray(src, 0, 0, _size.width, _size.height);

      NearesNeighborRotate(src, _size.width, _size.height, dst, iw, ih, radians, true);

      image->GetGraphics()->SetRGBArray(dst, 0, 0, iw, ih);
    } else {
      cairo_t *dst_context = dynamic_cast<Graphics *>(image->GetGraphics())->GetCairoContext();

      cairo_translate(dst_context, iw/2, ih/2);
      cairo_rotate(dst_context, -radians);
      cairo_translate(dst_context, -iw/2, -ih/2);
      cairo_set_source_surface(dst_context, src_surface, (iw - _size.width)/2, (ih - _size.height)/2);
      cairo_paint(dst_context);
    }

    return image;
  }

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, iw, ih);

  Image 
    *image = new BufferedImage(cairo_create(surface));
  
  if (GetGraphics()->GetAntialias() == JAM_NONE) {
    uint32_t src[_size.width*_size.height];
    uint32_t dst[iw*ih];

    GetRGBArray(src, 0, 0, _size.width, _size.height);

    NearesNeighborRotate(src, _size.width, _size.height, dst, iw, ih, radians, false);

    image->GetGraphics()->SetRGBArray(dst, 0, 0, iw, ih);
  } else {
    cairo_t *dst_context = dynamic_cast<Graphics *>(image->GetGraphics())->GetCairoContext();

    cairo_translate(dst_context, _size.width/2, _size.height/2);
    cairo_rotate(dst_context, -radians);
    cairo_translate(dst_context, -_size.width/2, -_size.height/2);
    cairo_set_source_surface(dst_context, src_surface, 0, 0);
    cairo_paint(dst_context);
  }

  return image;
}

Image * BufferedImage::Scale(int width, int height)
{
  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    return nullptr;
  }

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, width, height);

  cairo_t
    *cairo_context = cairo_create(surface);
  
#ifdef SVG_IMAGE
  std::string *data = (std::string *)cairo_surface_get_user_data(cairo_surface, nullptr);

  if (data != nullptr) {
    cairo_surface = create_svg_surface_from_data((uint8_t *)data->c_str(), data->size(), width, height);
    
    cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
    cairo_paint(cairo_context);

    return new jgui::BufferedImage(cairo_context);
  }
#endif

  jgui::Image
    *image = new jgui::BufferedImage(cairo_context);

  if (GetGraphics()->GetAntialias() == JAM_NONE) {
    jinterpolation_method_t method = GetInterpolationMethod();

    uint32_t src[_size.width*_size.height];
    uint32_t dst[width*height];

    GetRGBArray(src, 0, 0, _size.width, _size.height);

    if (method == JIM_NEAREST) {
      NearestNeighborScale(src, dst, _size.width, _size.height, width, height); 
    } else if (method == JIM_BILINEAR) {
      BilinearScale(src, dst, _size.width, _size.height, width, height); 
    } else if (method == JIM_BICUBIC) {
      BicubicScale(src, dst, _size.width, _size.height, width, height); 
    }

    image->GetGraphics()->SetRGBArray(dst, 0, 0, width, height);
  } else {
    cairo_t *cairo_context = dynamic_cast<Graphics *>(image->GetGraphics())->GetCairoContext();

    cairo_surface_flush(cairo_surface);
    cairo_scale(cairo_context, (double)width/_size.width, (double)height/_size.height);
    cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
    cairo_paint(cairo_context);
  }

  return image;
}

Image * BufferedImage::Crop(int x, int y, int width, int height)
{
  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    return nullptr;
  }

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, width, height);

  cairo_t
    *cairo_context = cairo_create(surface);

  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cairo_context, cairo_surface, -x, -y);
  cairo_paint(cairo_context);

  return new jgui::BufferedImage(cairo_context);
}

Image * BufferedImage::Blend(double alpha)
{
  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    return nullptr;
  }

  if (alpha < 0.0) {
    alpha = 0.0;
  }

  if (alpha > 1.0) {
    alpha = 1.0;
  }

  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, _size.width, _size.height);

  cairo_t
    *cairo_context = cairo_create(surface);

  cairo_surface_flush(cairo_surface);
  cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
  cairo_paint_with_alpha(cairo_context, alpha);

  return new jgui::BufferedImage(cairo_context);
}

Image * BufferedImage::Colorize(Color color)
{
  Image *image = (Image *)Blend(1.0);

  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(image->GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    delete image;

    return nullptr;
  }

  uint8_t *data = cairo_image_surface_get_data(cairo_surface);

  if (data == nullptr) {
    delete image;

    return nullptr;
  }

  int stride = cairo_image_surface_get_stride(cairo_surface);
  double hue, sat, bri;

  jgui::Color::RGBtoHSB(color.GetRed(), color.GetGreen(), color.GetBlue(), &hue, &sat, &bri); 

  HSLColorSpace hsl(hue, sat, 0.0);
  jgui::jsize_t size = image->GetSize();

  if (image->GetPixelFormat() == JPF_ARGB) {
    for (int j=0; j<size.height; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=4) {
        // int a = *(dst + i + 3);
        int r = *(dst + i + 2);
        int g = *(dst + i + 1);
        int b = *(dst + i + 0);

        hsl.GetRGB(&r, &g, &b);

        // *(dst + i + 3) = a;
        *(dst + i + 2) = r;
        *(dst + i + 1) = g;
        *(dst + i + 0) = b;
      }
    }
  } else if (image->GetPixelFormat() == JPF_RGB32) {
    for (int j=0; j<size.height; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=4) {
        // int a = *(dst + i + 3);
        int r = *(dst + i + 2);
        int g = *(dst + i + 1);
        int b = *(dst + i + 0);

        hsl.GetRGB(&r, &g, &b);

        // *(dst + i + 3) = a;
        *(dst + i + 2) = r;
        *(dst + i + 1) = g;
        *(dst + i + 0) = b;
      }
    }
  } else if (image->GetPixelFormat() == JPF_RGB24) {
    for (int j=0; j<size.height; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=3) {
        int r = *(dst + i + 2);
        int g = *(dst + i + 1);
        int b = *(dst + i + 0);

        hsl.GetRGB(&r, &g, &b);

        *(dst + i + 2) = r;
        *(dst + i + 1) = g;
        *(dst + i + 0) = b;
      }
    }
  } else if (image->GetPixelFormat() == JPF_RGB16) {
    for (int j=0; j<size.height; j++) {
      uint8_t *dst = (uint8_t *)(data + j * stride);

      for (int i=0; i<stride; i+=2) {
        uint16_t pixel = *((uint16_t *)dst);
        int r = (pixel >> 0x0b) & 0x1f;
        int g = (pixel >> 0x05) & 0x3f;
        int b = (pixel >> 0x00) & 0x1f;

        hsl.GetRGB(&r, &g, &b);

        *(dst + i + 1) = (r << 0x03 | g >> 0x03) & 0xff;
        *(dst + i + 0) = (g << 0x03 | b >> 0x00) & 0xff;
      }
    }
  }
  
  cairo_surface_mark_dirty(cairo_surface);

  return image;
}

uint8_t * BufferedImage::LockData()
{
  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    return nullptr;
  }

  _mutex.lock();

  return cairo_image_surface_get_data(cairo_surface);
}

void BufferedImage::UnlockData()
{
  cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<Graphics *>(GetGraphics())->GetCairoContext());

  if (cairo_surface == nullptr) {
    _mutex.unlock();

    return;
  }

  cairo_surface_mark_dirty(cairo_surface);
    
  _mutex.unlock();
}

void BufferedImage::GetRGBArray(uint32_t *rgb, int xp, int yp, int wp, int hp)
{
  if (_graphics == nullptr) {
    return;
  }

  _graphics->GetRGBArray(rgb, xp, yp, wp, hp);
}
    
jcommon::Object * BufferedImage::Clone()
{
  cairo_surface_t 
    *surface = nullptr;
  cairo_format_t 
    format = CAIRO_FORMAT_INVALID;

  if (_pixelformat == JPF_ARGB) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB32) {
    format = CAIRO_FORMAT_ARGB32;
  } else if (_pixelformat == JPF_RGB24) {
    format = CAIRO_FORMAT_RGB24;
  } else if (_pixelformat == JPF_RGB16) {
    format = CAIRO_FORMAT_RGB16_565;
  }

  surface = cairo_image_surface_create(format, _size.width, _size.height);

  jgui::Image 
    *clone = new BufferedImage(cairo_create(surface));
  jgui::Graphics 
    *g = clone->GetGraphics();
  jgui::jcomposite_flags_t 
    flags = g->GetCompositeFlags();

  g->SetCompositeFlags(jgui::JCF_SRC);

  if (g->DrawImage(this, 0, 0) == false) {
    delete clone;
    clone = nullptr;
  }

  g->SetCompositeFlags(flags);

  return clone;
}

}

