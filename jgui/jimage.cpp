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
#include "nativeimage.h"
#elif defined(SDL2_UI)
#include "nativeimage.h"
#elif defined(SFML2_UI)
#include "nativeimage.h"
#elif defined(X11_UI)
#include "nativeimage.h"
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
	t = NativeImage::GetImageSize(img);
#elif defined(SDL2_UI)
	t = NativeImage::GetImageSize(img);
#elif defined(SFML2_UI)
	t = NativeImage::GetImageSize(img);
#elif defined(X11_UI)
	t = NativeImage::GetImageSize(img);
#endif

	return t;
}

Image * Image::CreateImage(jpixelformat_t pixelformat, int width, int height)
{
	Image *image = NULL;

	try {
#if defined(DIRECTFB_UI)
		image = new NativeImage(NULL, pixelformat, width, height);
#elif defined(SDL2_UI)
		image = new NativeImage(NULL, pixelformat, width, height);
#elif defined(SFML2_UI)
		image = new NativeImage(NULL, pixelformat, width, height);
#elif defined(X11_UI)
		image = new NativeImage(NULL, pixelformat, width, height);
#endif
	} catch (jcommon::RuntimeException &) {
	}

	return image;
}

Image * Image::CreateImage(uint32_t *data, int width, int height)
{
	Image *image = Image::CreateImage(JPF_ARGB, width, height);

	if (image != NULL) {
		Graphics *g = image->GetGraphics();

		g->SetCompositeFlags(JCF_SRC);
		g->SetRGBArray(data, 0, 0, width, height);
		g->Reset();
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
			image = new NativeImage(file);
#elif defined(SDL2_UI)
			image = new NativeImage(file);
#elif defined(SFML2_UI)
			image = new NativeImage(file);
#elif defined(X11_UI)
			image = new NativeImage(file);
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
	#if defined(DIRECTFB_NODEPS_UI)
			image = NativeImage::CreateImageStream(stream);
	#else
			image = new NativeImage(stream);
	#endif
#elif defined(SDL2_UI)
			image = new NativeImage(stream);
#elif defined(SFML2_UI)
			image = new NativeImage(stream);
#elif defined(X11_UI)
			image = new NativeImage(stream);
#endif
	} catch (jcommon::NullPointerException &) {
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
		image = NativeImage::Flip(this, t);
#elif defined(SDL2_UI)
		image = NativeImage::Flip(this, t);
#elif defined(SFML2_UI)
		image = NativeImage::Flip(this, t);
#elif defined(X11_UI)
		image = NativeImage::Flip(this, t);
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
		image = NativeImage::Rotate(this, radians, resize);
#elif defined(SDL2_UI)
		image = NativeImage::Rotate(this, radians, resize);
#elif defined(SFML2_UI)
		image = NativeImage::Rotate(this, radians, resize);
#elif defined(X11_UI)
		image = NativeImage::Rotate(this, radians, resize);
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
		image = NativeImage::Scale(this, width, height);
#elif defined(SDL2_UI)
		image = NativeImage::Scale(this, width, height);
#elif defined(SFML2_UI)
		image = NativeImage::Scale(this, width, height);
#elif defined(X11_UI)
		image = NativeImage::Scale(this, width, height);
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
		image = NativeImage::Crop(this, x, y, width, height);
#elif defined(SDL2_UI)
		image = NativeImage::Crop(this, x, y, width, height);
#elif defined(SFML2_UI)
		image = NativeImage::Crop(this, x, y, width, height);
#elif defined(X11_UI)
		image = NativeImage::Crop(this, x, y, width, height);
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
		image = NativeImage::Blend(this, alpha);
#elif defined(SDL2_UI)
		image = NativeImage::Blend(this, alpha);
#elif defined(SFML2_UI)
		image = NativeImage::Blend(this, alpha);
#elif defined(X11_UI)
		image = NativeImage::Blend(this, alpha);
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
		image = NativeImage::Colorize(this, color);
#elif defined(SDL2_UI)
		image = NativeImage::Colorize(this, color);
#elif defined(SFML2_UI)
		image = NativeImage::Colorize(this, color);
#elif defined(X11_UI)
		image = NativeImage::Colorize(this, color);
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
	Graphics *g = clone->GetGraphics();
	jcomposite_flags_t flags = g->GetCompositeFlags();

	g->SetCompositeFlags(jgui::JCF_SRC);

	if (g->DrawImage(this, 0, 0) == false) {
		delete clone;
		clone = NULL;
	}

	g->SetCompositeFlags(flags);

	return clone;
}

void Image::Release()
{
}

void Image::Restore()
{
}

}

