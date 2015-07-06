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
#include "jhslcolorspace.h"
#include "jnullpointerexception.h"

#define DFB_FIXED_POINT	(1 << 16)

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

bool CreateSurface(IDirectFBSurface **surface, jpixelformat_t pixelformat, int wp, int hp)
{
	DFBSurfaceDescription desc;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	// desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	desc.width = wp;
	desc.height = hp;

	if (pixelformat == JPF_UNKNOWN) {
		desc.pixelformat = DSPF_UNKNOWN;
	} else if (pixelformat == JPF_ARGB1555) {
		desc.pixelformat = DSPF_ARGB1555;
	} else if (pixelformat == JPF_RGB16) {
		desc.pixelformat = DSPF_RGB16;
	} else if (pixelformat == JPF_RGB24) {
		desc.pixelformat = DSPF_RGB24;
	} else if (pixelformat == JPF_RGB32) {
		desc.pixelformat = DSPF_RGB32;
	} else if (pixelformat == JPF_ARGB) {
		desc.pixelformat = DSPF_ARGB;
	} else if (pixelformat == JPF_A8) {
		desc.pixelformat = DSPF_A8;
	} else if (pixelformat == JPF_YUY2) {
		desc.pixelformat = DSPF_YUY2;
	} else if (pixelformat == JPF_RGB332) {
		desc.pixelformat = DSPF_RGB332;
	} else if (pixelformat == JPF_UYVY) {
		desc.pixelformat = DSPF_UYVY;
	} else if (pixelformat == JPF_I420) {
		desc.pixelformat = DSPF_I420;
	} else if (pixelformat == JPF_YV12) {
		desc.pixelformat = DSPF_YV12;
	} else if (pixelformat == JPF_LUT8) {
		desc.pixelformat = DSPF_LUT8;
	} else if (pixelformat == JPF_ALUT44) {
		desc.pixelformat = DSPF_ALUT44;
	} else if (pixelformat == JPF_AiRGB) {
		desc.pixelformat = DSPF_AiRGB;
	} else if (pixelformat == JPF_A1) {
		desc.pixelformat = DSPF_A1;
	} else if (pixelformat == JPF_NV12) {
		desc.pixelformat = DSPF_NV12;
	} else if (pixelformat == JPF_NV16) {
		desc.pixelformat = DSPF_NV16;
	} else if (pixelformat == JPF_ARGB2554) {
		desc.pixelformat = DSPF_ARGB2554;
	} else if (pixelformat == JPF_ARGB4444) {
		desc.pixelformat = DSPF_ARGB4444;
	} else if (pixelformat == JPF_RGBA4444) {
		desc.pixelformat = DSPF_RGBA4444;
	} else if (pixelformat == JPF_NV21) {
		desc.pixelformat = DSPF_NV21;
	} else if (pixelformat == JPF_AYUV) {
		desc.pixelformat = DSPF_AYUV;
	} else if (pixelformat == JPF_A4) {
		desc.pixelformat = DSPF_A4;
	} else if (pixelformat == JPF_ARGB1666) {
		desc.pixelformat = DSPF_ARGB1666;
	} else if (pixelformat == JPF_ARGB6666) {
		desc.pixelformat = DSPF_ARGB6666;
	} else if (pixelformat == JPF_RGB18) {
		desc.pixelformat = DSPF_RGB18;
	} else if (pixelformat == JPF_LUT2) {
		desc.pixelformat = DSPF_LUT2;
	} else if (pixelformat == JPF_RGB444) {
		desc.pixelformat = DSPF_RGB444;
	} else if (pixelformat == JPF_RGB555) {
		desc.pixelformat = DSPF_RGB555;
	} else if (pixelformat == JPF_BGR555) {
		desc.pixelformat = DSPF_BGR555;
	} else if (pixelformat == JPF_RGBA5551) {
		desc.pixelformat = DSPF_RGBA5551;
	} else if (pixelformat == JPF_AVYU) {
		desc.pixelformat = DSPF_AVYU;
	} else if (pixelformat == JPF_VYU) {
		desc.pixelformat = DSPF_VYU;
	}

	DFBHandler *handler = dynamic_cast<DFBHandler *>(GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	if (engine->CreateSurface(engine, &desc, surface) != DFB_OK) {
		(*surface) = NULL;

		return false;
	}

	(*surface)->SetBlittingFlags((*surface), (DFBSurfaceBlittingFlags)(DSBLIT_NOFX));
	(*surface)->SetDrawingFlags((*surface), (DFBSurfaceDrawingFlags)(DSDRAW_NOFX));
	(*surface)->SetPorterDuff((*surface), (DFBSurfacePorterDuffRule)(DSPD_NONE));

	(*surface)->Clear((*surface), 0x00, 0x00, 0x00, 0x00);

	return true;
}

DFBImage::DFBImage(jpixelformat_t pixelformat, int width, int height):
	jgui::Image(pixelformat, width, height)
{
	jcommon::Object::SetClassName("jgui::DFBImage");

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

	IDirectFBSurface *surface = NULL;
	
	if (CreateSurface(&surface, pixelformat, width, height) == false) {
		throw jcommon::NullPointerException("Cannot create a native surface");
	}

	_graphics = new DFBGraphics(surface, pixelformat, width, height, false);

	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Add(this);
}

DFBImage::DFBImage(std::string file):
	jgui::Image(JPF_ARGB, -1, -1)
{
	jcommon::Object::SetClassName("jgui::DFBImage");

	_buffer = NULL;
	
	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	IDirectFBImageProvider *provider = NULL;
	DFBSurfaceDescription desc;

	if (engine->CreateImageProvider(engine, file.c_str(), &provider) != DFB_OK) {
		throw jcommon::RuntimeException("Cannot open this image type");
	}

	if (provider->GetSurfaceDescription(provider, &desc) != DFB_OK) {
		provider->Release(provider);

		throw jcommon::RuntimeException("Cannot get image description");
	}
	
	IDirectFBSurface *src = NULL;

	desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
	desc.caps = (DFBSurfaceCapabilities)(DSCAPS_SYSTEMONLY);
	desc.pixelformat = DSPF_ARGB;

	if (engine->CreateSurface(engine, &desc, &src) != DFB_OK) {
		provider->Release(provider);

		throw jcommon::RuntimeException("Cannot allocate memory to the image surface");
	}

	src->Clear(src, 0x00, 0x00, 0x00, 0x00);
	
	if (provider->RenderTo(provider, src, NULL) != DFB_OK) {
		provider->Release(provider);
		src->Release(src);

		throw jcommon::RuntimeException("Cannot blit image to the image surface");
	}

	provider->Release(provider);

	IDirectFBSurface *dst = NULL;

	if (CreateSurface(&dst, JPF_ARGB, desc.width, desc.height) == false) {
		src->Release(src);

		throw jcommon::RuntimeException("Cannot blit image to the image surface");
	}

	dst->SetPorterDuff(dst, (DFBSurfacePorterDuffRule)(DSPD_NONE));
	dst->SetBlittingFlags(dst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
	dst->Blit(dst, src, NULL, 0, 0);
	
	src->Release(src);

	_graphics = new DFBGraphics(dst, JPF_ARGB, desc.width, desc.height, false);

	_pixelformat = JPF_ARGB;
	_size.width = desc.width;
	_size.height = desc.height;

	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Add(this);
}

DFBImage::~DFBImage()
{
	dynamic_cast<DFBHandler *>(GFXHandler::GetInstance())->Remove(this);

	if (_buffer != NULL) {
		delete [] _buffer;
		_buffer = NULL;
	}

	if (_graphics != NULL) {
		IDirectFBSurface *surface = (IDirectFBSurface *)_graphics->GetNativeSurface();

		if (surface != NULL) {
			surface->Release(surface);
		}

		delete _graphics;
		_graphics = NULL;
	}
}

jsize_t DFBImage::GetImageSize(std::string img)
{
	jsize_t t;

	t.width = -1;
	t.height = -1;

	GFXHandler *handler = ((GFXHandler *)GFXHandler::GetInstance());
	IDirectFB *engine = (IDirectFB *)handler->GetGraphicEngine();

	IDirectFBImageProvider *provider = NULL;
	DFBSurfaceDescription desc;

	if (engine->CreateImageProvider(engine, img.c_str(), &provider) != DFB_OK) {
		return t;
	}

	if (provider->GetSurfaceDescription(provider, &desc) != DFB_OK) {
		provider->Release(provider);

		return t;
	}

	t.width = desc.width;
	t.height = desc.height;
		
	provider->Release(provider);

	return t;
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

	image = new DFBImage(JPF_ARGB, sdsc.width, sdsc.height);

	IDirectFBSurface *image_surface = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	image_surface->Blit(image_surface, surface, NULL, 0, 0);

	surface->Release(surface);
	provider->Release(provider);
	buffer->Release( buffer );

	return image;
}

void DFBImage::Release()
{
	_graphics->Lock();
	
	IDirectFBSurface *surface = (IDirectFBSurface *)_graphics->GetNativeSurface();

	if (_graphics != NULL) {
		_graphics->SetNativeSurface(NULL, 0, 0);
	}

	_graphics->Unlock();

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
		surface->Release(surface);
	}
}

void DFBImage::Restore()
{
	IDirectFBSurface *surface = NULL;

	CreateSurface(&surface, _pixelformat, _size.width, _size.height);

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
	
	_graphics->Lock();
	_graphics->SetNativeSurface(surface, width, height);
	_graphics->Unlock();
}

Image * DFBImage::Flip(Image *img, jflip_flags_t mode)
{
	jsize_t size = img->GetSize();

	DFBImage *image = new DFBImage(img->GetPixelFormat(), size.width, size.height);
	
	IDirectFBSurface *isrc = (IDirectFBSurface *)img->GetGraphics()->GetNativeSurface();
	IDirectFBSurface *idst = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	/*
	// INFO:: use matrix
	static const s32 x_mat[9] = {
		-DFB_FIXED_POINT, 0, (size.width-1) << 16,
		0, DFB_FIXED_POINT, 0,
		0, 0, DFB_FIXED_POINT
	};

	static const s32 y_mat[9] = {
		DFB_FIXED_POINT, 0, 0,
		0, -DFB_FIXED_POINT, (size.height-1) << 16,
		0, 0, DFB_FIXED_POINT
	};

	static const s32 xy_mat[9] = {
		-DFB_FIXED_POINT, 0, (size.width-1) << 16,
		0, -DFB_FIXED_POINT, (size.height-1) << 16,
		0, 0, DFB_FIXED_POINT
	};

	idst->SetRenderOptions(idst, (DFBSurfaceRenderOptions)(DSRO_MATRIX | DSRO_ANTIALIAS));

	if (t == JFF_HORIZONTAL) {
		idst->SetMatrix(idst, x_mat);
	} else if (t == JFF_VERTICAL) {
		idst->SetMatrix(idst, y_mat);
	} else {
		idst->SetMatrix(idst, xy_mat);
	}

	idst->Blit(idst, isrc, NULL, 0, 0);
	
	idst->SetRenderOptions(idst, (DFBSurfaceRenderOptions)(DSRO_NONE));
	*/

	// INFO:: use blitting flags
	if (mode == JFF_HORIZONTAL) {
		idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_FLIP_HORIZONTAL));
	} else if (mode == JFF_VERTICAL) {
		idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_FLIP_VERTICAL));
	} else {
		idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_FLIP_HORIZONTAL | DSBLIT_FLIP_VERTICAL));
	}

	idst->Blit(idst, isrc, NULL, 0, 0);
	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));

	return image;
}

Image * DFBImage::Rotate(Image *img, double radians, bool resize)
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

	DFBImage *rotate = new DFBImage(ipixel, iw, ih);

	IDirectFBSurface *isrc = (IDirectFBSurface *)img->GetGraphics()->GetNativeSurface();
	IDirectFBSurface *idst = (IDirectFBSurface *)rotate->GetGraphics()->GetNativeSurface();

	/*
	// INFO:: use matrix
	int sw, sh;
	int dw, dh;

	isrc->GetSize(isrc, &sw, &sh);
	idst->GetSize(idst, &dw, &dh);

	static const s32 r_mat[9] = {
		(s32)(DFB_FIXED_POINT*cos(angle)), (s32)(DFB_FIXED_POINT*sin(angle)), (s32)(DFB_FIXED_POINT*(dw/2)),
		(s32)(-DFB_FIXED_POINT*sin(angle)), (s32)(DFB_FIXED_POINT*cos(angle)), (s32)(DFB_FIXED_POINT*(dh/2)),
		0, 0, DFB_FIXED_POINT
	};

	idst->SetRenderOptions(idst, (DFBSurfaceRenderOptions)(DSRO_MATRIX | DSRO_ANTIALIAS));
	idst->SetMatrix(idst, r_mat);

	idst->Blit(idst, isrc, NULL, -sw/2, -sh/2);
	
	idst->SetRenderOptions(idst, (DFBSurfaceRenderOptions)(DSRO_NONE));
	*/

	// INFO:: use algebra
	void *sptr;
	// uint32_t *sptr32;
	void *dptr;
	uint32_t *dptr32;
	int spitch;
	int dpitch;
	int sw,
			sh;
	int dw,
			dh;

	isrc->GetSize(isrc, &sw, &sh);
	idst->GetSize(idst, &dw, &dh);

	isrc->Lock(isrc, (DFBSurfaceLockFlags)(DSLF_READ), &sptr, &spitch);
	idst->Lock(idst, (DFBSurfaceLockFlags)(DSLF_WRITE), &dptr, &dpitch);

	int sxc = sw/2;
	int syc = sh/2;
	int dxc = dw/2;
	int dyc = dh/2;
	int xo;
	int yo;
	int t1;
	int t2;

	// sptr32 = (uint32_t *)sptr;

	for (int j=0; j<dh; j++) {
		dptr32 = (uint32_t *)((uint8_t *)dptr + j*dpitch);
		t1 = (j-dyc)*sinTheta;
		t2 = (j-dyc)*cosTheta;

		for (int i=0; i<dw; i++) {
			xo = ((i-dxc)*cosTheta - t1)/precision;
			yo = ((i-dxc)*sinTheta + t2)/precision;

			if (xo >= -sxc && xo < sxc && yo >= -syc && yo < syc) {
				*(dptr32+i) = *((uint32_t *)((uint8_t *)sptr + (yo+syc)*spitch) + (xo+sxc));
			}
		}
	}

	isrc->Unlock(isrc);
	idst->Unlock(idst);

	return rotate;
}

Image * DFBImage::Scale(Image *img, int width, int height)
{
	if (img == NULL) {
		return NULL;
	}

	DFBImage *image = new DFBImage(img->GetPixelFormat(), width, height);
	
	IDirectFBSurface *isrc = (IDirectFBSurface *)img->GetGraphics()->GetNativeSurface();
	IDirectFBSurface *idst = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	DFBRectangle src, dst;

	src.x = 0;
	src.y = 0;
	src.w = img->GetWidth();
	src.h = img->GetHeight();

	dst.x = 0;
	dst.y = 0;
	dst.w = width;
	dst.h = height;

	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
	idst->StretchBlit(idst, isrc, &src, &dst);

	return image;
}

Image * DFBImage::Crop(Image *img, int x, int y, int width, int height)
{
	if (img == NULL) {
		return NULL;
	}

	DFBImage *image = new DFBImage(img->GetPixelFormat(), width, height);

	IDirectFBSurface *isrc = (IDirectFBSurface *)img->GetGraphics()->GetNativeSurface();
	IDirectFBSurface *idst = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	DFBRectangle src, dst;

	src.x = x;
	src.y = y;
	src.w = width;
	src.h = height;

	dst.x = 0;
	dst.y = 0;
	dst.w = width;
	dst.h = height;

	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
	idst->StretchBlit(idst, isrc, &src, &dst);

	return image;
}

Image * DFBImage::Blend(Image *img, double alpha)
{
	DFBImage *image = new DFBImage(img->GetPixelFormat(), img->GetWidth(), img->GetHeight());
	
	IDirectFBSurface *isrc = (IDirectFBSurface *)img->GetGraphics()->GetNativeSurface();
	IDirectFBSurface *idst = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	idst->SetColor(idst, 0x00, 0x00, 0x00, (int)(alpha*255.0));
	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_BLEND_COLORALPHA));
	idst->Blit(idst, isrc, NULL, 0, 0);
	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));

	return image;
}

Image * DFBImage::Colorize(Image *img, Color color)
{
	DFBImage *image = new DFBImage(img->GetPixelFormat(), img->GetWidth(), img->GetHeight());
	
	IDirectFBSurface *isrc = (IDirectFBSurface *)img->GetGraphics()->GetNativeSurface();
	IDirectFBSurface *idst = (IDirectFBSurface *)image->GetGraphics()->GetNativeSurface();

	idst->SetColor(idst, color.GetRed(), color.GetGreen(), color.GetBlue(), color.GetAlpha());
	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL | DSBLIT_COLORIZE));
	idst->Blit(idst, isrc, NULL, 0, 0);
	idst->SetBlittingFlags(idst, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));

	return image;
}

void DFBImage::SetPixels(uint8_t *buffer, int xp, int yp, int wp, int hp, int stride)
{
	if (buffer == NULL) {
		return;
	}

	IDirectFBSurface *surface = (IDirectFBSurface *)GetGraphics()->GetNativeSurface();

	if (surface == NULL) {
		return;
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	xp = (xp*pitch)/GetWidth();
	wp = (wp*pitch)/GetWidth();

	for (int j=0; j<hp; j++) {
		uint8_t *src = (uint8_t *)(buffer + j * pitch);
		uint8_t *dst = (uint8_t *)((uint8_t *)ptr + (j + yp) * pitch + xp);

		for (int i=0; i<wp; i++) {
			*(dst + i) = *(src + i);
		}
	}

	surface->Unlock(surface);
}

void DFBImage::GetPixels(uint8_t **buffer, int xp, int yp, int wp, int hp, int *stride)
{
	IDirectFBSurface *surface = (IDirectFBSurface *)GetGraphics()->GetNativeSurface();

	if (surface == NULL) {
		return;
	}

	void *ptr;
	int pitch;

	surface->Lock(surface, DSLF_WRITE, &ptr, &pitch);

	if (*buffer == NULL) {
		(*buffer) = new uint8_t[pitch*hp];
	}

	xp = (xp*pitch)/GetWidth();
	wp = (wp*pitch)/GetWidth();

	for (int j=0; j<hp; j++) {
		uint8_t *src = (uint8_t *)((uint8_t *)ptr + (j + yp) * pitch + xp);
		uint8_t *dst = (uint8_t *)(*buffer + j * pitch);

		for (int i=0; i<wp; i++) {
			*(dst + i) = *(src + i);
		}
	}

	surface->Unlock(surface);

	(*stride) = pitch;
}

}

