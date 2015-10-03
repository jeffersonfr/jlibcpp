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
#include "genericprovider_bmp.h"
#include "genericprovider_gif.h"
#include "genericprovider_ico.h"
#include "genericprovider_jpg.h"
#include "genericprovider_pcx.h"
#include "genericprovider_png.h"
#include "genericprovider_ppm.h"
#include "genericprovider_tga.h"
#include "generichandler.h"
#include "jimage.h"
#include "jthread.h"
#include "jhslcolorspace.h"
#include "jinputstream.h"
#include "jnullpointerexception.h"

namespace jgui {

/*
uint32_t * resize_bilinear(uint32_t *pixels, int w, int h, int w2, int h2) 
{
	uint32_t *temp = new uint32_t[w2*h2];
	uint32_t a, b, c, d, x, y, index;
	double x_ratio = ((double)(w-1))/w2;
	double y_ratio = ((double)(h-1))/h2;
	double x_diff, y_diff, blue, red, green;
	int offset = 0;

	for (int i=0; i<h2; i++) {
		for (int j=0; j<w2; j++) {
			x = (int)(x_ratio * j);
			y = (int)(y_ratio * i);
			x_diff = (x_ratio * j) - x;
			y_diff = (y_ratio * i) - y;
			index = (y*w+x); 
			a = pixels[index];
			b = pixels[index+1];
			c = pixels[index+w];
			d = pixels[index+w+1];

			// blue element:: Yb = Ab(1-w)(1-h) + Bb(w)(1-h) + Cb(h)(1-w) + Db(wh)
			blue = (a&0xff)*(1-x_diff)*(1-y_diff) + (b&0xff)*(x_diff)*(1-y_diff) + (c&0xff)*(y_diff)*(1-x_diff)   + (d&0xff)*(x_diff*y_diff);
			// green element:: Yg = Ag(1-w)(1-h) + Bg(w)(1-h) + Cg(h)(1-w) + Dg(wh)
			green = ((a>>8)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>8)&0xff)*(x_diff)*(1-y_diff) + ((c>>8)&0xff)*(y_diff)*(1-x_diff)   + ((d>>8)&0xff)*(x_diff*y_diff);
			// red element:: Yr = Ar(1-w)(1-h) + Br(w)(1-h) + Cr(h)(1-w) + Dr(wh)
			red = ((a>>16)&0xff)*(1-x_diff)*(1-y_diff) + ((b>>16)&0xff)*(x_diff)*(1-y_diff) + ((c>>16)&0xff)*(y_diff)*(1-x_diff)   + ((d>>16)&0xff)*(x_diff*y_diff);

			temp[offset++] = ((((int)red) << 24) & 0xff000000) | ((((int)red) << 16) & 0xff0000) | ((((int)green) << 8) & 0xff00) | ((int)blue);
		}
	}

	return temp;
}
*/

GenericImage::GenericImage(cairo_t *cairo_context, jpixelformat_t pixelformat, int width, int height):
	jgui::Image(pixelformat, width, height)
{
	jcommon::Object::SetClassName("jgui::GenericImage");

	if (width <= 0 || height <= 0) {
		throw jcommon::RuntimeException("Image must have width and height greater than 0");
	}

	_graphics = new GenericGraphics(NULL, cairo_context, pixelformat, width, height);

	dynamic_cast<GenericHandler *>(GFXHandler::GetInstance())->Add(this);
}

GenericImage::GenericImage(std::string file):
	jgui::Image(JPF_ARGB, -1, -1)
{
	jcommon::Object::SetClassName("jgui::GenericImage");

	cairo_surface_t *cairo_surface = create_png_surface_from_file(file.c_str());
	
	if (cairo_surface == NULL) {
		cairo_surface = create_jpg_surface_from_file(file.c_str());
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_bmp_surface_from_file(file.c_str());
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_ppm_surface_from_file(file.c_str());
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_gif_surface_from_file(file.c_str());
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_ico_surface_from_file(file.c_str());
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_pcx_surface_from_file(file.c_str());
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_tga_surface_from_file(file.c_str());
	}

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

GenericImage::GenericImage(jio::InputStream *stream):
	jgui::Image(JPF_ARGB, -1, -1)
{
	if ((void *)stream == NULL) {
		throw jcommon::NullPointerException("Cannot request data from a invalid stream");
	}

	int size = stream->Available();

	if (size <= 0) {
		throw jcommon::NullPointerException("Cannot request avaiable data from the stream");
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

	if (cairo_surface == NULL) {
		cairo_surface = create_jpg_surface_from_data(buffer, count);
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_bmp_surface_from_data(buffer, count);
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_ppm_surface_from_data(buffer, count);
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_gif_surface_from_data(buffer, count);
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_ico_surface_from_data(buffer, count);
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_pcx_surface_from_data(buffer, count);
	}

	if (cairo_surface == NULL) {
		cairo_surface = create_tga_surface_from_data(buffer, count);
	}

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

jsize_t GenericImage::GetImageSize(std::string file)
{
	GenericImage *image = NULL;
	
	jsize_t t;

	t.width = -1;
	t.height = -1;

	try {
		image = new GenericImage(file);

		t = image->GetSize();

		delete image;
	} catch (jcommon::Exception &e) {
	}

	return t;
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

