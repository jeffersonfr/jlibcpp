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
	Image(width, height, JPF_UNKNOWN, 0, 0)
{
	jcommon::Object::SetClassName("jgui::IndexedImage");
	
	GFXHandler *handler = GFXHandler::GetInstance();

	_graphics = new NullGraphics();

	_graphics->SetWorkingScreenSize(handler->GetScreenWidth(), handler->GetScreenHeight());

	_palette = new uint32_t[palette_size];
	_palette_size = palette_size;
	
	memcpy(_palette, palette, palette_size*sizeof(uint32_t));
	
	int size = width*height;

	_data = new uint8_t[size];
	
	memcpy(_data, data, size*sizeof(uint8_t));
}

IndexedImage::IndexedImage(uint32_t *palette, int palette_size, uint32_t *argb, int width, int height):
	Image(width, height, JPF_UNKNOWN, 0, 0)
{
	jcommon::Object::SetClassName("jgui::IndexedImage");

	GFXHandler *handler = GFXHandler::GetInstance();

	_graphics = new NullGraphics();

	_graphics->SetWorkingScreenSize(handler->GetScreenWidth(), handler->GetScreenHeight());

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
			GFXHandler *handler = GFXHandler::GetInstance();
			jsize_t scale = image->GetGraphics()->GetWorkingScreenSize();
			int size_w = image->GetWidth(),
					size_h = image->GetHeight();
			uint32_t *rgb = NULL;

			image->GetRGB(&rgb, 0, 0, size_w, size_h);

			if ((void *)rgb != NULL) {
				packed = Pack(rgb, SCALE_TO_SCREEN(size_w, handler->GetScreenWidth(), scale.width), SCALE_TO_SCREEN(size_h, handler->GetScreenHeight(), scale.height));

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

Image * IndexedImage::Scale(int width, int height)
{
	if (width <= 0 || height <= 0) {
		return NULL;
	}

	int size = width*height,
			srcWidth = GetWidth(),
			srcHeight = GetHeight(),
			srcSize = GetWidth()*GetHeight(),
			yRatio = (srcHeight << 16) / height,
			xRatio = (srcWidth << 16) / width,
			xPos = xRatio / 2,
			yPos = yRatio / 2;
	uint8_t data[size];

	for (int x = 0; x < width; x++) {
		int srcX = xPos >> 16;

		for(int y = 0 ; y < height ; y++) {
			int dpixel = x + y * width,
					spixel = srcX + (yPos >> 16) * srcWidth;

			if((dpixel >= 0 && dpixel < size) && (spixel >= 0 && spixel < srcSize)) {
				data[dpixel] = _data[spixel];
			}

			yPos = yPos + yRatio;
		}

		yPos = yRatio / 2;
		xPos = xPos + xRatio;
	}

	return new IndexedImage(_palette, _palette_size, data, width, height);
}

Image * IndexedImage::SubImage(int x, int y, int width, int height)
{
	if (width <= 0 || height <= 0) {
		return NULL;
	}

	int size = width*height;
	uint8_t data[size];

	for (int i=0; i<size; i++) {
		data[i] = _data[x + i%width + ((y + i/width) * _size.width)];
	}

	return new IndexedImage(_palette, _palette_size, data, width, height);
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

void IndexedImage::Release()
{
	// do nothing
}

void IndexedImage::Restore()
{
	// do nothing
}

}

