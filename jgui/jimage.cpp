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

namespace jgui {

#ifdef DIRECTFB_UI
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
#endif

Image::Image(int width, int height, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height)
{
	jcommon::Object::SetClassName("jgui::Image");

	_graphics = NULL;
	_buffer = NULL;
	_pixelformat = pixelformat;
	
	_size.width = width;
	_size.height = height;

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = scale_width;
	_scale.height = scale_height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}

#ifdef DIRECTFB_UI
	IDirectFBSurface *surface = NULL;

	GFXHandler::GetInstance()->CreateSurface(_size.width, _size.height, &surface, pixelformat, _scale.width, _scale.height);

	_graphics = new Graphics(surface, true);

	_graphics->SetWorkingScreenSize(_scale.width, _scale.height);
#endif

	GFXHandler::GetInstance()->Add(this);
}

Image::Image(int width, int height, int scale_width, int scale_height)
{
	jcommon::Object::SetClassName("jgui::Image");

	_graphics = NULL;
	_buffer = NULL;
	_pixelformat = SPF_UNKNOWN;

	_size.width = width;
	_size.height = height;

	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	_scale.width = scale_width;
	_scale.height = scale_height;

	if (_scale.width <= 0) {
		_scale.width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale.height <= 0) {
		_scale.height = DEFAULT_SCALE_HEIGHT;
	}

	GFXHandler::GetInstance()->Add(this);
}

Image::~Image()
{
	GFXHandler::GetInstance()->Remove(this);

#ifdef DIRECTFB_UI
	if (_graphics != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)_graphics->GetNativeSurface();

		if (surface != NULL) {
			// CHANGE:: ReleaseSource()->Release()
			surface->Release(surface);
		}
		
		delete _graphics;
	}

#endif
}

bool Image::GetImageSize(std::string img, int *width, int *height)
{
	if (width != NULL) {
		*width = -1;
	}

	if (height != NULL) {
		*height = -1;
	}

#ifdef DIRECTFB_UI
	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	IDirectFBImageProvider *provider = NULL;
	DFBSurfaceDescription desc;

	if (engine->CreateImageProvider(engine, img.c_str(), &provider) != DFB_OK) {
		return false;
	}

	if (provider->GetSurfaceDescription(provider, &desc) == DFB_OK) {
		provider->Release(provider);

		if (width != NULL) {
			*width = desc.width;
		}

		if (height != NULL) {
			*height = desc.height;
		}

		return true;
	}

	provider->Release(provider);
#endif
	
	return false;
}

		
Image * Image::CreateImage(int width, int height, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height)
{
	return new Image(width, height, pixelformat, scale_width, scale_height);
}

Image * Image::CreateImage(uint32_t *data, int width, int height)
{
	Image *image = new Image(width, height, SPF_ARGB, GFXHandler::GetInstance()->GetScreenWidth(), GFXHandler::GetInstance()->GetScreenHeight());

	image->GetGraphics()->SetDrawingFlags(DF_NOFX);
	image->GetGraphics()->SetRGB(data, 0, 0, width, height, width);

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
	int width,
			height;

	GetImageSize(file, &width, &height);

	if (width > 0 && height > 0) {
		image = new Image(width, height, SPF_ARGB, GFXHandler::GetInstance()->GetScreenWidth(), GFXHandler::GetInstance()->GetScreenHeight());

		if (image->GetGraphics()->DrawImage(file, 0, 0) == false) {
			delete image;
			image = NULL;
		}
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

#ifdef DIRECTFB_UI
	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	IDirectFBDataBuffer *buffer;
	IDirectFBImageProvider *provider = NULL;
	IDirectFBSurface *surface;
	DFBSurfaceDescription sdsc;

	engine->CreateDataBuffer(engine, NULL, &buffer);

	MediaLoaderThread loader(stream, buffer);

	loader.Start();

	if (buffer->CreateImageProvider(buffer, &provider) != DFB_OK) {
		loader.Cancel();
		loader.WaitThread();

		return NULL;
	}

	provider->GetSurfaceDescription(provider, &sdsc);

	engine->CreateSurface(engine, &sdsc, &surface);
	provider->RenderTo(provider, surface, NULL);

	loader.WaitThread();

	Image *image = new Image(sdsc.width, sdsc.height, SPF_ARGB, GFXHandler::GetInstance()->GetScreenWidth(), GFXHandler::GetInstance()->GetScreenHeight());

	IDirectFBSurface *image_surface = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	image_surface->Blit(image_surface, surface, NULL, 0, 0);

	surface->Release(surface);
	provider->Release(provider);
	buffer->Release( buffer );

	return image;
#endif

	return NULL;
}

Image * Image::CreateImage(Image *image)
{
	if ((void *)image == NULL) {
		return NULL;
	}

	jsize_t scale = image->GetWorkingScreenSize();

	Image *clone = new Image(image->GetWidth(), image->GetHeight(), image->GetPixelFormat(), scale.width, scale.height);

	if (clone->GetGraphics()->DrawImage(image, 0, 0) == false) {
		delete clone;
		clone = NULL;
	}

	return clone;
}

jsize_t Image::GetWorkingScreenSize()
{
	return _scale;
}

Graphics * Image::GetGraphics()
{
	return _graphics;
}

Image * Image::Scaled(int width, int height)
{
	Image *image = new Image(width, height, GetPixelFormat(), _scale.width, _scale.height);

	if (image->GetGraphics()->DrawImage(this, 0, 0, width, height) == false) {
		delete image;
		image = NULL;
	}

	return image;
}

Image * Image::SubImage(int x, int y, int width, int height)
{
	Image *image = new Image(width, height, GetPixelFormat(), _scale.width, _scale.height);

	if (image->GetGraphics()->DrawImage(this, x, y, width, height, 0, 0) == false) {
		delete image;
		image = NULL;
	}

	return image;
}

void Image::GetRGB(uint32_t **rgb, int xp, int yp, int wp, int hp)
{
	if (_graphics != NULL) {
		_graphics->GetRGB(rgb, xp, yp, wp, hp, GetWidth());

		return;
	}

	*rgb = NULL;
}
		
jsurface_pixelformat_t Image::GetPixelFormat()
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

void Image::Release()
{
#ifdef DIRECTFB_UI
	if (_graphics != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)_graphics->GetNativeSurface();
		
		void *ptr;
		int pitch,
				width,
				height;

		surface->GetSize(surface, &width, &height);
		surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_READ), &ptr, &pitch);

		if (_buffer != NULL) {
			delete [] _buffer;
			_buffer = NULL;
		}

		_buffer = new uint8_t[pitch*height];

		memcpy(_buffer, ptr, pitch*height);

		surface->Unlock(surface);

		if (surface != NULL) {
			// CHANGE:: ReleaseSource()->Release()
			surface->Release(surface);
		}
		
		delete _graphics;
		_graphics = NULL;
	}
#endif
}

void Image::Restore()
{
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

#ifdef DIRECTFB_UI
	if (_graphics == NULL) {
		IDirectFBSurface *surface = NULL;

		GFXHandler::GetInstance()->CreateSurface(_size.width, _size.height, &surface, _pixelformat, _scale.width, _scale.height);

		_graphics = new Graphics(surface, true);

		_graphics->SetWorkingScreenSize(_scale.width, _scale.height);

		void *ptr;
		int pitch,
				width,
				height;

		surface->GetSize(surface, &width, &height);
		surface->Lock(surface, (DFBSurfaceLockFlags)(DSLF_WRITE), &ptr, &pitch);

		memcpy(ptr, _buffer, pitch*height);

		surface->Unlock(surface);
		
		if (_buffer != NULL) {
			delete [] _buffer;
			_buffer = NULL;
		}
	}
#endif
}

}

