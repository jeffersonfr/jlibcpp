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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
#include "jdfbimage.h"
#include "jdfbgraphics.h"
#include "jdfbhandler.h"
#endif

namespace jgui {

Image::Image(int width, int height, jpixelformat_t pixelformat, int scale_width, int scale_height)
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

bool Image::GetImageSize(std::string img, int *width, int *height)
{
	bool b = false;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	b = DFBImage::GetImageSize(img, width, height);
#endif

	return b;
}

Image * Image::CreateImage(int width, int height, jpixelformat_t pixelformat, int scale_width, int scale_height)
{
	Image *image = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	image = new DFBImage(width, height, pixelformat, scale_width, scale_height);
#endif

	return image;
}

Image * Image::CreateImage(uint32_t *data, int width, int height, int scale_width, int scale_height)
{
	Image *image = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	image = new DFBImage(width, height, JPF_ARGB, scale_width, scale_height); // GFXHandler::GetInstance()->GetScreenWidth(), GFXHandler::GetInstance()->GetScreenHeight());

	image->GetGraphics()->SetDrawingFlags(JDF_NOFX);
	image->GetGraphics()->SetRGB(data, 0, 0, width, height, width);
#endif

	return image;
}

Image * Image::CreateImage(uint8_t *data, int size)
{
	jio::MemoryInputStream input(data, size);

	return CreateImage(&input);
}

Image * Image::CreateImage(std::string file)
{
	Image *image = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	int width,
			height;

	GetImageSize(file, &width, &height);

	if (width > 0 && height > 0) {
		image = new DFBImage(width, height, JPF_ARGB, GFXHandler::GetInstance()->GetScreenWidth(), GFXHandler::GetInstance()->GetScreenHeight());

		if (image->GetGraphics()->DrawImage(file, 0, 0) == false) {
			delete image;
			image = NULL;
		}
	}
#endif

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

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	image = DFBImage::CreateImageStream(stream);
#endif

	return image;
}

Image * Image::CreateImage(Image *image)
{
	if (image == NULL) {
		return NULL;
	}

	Image *clone = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	clone = dynamic_cast<Image *>(image->Clone());
#endif

	return clone;
}

Graphics * Image::GetGraphics()
{
	return _graphics;
}

Image * Image::Scaled(int width, int height)
{
	Image *image = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	jsize_t scale = _graphics->GetWorkingScreenSize();

	image = new DFBImage(width, height, GetPixelFormat(), scale.width, scale.height);

	if (image->GetGraphics()->DrawImage(this, 0, 0, width, height) == false) {
		delete image;
		image = NULL;
	}
#endif

	return image;
}

Image * Image::SubImage(int x, int y, int width, int height)
{
	Image *image = NULL;

#if defined(DIRECTFB_UI) || defined(DIRECTFB_CAIRO_UI)
	jsize_t scale = _graphics->GetWorkingScreenSize();

	image = new DFBImage(width, height, GetPixelFormat(), scale.width, scale.height);

	if (image->GetGraphics()->DrawImage(this, x, y, width, height, 0, 0) == false) {
		delete image;
		image = NULL;
	}
#endif

	return image;
}

void Image::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	if (_graphics != NULL) {
		_graphics->GetRGB(rgb, xp, yp, wp, hp, GetWidth());

		return;
	}

	(*rgb) = NULL;
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

void Image::Release()
{
}

void Image::Restore()
{
}

}

