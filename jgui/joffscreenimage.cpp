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
#include "joffscreenimage.h"
#include "jgfxhandler.h"

namespace jgui {

OffScreenImage::OffScreenImage(int width, int height, jsurface_pixelformat_t pixelformat, int scale_width, int scale_height)
{
	jcommon::Object::SetClassName("jgui::OffScreenImage");

	graphics = NULL;

	_buffer = NULL;
	_pixelformat = pixelformat;
	_width = width;
	_height = height;
	_scale_width = scale_width;
	_scale_height = scale_height;

	if (_scale_width <= 0) {
		_scale_width = DEFAULT_SCALE_WIDTH;
	}

	if (_scale_height <= 0) {
		_scale_height = DEFAULT_SCALE_HEIGHT;
	}

#ifdef DIRECTFB_UI
	IDirectFBSurface *surface = NULL;

	GFXHandler::GetInstance()->CreateSurface(width, height, &surface, pixelformat, _scale_width, _scale_height);

	graphics = new Graphics(surface, true);

	graphics->SetWorkingScreenSize(_scale_width, _scale_height);
#endif

	GFXHandler::GetInstance()->Add(this);
}

OffScreenImage::~OffScreenImage()
{
	GFXHandler::GetInstance()->Remove(this);

#ifdef DIRECTFB_UI
	if (graphics != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)graphics->GetNativeSurface();

		if (surface != NULL) {
			// CHANGE:: ReleaseSource()->Release()
			surface->Release(surface);
		}
		
		delete graphics;
	}

#endif
}

Graphics * OffScreenImage::GetGraphics()
{
	return graphics;
}

OffScreenImage * OffScreenImage::Create()
{
	OffScreenImage *off = new OffScreenImage(_width, _height, _pixelformat, _scale_width, _scale_height);

	off->GetGraphics()->DrawImage(this, 0, 0);

	return off;
}

jsurface_pixelformat_t OffScreenImage::GetPixelFormat()
{
	return _pixelformat;
}

int OffScreenImage::GetWidth()
{
	return _width;
}

int OffScreenImage::GetHeight()
{
	return _height;
}

int OffScreenImage::GetScaleWidth()
{
	return _scale_width;
}

int OffScreenImage::GetScaleHeight()
{
	return _scale_height;
}

void OffScreenImage::Release()
{
#ifdef DIRECTFB_UI
	if (graphics != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)graphics->GetNativeSurface();
		
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
		
		delete graphics;
		graphics = NULL;
	}

#endif
}

void OffScreenImage::Restore()
{
#ifdef DIRECTFB_UI
	if (graphics == NULL) {
		IDirectFBSurface *surface = NULL;
		DFBSurfaceDescription desc;

		desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
		desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
		desc.width = SCALE_TO_SCREEN(_width, GFXHandler::GetInstance()->GetScreenWidth(), _scale_width);
		desc.height = SCALE_TO_SCREEN(_height, GFXHandler::GetInstance()->GetScreenHeight(), _scale_height);
		desc.pixelformat = DSPF_ARGB;

		GFXHandler *dfb = ((GFXHandler *)GFXHandler::GetInstance());
		IDirectFB *engine = ((IDirectFB *)dfb->GetGraphicEngine());

		if (engine->CreateSurface(engine, &desc, &surface) != DFB_OK) {
			return;
		}

		surface->SetBlittingFlags(surface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
		surface->SetDrawingFlags(surface, (DFBSurfaceDrawingFlags)(DSDRAW_BLEND));
		surface->SetPorterDuff(surface, DSPD_SRC_OVER);

		surface->Clear(surface, 0x00, 0x00, 0x00, 0x00);

		graphics = new Graphics(surface, true);

		graphics->SetWorkingScreenSize(_scale_width, _scale_height);

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

