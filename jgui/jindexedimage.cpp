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
#include "Stdafx.h"
#include "jindexedimage.h"
#include "jimageexception.h"
#include "jgfxhandler.h"
#include "jruntimeexception.h"
#include "jnullgraphics.h"

#include <math.h>

namespace jgui {

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint8_t *data, int width, int height):
	Image(JPF_UNKNOWN, width, height)
{
	jcommon::Object::SetClassName("jgui::IndexedImage");
	
	_palette = new uint32_t[palette_size];
	_palette_size = palette_size;
	
	memcpy(_palette, palette, palette_size*sizeof(uint32_t));
	
	int size = width*height;

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
	
	int size = width*height;

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
	if ((void *)_palette != NULL) {
		delete [] _palette;
		_palette = NULL;
	}

	if ((void *)_data != NULL) {
		delete [] _data;
		_data = NULL;
	}
}

IndexedImage * IndexedImage::Pack(Image *image)
{
	IndexedImage *packed = NULL;

	if ((void *)image != NULL) {
		if (image->GetGraphics() != NULL) {
			int size_w = image->GetWidth(),
					size_h = image->GetHeight();
			uint32_t *rgb = NULL;

			image->GetRGB(&rgb, 0, 0, size_w, size_h);

			if ((void *)rgb != NULL) {
				packed = Pack(rgb, size_w, size_h);

				delete [] rgb;
			}
		}
	}

	return packed;
}

IndexedImage * IndexedImage::Pack(uint32_t *rgb, int width, int height)
{
	if ((void *)rgb == NULL) {
		return NULL;
	}

	uint32_t palette[256];
	uint32_t current;
	int size = width*height;
	int palette_location = 0;
	bool flag;

	for (int i=0; i<size; i++) {
		current = rgb[i];
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
				throw jcommon::RuntimeException("IndexedImage cannot support palettes with more than 256 colors");
			}
		}
	}

	return new IndexedImage(palette, palette_location, rgb, width, height);
}

Image * IndexedImage::Flip(jflip_flags_t t)
{
	IndexedImage *image = NULL;

	jsize_t size = GetSize();
	uint8_t *data = new uint8_t[size.width*size.height];
	
	if ((t & JFF_HORIZONTAL) != 0) {
		for (int j=0; j<size.height; j++) {
			for (int i=0; i<size.width; i++) {
				int index = j*size.width;
				int pixel = data[index+i];

				data[index+i] = _data[index+size.width-i-1];
				_data[index+size.width-i-1] = pixel;
			}
		}
	}

	if ((t & JFF_VERTICAL) != 0) {
		int offset = (size.height-1)*size.width;

		for (int i=0; i<size.width; i++) {
			for (int j=0; j<size.height; j++) {
				int index = j*size.width+i;
				int pixel = data[index];

				data[index] = _data[offset-index];
				_data[offset-index] = pixel;
			}
		}
	}

	image = new IndexedImage(_palette, _palette_size, data, size.width, size.height);

	delete [] data;

	return image;
}

Image * IndexedImage::Rotate(double radians, bool resize)
{
	IndexedImage *image = NULL;

	jsize_t isize = GetSize();

	double angle = fmod(radians, 2*M_PI);

	int precision = 1024;
	int sinTheta = (int)(precision*sin(angle));
	int cosTheta = (int)(precision*cos(angle));

	int iw = isize.width;
	int ih = isize.height;

	if (resize == true) {
		iw = (abs(isize.width*cosTheta)+abs(isize.height*sinTheta))/precision;
		ih = (abs(isize.width*sinTheta)+abs(isize.height*cosTheta))/precision;
	}

	uint8_t *data = new uint8_t[iw*ih];

	int sxc = isize.width/2;
	int syc = isize.height/2;
	int dxc = iw/2;
	int dyc = ih/2;
	int xo;
	int yo;
	int t1;
	int t2;

	for (int j=0; j<ih; j++) {
		t1 = (j-dyc)*sinTheta;
		t2 = (j-dyc)*cosTheta;

		for (int i=0; i<iw; i++) {
			xo = ((i-dxc)*cosTheta - t1)/precision;
			yo = ((i-dxc)*sinTheta + t2)/precision;

			if (xo >= -sxc && xo < sxc && yo >= -syc && yo < syc) {
				data[j*iw+i] = _data[(yo+syc)*isize.width + (xo+sxc)];
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
		return NULL;
	}

	IndexedImage *image = NULL;

	int srcWidth = _size.width;
	int srcHeight = _size.height;
	double yRatio = srcHeight/(double)height;
	double xRatio = srcWidth/(double)width;
	int size = width*height;
	uint8_t *data = new uint8_t[size];

	for(int y=0; y<height; y++) {
		double src = ((int)(y * yRatio)) * srcWidth;
		int dst = y * width;

		for (int x=0; x<width; x++) {
			data[dst + x] = _data[(int)src];
		
			src = src + xRatio;
		}
	}

	image = new IndexedImage(_palette, _palette_size, data, width, height);

	delete [] data;

	return image;
}

Image * IndexedImage::Crop(int x, int y, int width, int height)
{
	if (width <= 0 || height <= 0) {
		return NULL;
	}

	IndexedImage *image = NULL;

	int size = width*height;
	uint8_t *data = new uint8_t[size];

	for (int i=0; i<size; i++) {
		data[i] = _data[x + i%width + ((y + i/width) * _size.width)];
	}

	image = new IndexedImage(_palette, _palette_size, data, width, height);

	delete [] data;

	return image;
}

Image * IndexedImage::Blend(double alpha)
{
	return NULL;
}

Image * IndexedImage::Colorize(Color color)
{
	IndexedImage *image = NULL;

	uint32_t palette[_palette_size];
	int red, green, blue;
	double hue, saturation, brightness;

  Color::RGBtoHSB(color.GetRed(), color.GetGreen(), color.GetBlue(), &hue, &saturation, &brightness);

	for (int i=0; i<_palette_size; i++) {
		jgui::Color color(_palette[i]);
		double h, s, b;

  	Color::RGBtoHSB(color.GetRed(), color.GetGreen(), color.GetBlue(), &h, &s, &b);
		Color::HSBtoRGB(hue, s, b, &red, &green, &blue);

		palette[i] = (0xff << 24) | (red << 16) | (green << 8) | (blue << 0);
	}

	image = new IndexedImage(palette, _palette_size, _data, GetWidth(), GetHeight());
	
	return image;
}

void IndexedImage::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	int width = _size.width,
			height = _size.height;

	if ((xp+wp) > width || (yp+hp) > height) {
		(*rgb) = NULL;

		return;
	}

	uint32_t *buffer = NULL;

	if (*rgb == NULL) {
		buffer = new uint32_t[wp*hp];
	}

	int line,
			data;

	for (int j=0; j<hp; j++) {
		data = (yp+j)*width+xp;
		line = j*wp;

		for (int i=0; i<wp; i++) {
			buffer[line + i] = _palette[_data[data+i]];
		}
	}

	*rgb = buffer;
}
		
void IndexedImage::GetPalette(uint32_t **palette, int *size)
{
	if (palette != NULL) {
		*palette = _palette;
	}

	if (size != NULL) {
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

