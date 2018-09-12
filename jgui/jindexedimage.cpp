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

#include <string.h>
#include <math.h>

namespace jgui {

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint8_t *data, int width, int height):
	Image(JPF_UNKNOWN, width, height)
{
	jcommon::Object::SetClassName("jgui::IndexedImage");
	
	_palette = new uint32_t[palette_size];
	_palette_size = palette_size;
	
	memcpy(_palette, palette, palette_size*sizeof(uint32_t));
	
	int 
		size = width*height;

	_data = new uint8_t[size];
	
	memcpy(_data, data, size*sizeof(uint8_t));
}

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint32_t *argb, int width, int height):
	Image(JPF_UNKNOWN, width, height)
{
	jcommon::Object::SetClassName("jgui::IndexedImage");

	_palette = new uint32_t[palette_size];
	_palette_size = palette_size;
	
	memcpy(_palette, palette, palette_size*sizeof(uint32_t));
	
	int 
		size = width*height;

	_data = new uint8_t[size];

	for (int i=0; i<size; i++) {
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
			jgui::jsize_t 
				size = image->GetSize();
			uint32_t 
				*rgb = nullptr;

			image->GetRGBArray(&rgb, 0, 0, size.width, size.height);

			if ((void *)rgb != nullptr) {
				packed = Pack(rgb, size.width, size.height);

				delete [] rgb;
			}
		}
	}

	return packed;
}

IndexedImage * IndexedImage::Pack(uint32_t *rgb, int width, int height)
{
	if ((void *)rgb == nullptr) {
		return nullptr;
	}

	uint32_t 
		palette[256];
	int 
		size = width*height,
		palette_location = 0;

	for (int i=0; i<size; i++) {
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

	return new IndexedImage(palette, palette_location, rgb, width, height);
}

Image * IndexedImage::Flip(jflip_flags_t t)
{
	jsize_t 
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

	IndexedImage *image = new IndexedImage(_palette, _palette_size, data, size.width, size.height);

	delete [] data;

	return image;
}

Image * IndexedImage::Rotate(double radians, bool resize)
{
	IndexedImage *image = nullptr;

	jsize_t 
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

	image = new IndexedImage(_palette, _palette_size, data, iw, ih);

	delete[] data;

	return image;
}

Image * IndexedImage::Scale(int width, int height)
{
	if (width <= 0 || height <= 0) {
		return nullptr;
	}

	jgui::jsize_t 
		size = GetSize();
	double 
		xRatio = size.width/(double)width,
		yRatio = size.height/(double)height;
	uint8_t 
		*data = new uint8_t[width*height];

	for(int y=0; y<height; y++) {
		double 
			src = ((int)(y * yRatio)) * size.width;
		int 
			dst = y * width;

		for (int x=0; x<width; x++) {
			data[dst + x] = _data[(int)src];
		
			src = src + xRatio;
		}
	}

	IndexedImage *image = new IndexedImage(_palette, _palette_size, data, width, height);

	delete [] data;

	return image;
}

Image * IndexedImage::Crop(int x, int y, int width, int height)
{
	if (width <= 0 || height <= 0) {
		return nullptr;
	}

	int 
		size = width*height;
	uint8_t 
		*data = new uint8_t[size];

	for (int i=0; i<size; i++) {
		data[i] = _data[x + i%width + ((y + i/width) * _size.width)];
	}

	IndexedImage *image = new IndexedImage(_palette, _palette_size, data, width, height);

	delete [] data;

	return image;
}

Image * IndexedImage::Blend(double alpha)
{
	return nullptr;
}

Image * IndexedImage::Colorize(Color color)
{
	jgui::jsize_t 
		size = GetSize();
	double 
		hue, 
		sat, 
		bri;
	uint32_t 
		palette[_palette_size];

	jgui::Color::RGBtoHSB(color.GetRed(), color.GetGreen(), color.GetBlue(), &hue, &sat, &bri); 

	HSLColorSpace 
		hsl(hue, sat, 0.0);

	for (int i=0; i<_palette_size; i++) {
		jgui::Color 
			color(_palette[i]);
		int 
			r = color.GetRed(),
			g = color.GetGreen(),
			b = color.GetBlue();

  	hsl.GetRGB(&r, &g, &b);

		palette[i] = (0xff << 24) | (r << 16) | (g << 8) | (b << 0);
	}

	return new IndexedImage(palette, _palette_size, _data, size.width, size.height);
}

void IndexedImage::SetPixels(uint8_t *pixels, int xp, int yp, int wp, int hp, int stride)
{
	uint8_t 
		*ptr = pixels;
	
	for (int j=0; j<hp; j++) {
		uint8_t 
			*src = (ptr + j*stride),
			*dst = (_data + (j+yp)*stride + xp);

		for (int i=0; i<stride; i++) {
			*(dst + i) = *(src + i);
		}
	}
}

void IndexedImage::GetPixels(uint8_t **pixels, int xp, int yp, int wp, int hp, int *stride)
{
	uint8_t 
		*ptr = (*pixels);
	
	if (ptr == nullptr) {
		ptr = new uint8_t[wp*hp];
	}

	int 
		pitch = (*stride);

	for (int j=0; j<hp; j++) {
		uint8_t 
			*src = (_data + (j+yp)*pitch + xp),
			*dst = (ptr + j*pitch);

		for (int i=0; i<wp; i++) {
			*(dst + i) = *(src + i);
		}
	}

	(*stride) = wp;
	(*pixels) = ptr;
}

void IndexedImage::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	jgui::jsize_t 
		size = GetSize();

	if ((xp + wp) > size.width || (yp + hp) > size.height) {
		(*rgb) = nullptr;

		return;
	}

	uint32_t 
		*buffer = nullptr;

	if (*rgb == nullptr) {
		buffer = new uint32_t[wp*hp];
	}

	for (int j=0; j<hp; j++) {
		int 
			data = (yp+j)*size.width+xp,
			line = j*wp;

		for (int i=0; i<wp; i++) {
			buffer[line + i] = _palette[_data[data + i]];
		}
	}

	*rgb = buffer;
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
	return (jcommon::Object *)(new IndexedImage(_palette, _palette_size, _data, _size.width, _size.height));
}

}

