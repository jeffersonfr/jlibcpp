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
#include "genericimage.h"
#include "genericgraphics.h"
#include "generichandler.h"
#include "jimage.h"
#include "jthread.h"
#include "jhslcolorspace.h"
#include "jnullpointerexception.h"

namespace jgui {

GenericImage::GenericImage(cairo_t *cairo_context, jpixelformat_t pixelformat, int width, int height):
	jgui::Image(pixelformat, width, height)
{
	jcommon::Object::SetClassName("jgui::GenericImage");

	if (width < 1 || height < 1) {
		throw jcommon::RuntimeException("Invalid image size");
	}

	_graphics = new GenericGraphics(NULL, cairo_context, pixelformat, width, height);

	dynamic_cast<GenericHandler *>(GFXHandler::GetInstance())->Add(this);
}

GenericImage::GenericImage(std::string file):
	jgui::Image(JPF_ARGB, -1, -1)
{
	jcommon::Object::SetClassName("jgui::GenericImage");

	cairo_surface_t *cairo_surface = cairo_image_surface_create_from_png(file.c_str());
	
	if (cairo_surface == NULL) {
		throw jcommon::RuntimeException("Cannot open this image type");
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

	_graphics = new GenericGraphics(NULL, cairo_context, _pixelformat, _size.width, _size.height);

	dynamic_cast<GenericHandler *>(GFXHandler::GetInstance())->Add(this);
}

GenericImage::~GenericImage()
{
	dynamic_cast<GenericHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_graphics != NULL) {
		delete _graphics;
		_graphics = NULL;
	}
}

jsize_t GenericImage::GetImageSize(std::string img)
{
	jsize_t t;

	t.width = -1;
	t.height = -1;

	/*
	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	IDirectFBImageProvider *provider = NULL;
	GenericSurfaceDescription desc;

	if (engine->CreateImageProvider(engine, img.c_str(), &provider) != Generic_OK) {
		return t;
	}

	if (provider->GetSurfaceDescription(provider, &desc) != Generic_OK) {
		provider->Release(provider);

		return t;
	}

	t.width = desc.width;
	t.height = desc.height;
		
	provider->Release(provider);
	*/

	return t;
}

Image * GenericImage::CreateImageStream(jio::InputStream *stream)
{
	Image *image = NULL;

	/*
	if ((void *)stream == NULL) {
		return NULL;
	}

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	IDirectFBDataBuffer *buffer;
	IDirectFBImageProvider *provider = NULL;
	IDirectFBSurface *surface;
	GenericSurfaceDescription sdsc;

	engine->CreateDataBuffer(engine, NULL, &buffer);

	MediaLoaderThread loader(stream, buffer);

	loader.Start();

	if (buffer->CreateImageProvider(buffer, &provider) != Generic_OK) {
		loader.Cancel();
		loader.WaitThread();

		return NULL;
	}

	provider->GetSurfaceDescription(provider, &sdsc);

	engine->CreateSurface(engine, &sdsc, &surface);
	provider->RenderTo(provider, surface, NULL);

	loader.WaitThread();

	image = new GenericImage(NULL, JPF_ARGB, sdsc.width, sdsc.height);

	// INFO:: draw image pixels to cairo's surface
	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_READ, &ptr, &pitch);

	image->GetGraphics()->SetRGBArray((uint32_t *)ptr, 0, 0, sdsc.width, sdsc.height);

	surface->Unlock(surface);

	surface->Release(surface);
	provider->Release(provider);
	buffer->Release( buffer );
	*/

	return image;
}

void GenericImage::Release()
{
}

void GenericImage::Restore()
{
}

Image * GenericImage::Flip(Image *img, jflip_flags_t mode)
{
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(img->GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return NULL;
	}

	int width = img->GetWidth();
	int height = img->GetHeight();

	GenericImage *image = new GenericImage(NULL, img->GetPixelFormat(), width, height);
	cairo_t *cairo_context = dynamic_cast<GenericGraphics *>(image->GetGraphics())->_cairo_context;

	cairo_surface_flush(cairo_surface);

	cairo_matrix_t ms, mt, m;

	if (mode == JFF_HORIZONTAL) {
		cairo_matrix_init_scale(&ms, -1.0f, 1.0f);
		cairo_matrix_init_translate(&mt, -width, 0.0f);
	} else {
		cairo_matrix_init_scale(&ms, 1.0f, -1.0f);
		cairo_matrix_init_translate(&mt, 0.0f, -height);
	}

	cairo_matrix_multiply(&m, &mt, &ms);
	cairo_set_matrix(cairo_context, &m);

	cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
	cairo_paint(cairo_context);

	return image;
}

Image * GenericImage::Rotate(Image *img, double radians, bool resize)
{
	cairo_t *src_context = dynamic_cast<GenericGraphics *>(img->GetGraphics())->_cairo_context;
	cairo_surface_t *src_surface = cairo_get_target(src_context);

	if (src_surface == NULL) {
		return NULL;
	}

	cairo_surface_flush(src_surface);

	jsize_t isize = img->GetSize();

	double angle = fmod(radians, 2*M_PI);

	int iw = isize.width;
	int ih = isize.height;

	if (resize == true) {
		int precision = 10240;
		int sinTheta = (int)(precision*sin(angle));
		int cosTheta = (int)(precision*cos(angle));

		iw = (abs(isize.width*cosTheta)+abs(isize.height*sinTheta))/precision;
		ih = (abs(isize.width*sinTheta)+abs(isize.height*cosTheta))/precision;
		
		GenericImage *dst = new GenericImage(NULL, img->GetPixelFormat(), iw, ih);
		cairo_t *dst_context = dynamic_cast<GenericGraphics *>(dst->GetGraphics())->_cairo_context;

		cairo_translate(dst_context, iw/2, ih/2);
		cairo_rotate(dst_context, -radians);
		cairo_translate(dst_context, -iw/2, -ih/2);
		cairo_set_source_surface(dst_context, src_surface, (iw-isize.width)/2, (ih-isize.height)/2);
		cairo_paint(dst_context);

		return dst;
	}

	GenericImage *dst = new GenericImage(NULL, img->GetPixelFormat(), iw, ih);
	cairo_t *dst_context = dynamic_cast<GenericGraphics *>(dst->GetGraphics())->_cairo_context;

	cairo_translate(dst_context, isize.width/2, isize.height/2);
	cairo_rotate(dst_context, -radians);
	cairo_translate(dst_context, -isize.width/2, -isize.height/2);
	cairo_set_source_surface(dst_context, src_surface, 0, 0);
	cairo_paint(dst_context);

	return dst;
}

Image * GenericImage::Scale(Image *img, int width, int height)
{
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(img->GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return NULL;
	}

	GenericImage *image = new GenericImage(NULL, img->GetPixelFormat(), width, height);
	cairo_t *cairo_context = dynamic_cast<GenericGraphics *>(image->GetGraphics())->_cairo_context;

	cairo_surface_flush(cairo_surface);
	cairo_scale(cairo_context, (double)width/img->GetWidth(), (double)height/img->GetHeight());
	cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
	cairo_paint(cairo_context);

	return image;
}

Image * GenericImage::Crop(Image *img, int x, int y, int width, int height)
{
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(img->GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return NULL;
	}

	GenericImage *image = new GenericImage(NULL, img->GetPixelFormat(), width, height);
	cairo_t *cairo_context = dynamic_cast<GenericGraphics *>(image->GetGraphics())->_cairo_context;

	cairo_surface_flush(cairo_surface);
	cairo_set_source_surface(cairo_context, cairo_surface, -x, -y);
	cairo_paint(cairo_context);

	return image;
}

Image * GenericImage::Blend(Image *img, double alpha)
{
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(img->GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return NULL;
	}

	if (alpha < 0.0) {
		alpha = 0.0;
	}

	if (alpha > 1.0) {
		alpha = 1.0;
	}

	GenericImage *image = new GenericImage(NULL, img->GetPixelFormat(), img->GetWidth(), img->GetHeight());
	cairo_t *cairo_context = dynamic_cast<GenericGraphics *>(image->GetGraphics())->_cairo_context;

	cairo_surface_flush(cairo_surface);
	cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
	cairo_paint_with_alpha(cairo_context, alpha);

	return image;
}

Image * GenericImage::Colorize(Image *img, Color color)
{
	GenericImage *image = (GenericImage *)Blend(img, 1.0);

	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(image->GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		delete image;

		return NULL;
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		delete image;

		return NULL;
	}

	int stride = cairo_image_surface_get_stride(cairo_surface);
	double hue, sat, bri;

	jgui::Color::RGBtoHSB(color.GetRed(), color.GetGreen(), color.GetBlue(), &hue, &sat, &bri); 

	HSLColorSpace hsl(hue, sat, 0.0);

	if (image->GetPixelFormat() == JPF_ARGB) {
		for (int j=0; j<image->GetHeight(); j++) {
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
		for (int j=0; j<image->GetHeight(); j++) {
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
		for (int j=0; j<image->GetHeight(); j++) {
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
		for (int j=0; j<image->GetHeight(); j++) {
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

	/*
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(img->GetGraphics())->_cairo_context);

	GenericImage *image = new GenericImage(NULL, img->GetPixelFormat(), img->GetWidth(), img->GetHeight());
	cairo_t *cairo_context = dynamic_cast<GenericGraphics *>(image->GetGraphics())->_cairo_context;

	cairo_surface_t *ref_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, img->GetWidth(), img->GetHeight());
	cairo_t *ref_context = cairo_create(ref_surface);
	
	cairo_set_source_rgb(ref_context, r/255.0, g/255.0, b/255.0);
	cairo_paint(ref_context);

	cairo_surface_flush(cairo_surface);
	cairo_set_source_surface(cairo_context, cairo_surface, 0, 0);
	cairo_paint(cairo_context);

	cairo_set_operator(cairo_context, CAIRO_OPERATOR_HSL_COLOR);
	cairo_set_source_surface(cairo_context, ref_surface, 0, 0);
	cairo_paint(cairo_context);

	cairo_destroy(ref_context);
	cairo_surface_destroy(ref_surface);
	*/

	return image;
}

void GenericImage::SetPixels(uint8_t *buffer, int xp, int yp, int wp, int hp, int stride)
{
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	if (stride <= 0) {
		stride = cairo_image_surface_get_stride(cairo_surface);
	}

	xp = (xp*stride)/GetWidth();
	wp = (wp*stride)/GetWidth();

	for (int j=0; j<hp; j++) {
		uint8_t *src = (uint8_t *)(buffer + j * stride);
		uint8_t *dst = (uint8_t *)(data + (j + yp) * stride);

		for (int i=0; i<wp; i++) {
			*(dst + (i + xp)) = *(src + i);
		}
	}

	cairo_surface_mark_dirty(cairo_surface);
}

void GenericImage::GetPixels(uint8_t **buffer, int xp, int yp, int wp, int hp, int *stride)
{
	cairo_surface_t *cairo_surface = cairo_get_target(dynamic_cast<GenericGraphics *>(GetGraphics())->_cairo_context);

	if (cairo_surface == NULL) {
		return;
	}

	uint8_t *data = cairo_image_surface_get_data(cairo_surface);

	if (data == NULL) {
		return;
	}

	int pitch = cairo_image_surface_get_stride(cairo_surface);

	xp = (xp*pitch)/GetWidth();
	wp = (wp*pitch)/GetWidth();

	for (int j=0; j<hp; j++) {
		uint8_t *src = (uint8_t *)(data + (j + yp) * pitch + xp);
		uint8_t *dst = (uint8_t *)(*buffer + j * pitch);

		for (int i=0; i<wp; i++) {
			*(dst + i) = *(src + i);
		}
	}

	(*stride) = pitch;
}

}

