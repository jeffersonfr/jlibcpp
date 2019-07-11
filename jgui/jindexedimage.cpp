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
#include "jgui/jindexedimage.h"
#include "jgui/jhslcolorspace.h"
#include "jexception/jruntimeexception.h"
#include "jexception/jinvalidargumentexception.h"
#include "jexception/jnullpointerexception.h"

#include <string.h>
#include <math.h>

namespace jgui {

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint8_t *data, jsize_t<int> size):
  Image(JPF_UNKNOWN, size)
{
  jcommon::Object::SetClassName("jgui::IndexedImage");
  
  _palette = new uint32_t[palette_size];
  _palette_size = palette_size;
  
  memcpy(_palette, palette, palette_size*sizeof(uint32_t));
  
  int 
    length = size.width*size.height;

  _data = new uint8_t[length];
  
  memcpy(_data, data, length*sizeof(uint8_t));
}

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint32_t *argb, jsize_t<int> size):
  Image(JPF_UNKNOWN, size)
{
  jcommon::Object::SetClassName("jgui::IndexedImage");

  _palette = new uint32_t[palette_size];
  _palette_size = palette_size;
  
  memcpy(_palette, palette, palette_size*sizeof(uint32_t));
  
  int 
    length = size.width*size.height;

  _data = new uint8_t[length];

  for (int i=0; i<length; i++) {
    _data[i] = 0;

    for (int j=0; j<_palette_size; j++) {
      if (argb[i] == _palette[j]) {
        _data[i] = j;

        break;
      }
    }
  }
}

IndexedImage::~IndexedImage()
{
  if ((void *)_palette != nullptr) {
    delete [] _palette;
    _palette = nullptr;
  }

  if ((void *)_data != nullptr) {
    delete [] _data;
    _data = nullptr;
  }
}

IndexedImage * IndexedImage::Pack(Image *image)
{
  IndexedImage *packed = nullptr;

  if ((void *)image != nullptr) {
    if (image->GetGraphics() != nullptr) {
      jgui::jsize_t<int> 
        size = image->GetSize();
      uint32_t 
        rgb[size.width*size.height];

      image->GetRGBArray(rgb, {0, 0, size.width, size.height});

      packed = Pack(rgb, size);
    }
  }

  return packed;
}

IndexedImage * IndexedImage::Pack(uint32_t *rgb, jsize_t<int> size)
{
  if ((void *)rgb == nullptr) {
    return nullptr;
  }

  uint32_t 
    palette[256];
  int 
    length = size.width*size.height,
    palette_location = 0;

  for (int i=0; i<length; i++) {
    uint32_t
      current = rgb[i];
    bool
      flag = false;

    for (int j=0; j<palette_location; j++) {
      if (palette[j] == current) {
        flag = true;

        break;
      }
    }
    
    if (flag == false) {
      palette[palette_location++] = current;

      if (palette_location >= 256) {
        throw jexception::RuntimeException("IndexedImage cannot support palettes with more than 256 colors");
      }
    }
  }

  return new IndexedImage(palette, palette_location, rgb, size);
}

Image * IndexedImage::Flip(jflip_flags_t t)
{
  jsize_t<int> 
    size = GetSize();
  uint8_t 
    *data = new uint8_t[size.width*size.height];
  
  if ((t & JFF_HORIZONTAL) != 0) {
    for (int j=0; j<size.height; j++) {
      for (int i=0; i<size.width; i++) {
        int 
          index = j*size.width,
          pixel = data[index+i];

        data[index+i] = _data[index+size.width-i-1];
        _data[index+size.width-i-1] = pixel;
      }
    }
  }

  if ((t & JFF_VERTICAL) != 0) {
    int 
      offset = (size.height-1)*size.width;

    for (int i=0; i<size.width; i++) {
      for (int j=0; j<size.height; j++) {
        int 
          index = j*size.width + i,
          pixel = data[index];

        data[index] = _data[offset-index];
        _data[offset-index] = pixel;
      }
    }
  }

  IndexedImage *image = new IndexedImage(_palette, _palette_size, data, size);

  delete [] data;

  return image;
}

Image * IndexedImage::Rotate(double radians, bool resize)
{
  IndexedImage *image = nullptr;

  jsize_t<int> 
    isize = GetSize();
  double 
    angle = fmod(radians, 2*M_PI);
  int 
    precision = 1024,
    iw = isize.width,
    ih = isize.height,
    sinTheta = (int)(precision*sin(angle)),
    cosTheta = (int)(precision*cos(angle));

  if (resize == true) {
    iw = (abs(isize.width*cosTheta)+abs(isize.height*sinTheta))/precision;
    ih = (abs(isize.width*sinTheta)+abs(isize.height*cosTheta))/precision;
  }

  uint8_t 
    *data = new uint8_t[iw*ih];
  int 
    sxc = isize.width/2,
    syc = isize.height/2,
    dxc = iw/2,
    dyc = ih/2,
    xo,
    yo,
    t1,
    t2;

  for (int j=0; j<ih; j++) {
    t1 = (j - dyc)*sinTheta;
    t2 = (j - dyc)*cosTheta;

    for (int i=0; i<iw; i++) {
      xo = ((i - dxc)*cosTheta - t1)/precision;
      yo = ((i - dxc)*sinTheta + t2)/precision;

      if (xo >= -sxc && xo < sxc && yo >= -syc && yo < syc) {
        data[j*iw + i] = _data[(yo + syc)*isize.width + (xo + sxc)];
      } else {
        data[j*iw+i] = 0;
      }
    }
  }

  image = new IndexedImage(_palette, _palette_size, data, {iw, ih});

  delete[] data;

  return image;
}

Image * IndexedImage::Scale(jsize_t<int> size)
{
  if (size.width <= 0 || size.height <= 0) {
    return nullptr;
  }

  jgui::jsize_t<int> 
    isize = GetSize();
  double 
    xRatio = isize.width/(double)size.width,
    yRatio = isize.height/(double)size.height;
  uint8_t 
    *data = new uint8_t[size.width*size.height];

  for(int y=0; y<size.height; y++) {
    double 
      src = ((int)(y * yRatio)) * isize.width;
    int 
      dst = y * size.width;

    for (int x=0; x<size.width; x++) {
      data[dst + x] = _data[(int)src];
    
      src = src + xRatio;
    }
  }

  IndexedImage *image = new IndexedImage(_palette, _palette_size, data, size);

  delete [] data;

  return image;
}

Image * IndexedImage::Crop(jrect_t<int> rect)
{
  if (rect.size.width <= 0 || rect.size.height <= 0) {
    return nullptr;
  }

  int 
    length = rect.size.width*rect.size.height;
  uint8_t 
    *data = new uint8_t[length];

  for (int i=0; i<length; i++) {
    data[i] = _data[rect.point.x + i%rect.size.width + ((rect.point.y + i/rect.size.width) * _size.width)];
  }

  IndexedImage *image = new IndexedImage(_palette, _palette_size, data, rect.size);

  delete [] data;

  return image;
}

Image * IndexedImage::Blend(double alpha)
{
  return nullptr;
}

Image * IndexedImage::Colorize(jcolor_t<float> color)
{
  jgui::jsize_t<int> 
    size = GetSize();
  uint32_t 
    palette[_palette_size];
  jmath::jvector_t<3, float>
    hsb = color.ToHSB();

  HSLColorSpace 
    hsl(hsb[0], hsb[1], 0.0);

  for (int i=0; i<_palette_size; i++) {
    jgui::jcolor_t<float>
      color(_palette[i]);
    int 
      r = color[2],
      g = color[1],
      b = color[0];

    hsl.GetRGB(&r, &g, &b);

    palette[i] = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
  }

  return new IndexedImage(palette, _palette_size, _data, size);
}

uint8_t * IndexedImage::LockData()
{
  return (uint8_t *)_data;
}

void IndexedImage::UnlockData()
{
}

void IndexedImage::GetRGBArray(uint32_t *rgb, jrect_t<int> rect)
{
  jgui::jsize_t<int> 
    size = GetSize();

  if ((rect.point.x + rect.size.width) > size.width || (rect.point.y + rect.size.height) > size.height) {
    throw jexception::InvalidArgumentException("The limits are out of bounds");
  }

  if (rgb == nullptr) {
    throw jexception::NullPointerException("Destination buffer must be valid");
  }

  for (int j=0; j<rect.size.height; j++) {
    int 
      data = (rect.point.y + j)*size.width + rect.point.x,
      line = j*rect.size.width;

    for (int i=0; i<rect.size.width; i++) {
      rgb[line + i] = _palette[_data[data + i]];
    }
  }
}
    
void IndexedImage::GetPalette(uint32_t **palette, int *size)
{
  if (palette != nullptr) {
    *palette = _palette;
  }

  if (size != nullptr) {
    *size = _palette_size;
  }
}

void IndexedImage::SetPalette(uint32_t *palette, int palette_size)
{
  _palette = new uint32_t[palette_size];

  _palette_size = palette_size;
  
  memcpy(_palette, palette, palette_size*sizeof(uint32_t));
}

jcommon::Object * IndexedImage::Clone()
{
  return (jcommon::Object *)(new IndexedImage(_palette, _palette_size, _data, _size));
}

}

