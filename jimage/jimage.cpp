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
#include "jimage.h"
#include "jimageexception.h"
#include "joutofboundsexception.h"

namespace jimage {

Image::Image()
{
	_header = NULL;
	_header_length = 0;
	_pixels = NULL;
	_pixels_length = 0;
	_width = -1;
	_height = -1;
	_type = UNKNOWN_TYPE;
}

Image::~Image()
{
	if (_header != NULL) {
		delete _header;
	}

	if (_pixels != NULL) {
		delete _pixels;
	}
}

void Image::SetPixel(int x, int y, pixel_t pixel)
{
	int offset;

	if (_type == BW_TYPE) {
	} else if (_type == GRAY_TYPE) {
	} else if (_type == RGB_TYPE) {
		offset = 3*(y*_width + x);

		_pixels[offset + 0] = (pixel >> 0x00) & 0xff;
		_pixels[offset + 1] = (pixel >> 0x08) & 0xff;
		_pixels[offset + 2] = (pixel >> 0x10) & 0xff;
	} else if (_type == BGR_TYPE) {
	} else if (_type == ARGB_TYPE) {
	} else if (_type == RGBA_TYPE) {
	} else if (_type == UNKNOWN_TYPE) {
	}
}

pixel_t Image::GetPixel(int x, int y)
{

	if (_type == UNKNOWN_TYPE) {
		return 0x00000000;
	}

	if ((x < 0 || x > _width) || (y < 0 || y > _height)) {
		throw jcommon::OutOfBoundsException("Get pixel out of range from image");
	}

	if (_type == GRAY_TYPE) {
		uint8_t *pixel = (uint8_t *)_pixels;
		int pitch = 1 * _width,
			line = y * pitch;
		
		return (pixel_t)pixel[line + x];
	} else if (_type == RGB_TYPE) {
		uint8_t *pixel = (uint8_t *)_pixels;
		int pitch = 3 * _width,
			line = y * pitch;

		return (pixel_t)((pixel[line + 3*x + 0] << 0 | pixel[line + 3*x + 1] << 8 | pixel[line + 3*x + 2] << 16) & 0x00ffffff);
	} else if (_type == BGR_TYPE) {
		uint8_t *pixel = (uint8_t *)_pixels;
		int pitch = 3 * _width,
			line = y * pitch;

		return (pixel_t)((pixel[line + x + 0] << 16 | pixel[line + x + 1] << 8 | pixel[line + x + 2] << 0) & 0x00ffffff);
	} else if (_type == ARGB_TYPE) {
		uint8_t *pixel = (uint8_t *)_pixels;
		int pitch = 4 * _width,
			line = y * pitch;

		return (pixel_t)((pixel[line + x + 0] << 0 | pixel[line + x + 1] << 8 | pixel[line + x + 2] << 16 | pixel[line + x + 3] >> 24) & 0x00ffffff);
	} else if (_type == RGBA_TYPE) {
		uint8_t *pixel = (uint8_t *)_pixels;
		int pitch = 4 * _width,
			line = y * pitch;

		return (pixel_t)((pixel[line + x + 0] << 24 | pixel[line + x + 1] << 16 | pixel[line + x + 2] << 8 | pixel[line + x + 3] >> 0) & 0x00ffffff);
	}

	return (pixel_t)0x00000000;
}

void Image::SetPixels(void **pixels, int *size_buffer)
{
	if (_type == UNKNOWN_TYPE) {
		pixels = NULL;
		size_buffer = 0;
		
		throw ImageException("Unknown image format");
	}

	if ((void *)(*pixels) == NULL) {
		pixels = NULL;
		size_buffer = 0;
		
		throw ImageException("Null pointer exception");
	}

	/*
	if (_type == UNKNOWN_TYPE) {
		pixels = NULL;
		size_buffer = 0;
		
		return;
	}

	(*pixels) = (void *)_pixels;
	*size_buffer = _pixels_length;
	*/

	/* CHANGE::
	void *p = (void *)new uint8_t[_pixels_length];
	
	memcpy(p, _pixels, _pixels_length);
	pixels = (void *)p;
	*size_buffer = _pixels_length;
	*/
}

void Image::GetPixels(void **pixels, int *size_buffer)
{
	if (_type == UNKNOWN_TYPE) {
		pixels = NULL;
		size_buffer = 0;
		
		throw ImageException("Unknown image format");
	}

	if ((void *)(*pixels) == NULL) {
		pixels = NULL;
		size_buffer = 0;
		
		throw ImageException("Null pointer exception");
	}

	memcpy((*pixels), _pixels, _pixels_length);
	*size_buffer = _pixels_length;
}

void Image::Lock(uint8_t **pixels, int *size_buffer)
{
	_mutex.Lock();

	if (_type == UNKNOWN_TYPE) {
		pixels = NULL;
		size_buffer = 0;
		
		throw ImageException("Unknown image format");
	}

	(*pixels) = (uint8_t *)_pixels;
	*size_buffer = _pixels_length;
}

void Image::Unlock()
{
	_mutex.Unlock();
}

int Image::GetWidth()
{
	return _width;
}

int Image::GetHeight()
{
	return _height;
}

jimage_type_t Image::GetType()
{
	return _type;
}

void Image::GetRawImage(void *img, int *size_img)
{
	if (_type == UNKNOWN_TYPE) {
		img = NULL;
		size_img = 0;
		
		throw ImageException("Unknown image format");
	}

	char *p = new char[_header_length + _pixels_length];
	
	memcpy(p, _header, _header_length);
	memcpy((p + _header_length), _pixels, _pixels_length);
	img = (void *)p;
	*size_img = _header_length + _pixels_length;
}

std::string Image::what()
{
	return "Unknown Image Type";
}

}

