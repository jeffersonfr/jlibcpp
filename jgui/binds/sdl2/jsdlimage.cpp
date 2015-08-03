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
#include "jsdlimage.h"
#include "jsdlgraphics.h"
#include "jsdlhandler.h"
#include "jthread.h"
#include "jhslcolorspace.h"
#include "jnullpointerexception.h"

#define SDL_FIXED_POINT	(1 << 16)

namespace jgui {

	/*
class MediaLoaderThread : public jthread::Thread {
	
	private:
		jio::InputStream *_stream;
		IDirectFBDataBuffer *_buffer;
		bool _running;

	public:
		MediaLoaderThread(jio::InputStream *stream, IDirectFBDataBuffer *buffer)
		{
			_stream = stream;
			_buffer = buffer;
			_running = true;
		}

		virtual ~MediaLoaderThread()
		{
		}
	
		virtual void Cancel()
		{
			_running = false;
		}

		virtual void Run()
		{
			int limit = 64*1024,
					packet = 64*1024,
					length = 0;
			uint8_t data[packet];

			while (_running) {
				_buffer->GetLength(_buffer, (unsigned int *)&length);

				if (length >= limit) {
					continue;
				}

				length = (int)_stream->Read((char *)data, (int64_t)packet);

				if (length <= 0) {
					_buffer->Finish(_buffer);

					break;
				}

				_buffer->PutData(_buffer, data, length);

				// CHANGE:: works very well
				printf("\r");
			}
		}

};
*/

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

SDLImage::SDLImage(jpixelformat_t pixelformat, int width, int height):
	jgui::Image(pixelformat, width, height)
{
	jcommon::Object::SetClassName("jgui::SDLImage");

	_buffer = NULL;
	
	if (width < 1 || height < 1) {
		throw jcommon::RuntimeException("Invalid image size");
	}

	switch (pixelformat) {
		case JPF_RGB16:
		case JPF_RGB24:
		case JPF_RGB32:
		case JPF_ARGB:
			break;
		default:
			throw jcommon::RuntimeException("Invalid pixel format");
	}

	_graphics = new SDLGraphics(NULL, pixelformat, width, height);

	dynamic_cast<SDLHandler *>(GFXHandler::GetInstance())->Add(this);
}

SDLImage::SDLImage(std::string file):
	jgui::Image(JPF_ARGB, -1, -1)
{
	jcommon::Object::SetClassName("jgui::SDLImage");

	_graphics = new SDLGraphics(NULL, JPF_ARGB, 100, 100);

	_pixelformat = JPF_ARGB;
	_size.width = 100;
	_size.height = 100;

	dynamic_cast<SDLHandler *>(GFXHandler::GetInstance())->Add(this);
}

SDLImage::~SDLImage()
{
	dynamic_cast<SDLHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_buffer != NULL) {
		delete [] _buffer;
		_buffer = NULL;
	}

	delete _graphics;
	_graphics = NULL;
}

jsize_t SDLImage::GetImageSize(std::string img)
{
	jsize_t t;

	t.width = -1;
	t.height = -1;

	return t;
}

Image * SDLImage::CreateImageStream(jio::InputStream *stream)
{
	return NULL;
}

void SDLImage::Release()
{
}

void SDLImage::Restore()
{
}

Image * SDLImage::Flip(Image *img, jflip_flags_t mode)
{
	jsize_t size = img->GetSize();

	SDLImage *image = new SDLImage(img->GetPixelFormat(), size.width, size.height);
	
	return image;
}

Image * SDLImage::Rotate(Image *img, double radians, bool resize)
{
	jsize_t isize = img->GetSize();
	jpixelformat_t ipixel = img->GetPixelFormat();
	int precision = 1024;

	double angle = fmod(radians, 2*M_PI);

	int sinTheta = precision*sin(angle);
	int cosTheta = precision*cos(angle);

	int iw = isize.width;
	int ih = isize.height;

	if (resize == true) {
		iw = (abs(isize.width*cosTheta)+abs(isize.height*sinTheta))/precision;
		ih = (abs(isize.width*sinTheta)+abs(isize.height*cosTheta))/precision;
	}

	SDLImage *rotate = new SDLImage(ipixel, iw, ih);

	return rotate;
}

Image * SDLImage::Scale(Image *img, int width, int height)
{
	if (img == NULL) {
		return NULL;
	}

	SDLImage *image = new SDLImage(img->GetPixelFormat(), width, height);
	
	return image;
}

Image * SDLImage::Crop(Image *img, int x, int y, int width, int height)
{
	if (img == NULL) {
		return NULL;
	}

	SDLImage *image = new SDLImage(img->GetPixelFormat(), width, height);

	return image;
}

Image * SDLImage::Blend(Image *img, double alpha)
{
	SDLImage *image = new SDLImage(img->GetPixelFormat(), img->GetWidth(), img->GetHeight());
	
	return image;
}

Image * SDLImage::Colorize(Image *img, Color color)
{
	SDLImage *image = new SDLImage(img->GetPixelFormat(), img->GetWidth(), img->GetHeight());
	
	return image;
}

void SDLImage::SetPixels(uint8_t *buffer, int xp, int yp, int wp, int hp, int stride)
{
	if (buffer == NULL) {
		return;
	}
}

void SDLImage::GetPixels(uint8_t **buffer, int xp, int yp, int wp, int hp, int *stride)
{
}

}

