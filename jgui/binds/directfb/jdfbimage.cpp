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
#include "jdfbimage.h"
#include "jdfbgraphics.h"
#include "jdfbhandler.h"
#include "jthread.h"

namespace jgui {

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

DFBImage::DFBImage(int width, int height, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height):
	jgui::Image(width, height, pixelformat, scale_width, scale_height)
{
	jcommon::Object::SetClassName("jgui::DFBImage");

	_buffer = NULL;
	_graphics = NULL;
	
	IDirectFBSurface *surface = NULL;

	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->
		CreateSurface(_size.width, _size.height, &surface, pixelformat, _scale.width, _scale.height);

	_graphics = new DFBGraphics(surface, true);

	_graphics->SetWorkingScreenSize(_scale.width, _scale.height);

	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Add(this);
}

DFBImage::~DFBImage()
{
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_graphics != NULL) {
		delete _graphics;
	}

	IDirectFBSurface *surface = (IDirectFBSurface *)_graphics->GetNativeSurface();

	if (surface != NULL) {
		surface->Release(surface);
	}
}

bool DFBImage::GetImageSize(std::string img, int *width, int *height)
{
	if (width != NULL) {
		*width = -1;
	}

	if (height != NULL) {
		*height = -1;
	}

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
	
	return false;
}

Image * DFBImage::CreateImageStream(jio::InputStream *stream)
{
	if ((void *)stream == NULL) {
		return NULL;
	}

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

	Image *image = NULL;

	provider->GetSurfaceDescription(provider, &sdsc);

	engine->CreateSurface(engine, &sdsc, &surface);
	provider->RenderTo(provider, surface, NULL);

	loader.WaitThread();

	image = new DFBImage(sdsc.width, sdsc.height, JSP_ARGB, GFXHandler::GetInstance()->GetScreenWidth(), GFXHandler::GetInstance()->GetScreenHeight());

	IDirectFBSurface *image_surface = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	image_surface->Blit(image_surface, surface, NULL, 0, 0);

	surface->Release(surface);
	provider->Release(provider);
	buffer->Release( buffer );

	return image;
}

void DFBImage::Release()
{
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
}

void DFBImage::Restore()
{
	_screen.width = GFXHandler::GetInstance()->GetScreenWidth();
	_screen.height = GFXHandler::GetInstance()->GetScreenHeight();

	if (_graphics == NULL) {
		IDirectFBSurface *surface = NULL;

		dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->
			CreateSurface(_size.width, _size.height, &surface, _pixelformat, _scale.width, _scale.height);

		_graphics = new DFBGraphics(surface, true);

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
}

jcommon::Object * DFBImage::Clone()
{
	jsize_t scale = GetWorkingScreenSize();
	
	Image *clone = new DFBImage(GetWidth(), GetHeight(), GetPixelFormat(), scale.width, scale.height);

	if (clone->GetGraphics()->DrawImage(this, 0, 0) == false) {
		delete clone;
		clone = NULL;
	}

	return clone;
}

}

