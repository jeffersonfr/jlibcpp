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
#include "jgfxhandler.h"
#include "jmemoryinputstream.h"
#include "jthread.h"
#include "jnullpointerexception.h"

#if defined(DIRECTFB_UI)
#include "jdfbimage.h"
#elif defined(DIRECTFB_CAIRO_UI)
#include "jdfbimage.h"
#elif defined(GTK3_UI)
#include "jgtkimage.h"
#elif defined(SDL2_UI)
#include "jsdlimage.h"
#endif

namespace jgui {

Image::Image(jpixelformat_t pixelformat, int width, int height)
{
	jcommon::Object::SetClassName("jgui::Image");

	_graphics = NULL;
	_pixelformat = pixelformat;

	_size.width = width;
	_size.height = height;
}

Image::~Image()
{
}

jsize_t Image::GetImageSize(std::string img)
{
	jsize_t t;

	t.width = -1;
	t.height = -1;

#if defined(DIRECTFB_UI)
	t = DFBImage::GetImageSize(img);
#elif defined(DIRECTFB_CAIRO_UI)
	t = DFBImage::GetImageSize(img);
#elif defined(GTK3_UI)
	t = GTKImage::GetImageSize(img);
#elif defined(SDL2_UI)
	t = SDLImage::GetImageSize(img);
#endif

	return t;
}

Image * Image::CreateImage(jpixelformat_t pixelformat, int width, int height)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = new DFBImage(pixelformat, width, height);
#elif defined(DIRECTFB_CAIRO_UI)
		image = new DFBImage(pixelformat, width, height);
#elif defined(GTK3_UI)
		image = new GTKImage(pixelformat, width, height);
#elif defined(SDL2_UI)
		image = new SDLImage(pixelformat, width, height);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::CreateImage(uint32_t *data, int width, int height)
{
	Image *image = Image::CreateImage(JPF_ARGB, width, height);

	if (image != NULL) {
		image->GetGraphics()->SetRGBArray(data, 0, 0, width, height);
	}

	return image;
}

Image * Image::CreateImage(char *data, int size)
{
	jio::MemoryInputStream input(data, size);

	return CreateImage(&input);
}

Image * Image::CreateImage(std::string file)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
			image = new DFBImage(file);
#elif defined(DIRECTFB_CAIRO_UI)
			image = new DFBImage(file);
#elif defined(GTK3_UI)
			image = new GTKImage(file);
#elif defined(SDL2_UI)
			image = new SDLImage(file);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::CreateImage(jio::File *file)
{
	if ((void *)file == NULL) {
		return NULL;
	}

	return CreateImage(file->GetPath());
}

Image * Image::CreateImage(jio::InputStream *stream)
{
	if ((void *)stream == NULL) {
		return NULL;
	}

	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::CreateImageStream(stream);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::CreateImageStream(stream);
#elif defined(GTK3_UI)
		image = GTKImage::CreateImageStream(stream);
#elif defined(SDL2_UI)
		image = SDLImage::CreateImageStream(stream);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::CreateImage(Image *image)
{
	if (image == NULL) {
		return NULL;
	}

	return dynamic_cast<Image *>(image->Clone());
}

Graphics * Image::GetGraphics()
{
	return _graphics;
}

jpixelformat_t Image::GetPixelFormat()
{
	return _pixelformat;
}

int Image::GetWidth()
{
	return _size.width;
}

int Image::GetHeight()
{
	return _size.height;
}

jsize_t Image::GetSize()
{
	return _size;
}

Image * Image::Flip(jflip_flags_t t)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::Flip(this, t);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::Flip(this, t);
#elif defined(GTK3_UI)
		image = GTKImage::Flip(this, t);
#elif defined(SDL2_UI)
		image = SDLImage::Flip(this, t);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::Rotate(double radians, bool resize)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::Rotate(this, radians, resize);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::Rotate(this, radians, resize);
#elif defined(GTK3_UI)
		image = GTKImage::Rotate(this, radians, resize);
#elif defined(SDL2_UI)
		image = SDLImage::Rotate(this, radians, resize);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::Scale(int width, int height)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::Scale(this, width, height);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::Scale(this, width, height);
#elif defined(GTK3_UI)
		image = GTKImage::Scale(this, width, height);
#elif defined(SDL2_UI)
		image = SDLImage::Scale(this, width, height);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::Crop(int x, int y, int width, int height)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::Crop(this, x, y, width, height);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::Crop(this, x, y, width, height);
#elif defined(GTK3_UI)
		image = GTKImage::Crop(this, x, y, width, height);
#elif defined(SDL2_UI)
		image = SDLImage::Crop(this, x, y, width, height);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::Blend(double alpha)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::Blend(this, alpha);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::Blend(this, alpha);
#elif defined(GTK3_UI)
		image = GTKImage::Blend(this, alpha);
#elif defined(SDL2_UI)
		image = SDLImage::Blend(this, alpha);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::Colorize(Color color)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = DFBImage::Colorize(this, color);
#elif defined(DIRECTFB_CAIRO_UI)
		image = DFBImage::Colorize(this, color);
#elif defined(GTK3_UI)
		image = GTKImage::Colorize(this, color);
#elif defined(SDL2_UI)
		image = SDLImage::Colorize(this, color);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

void Image::SetPixels(uint8_t *buffer, int xp, int yp, int wp, int hp, int stride)
{
}

void Image::GetPixels(uint8_t **buffer, int xp, int yp, int wp, int hp, int *stride)
{
	(*buffer) = NULL;
}

void Image::GetRGBArray(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	if (_graphics != NULL) {
		_graphics->GetRGBArray(rgb, xp, yp, wp, hp);

		return;
	}

	(*rgb) = NULL;
}
		
jcommon::Object * Image::Clone()
{
	Image *clone = CreateImage(GetPixelFormat(), GetWidth(), GetHeight());

	if (clone->GetGraphics()->DrawImage(this, 0, 0) == false) {
		delete clone;
		clone = NULL;
	}

	return clone;
}

void Image::Release()
{
}

void Image::Restore()
{
}

}

