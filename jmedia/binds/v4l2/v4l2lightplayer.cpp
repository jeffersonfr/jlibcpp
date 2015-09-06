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
#include "v4l2lightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jmediaexception.h"
#include "jcolorconversion.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

namespace jmedia {

class V4LComponentImpl : public jgui::Component, jthread::Thread {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		jgui::Image *_image;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		jgui::jregion_t _src;
		/** \brief */
		uint32_t *_buffer;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		V4LComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = NULL;
			_image = NULL;
			_player = player;
			
			_frame_size.width = w;
			_frame_size.height = h;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			SetVisible(true);
		}

		virtual ~V4LComponentImpl()
		{
			if (IsRunning() == true) {
				WaitThread();
			}

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_mutex.Unlock();

			if (_buffer != NULL) {
				delete [] _buffer;
				_buffer = NULL;
			}
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(const uint8_t *buffer, int width, int height, jgui::jpixelformat_t format)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			if (_buffer == NULL) {
				_frame_size.width = _src.width = width;
				_frame_size.height = _src.height = height;

				_buffer = new uint32_t[width*height];
			}

			if (format == jgui::JPF_UYVY) {
				ColorConversion::GetRGB32FromYUYV((uint8_t **)&buffer, (uint32_t **)&_buffer, width, height);
			} else if (format == jgui::JPF_RGB24) {
				ColorConversion::GetRGB32FromRGB24((uint8_t **)&buffer, (uint32_t **)&_buffer, width, height);
			} else if (format == jgui::JPF_RGB32) {
				memcpy(_buffer, buffer, width*height*4);
			}

#if defined(DIRECTFB_NODEPS_UI)
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = width;
			int sh = height;

			IDirectFBSurface *frame;
			DFBSurfaceDescription desc;

			desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_PREALLOCATED);
			desc.caps = (DFBSurfaceCapabilities)(DSCAPS_NONE);
			desc.width = sw;
			desc.height = sh;
			desc.pixelformat = DSPF_ARGB;
			desc.preallocated[0].data = _buffer;
			desc.preallocated[0].pitch = sw*4;

			IDirectFB *directfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

			if (directfb->CreateSurface(directfb, &desc, &frame) == DFB_OK) {
				_mutex.Lock();

				if (_image != NULL) {
					delete _image;
					_image = NULL;
				}

				_image = new jgui::NativeImage(frame, jgui::JPF_ARGB, sw, sh);

				_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, _image));

				_mutex.Unlock();

				Start();
			}
#else
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = width;
			int sh = height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
			cairo_t *cairo_context = cairo_create(cairo_surface);

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_image = new jgui::NativeImage(cairo_context, jgui::JPF_RGB24, sw, sh);

			_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, _image));

			cairo_surface_flush(cairo_surface);
			cairo_surface_destroy(cairo_surface);

			_mutex.Unlock();

			Start();
#endif
		}

		virtual void Run()
		{
			if (IsVisible() != false) {
				Repaint();
			}
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

			_mutex.Lock();

			g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, _location.x, _location.y, _size.width, _size.height);
				
			_mutex.Unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		V4L2LightPlayer *_player;

	public:
		VideoSizeControlImpl(V4L2LightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			V4LComponentImpl *impl = dynamic_cast<V4LComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			V4LComponentImpl *impl = dynamic_cast<V4LComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<V4LComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			V4LComponentImpl *impl = dynamic_cast<V4LComponentImpl *>(_player->_component);

			jgui::jregion_t t;

			t.x = impl->GetX();
			t.y = impl->GetY();
			t.width = impl->GetWidth();
			t.height = impl->GetHeight();

			return t;
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		V4L2LightPlayer *_player;
		jaspect_ratio_t _aspect_ratio;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(V4L2LightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_aspect_ratio = LAR_16x9;
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~VideoFormatControlImpl()
		{
		}

		virtual void SetAspectRatio(jaspect_ratio_t t)
		{
			_aspect_ratio = t;
		}

		virtual void SetContentMode(jvideo_mode_t t)
		{
			_video_mode = t;
		}

		virtual void SetVideoFormatHD(jhd_video_format_t vf)
		{
			_hd_video_format = vf;
		}

		virtual void SetVideoFormatSD(jsd_video_format_t vf)
		{
			_sd_video_format = vf;
		}

		virtual void SetContrast(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(CONTRAST_CONTROL) == true) {
					control->SetValue(CONTRAST_CONTROL, value);
				}
			}
		}

		virtual void SetSaturation(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(SATURATION_CONTROL) == true) {
					control->SetValue(SATURATION_CONTROL, value);
				}
			}
		}

		virtual void SetHUE(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(HUE_CONTROL) == true) {
					control->SetValue(HUE_CONTROL, value);
				}
			}
		}

		virtual void SetBrightness(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(BRIGHTNESS_CONTROL) == true) {
					control->SetValue(BRIGHTNESS_CONTROL, value);
				}
			}
		}

		virtual void SetSharpness(int value)
		{
		}

		virtual void SetGamma(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(GAMMA_CONTROL) == true) {
					control->SetValue(GAMMA_CONTROL, value);
				}
			}
		}

		virtual jaspect_ratio_t GetAspectRatio()
		{
			jthread::AutoLock lock(&_player->_mutex);

			double aspect = _player->_aspect;

			if (aspect == (1.0/1.0)) {
				return LAR_1x1;
			} else if (aspect == (3.0/2.0)) {
				return LAR_3x2;
			} else if (aspect == (4.0/3.0)) {
				return LAR_4x3;
			} else if (aspect == (16.0/9.0)) {
				return LAR_16x9;
			}

			return LAR_16x9;
		}

		virtual jvideo_mode_t GetContentMode()
		{
			return LVM_FULL;
		}

		virtual jhd_video_format_t GetVideoFormatHD()
		{
			return LHVF_1080p;
		}

		virtual jsd_video_format_t GetVideoFormatSD()
		{
			return LSVF_PAL_M;
		}

		virtual int GetContrast()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(CONTRAST_CONTROL) == true) {
					return control->GetValue(CONTRAST_CONTROL);
				}
			}

			return 0;
		}

		virtual int GetSaturation()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(SATURATION_CONTROL) == true) {
					return control->GetValue(SATURATION_CONTROL);
				}
			}

			return 0;
		}

		virtual int GetHUE()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(HUE_CONTROL) == true) {
					return control->GetValue(HUE_CONTROL);
				}
			}

			return 0;
		}

		virtual int GetBrightness()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(BRIGHTNESS_CONTROL) == true) {
					return control->GetValue(BRIGHTNESS_CONTROL);
				}
			}

			return 0;
		}

		virtual int GetSharpness()
		{
			return 0;
		}

		virtual int GetGamma()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_grabber != NULL) {
				VideoControl *control = _player->_grabber->GetVideoControl();

				if (control->HasControl(GAMMA_CONTROL) == true) {
					return control->GetValue(GAMMA_CONTROL);
				}
			}

			return 0;
		}

};

V4L2LightPlayer::V4L2LightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = true;
	_aspect = 1.0;
	_media_time = 0LL;
	_is_loop = false;
	_decode_rate = 1.0;
	_frame_rate = 0.0;
	_component = NULL;
	
  _grabber = new VideoGrabber(this, file);

	jgui::jsize_t size;
	
	size.width = 720;
	size.height = 480;

	_grabber->Open();
	_grabber->Configure(size.width, size.height);
	_grabber->GetVideoControl()->Reset();

	_controls.push_back(new VideoSizeControlImpl(this));
	_controls.push_back(new VideoFormatControlImpl(this));
	
	_component = new V4LComponentImpl(this, 0, 0, -1, -1);
}

V4L2LightPlayer::~V4L2LightPlayer()
{
	Close();
	
	delete _component;
	_component = NULL;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();
}

void V4L2LightPlayer::ProcessFrame(const uint8_t *buffer, int width, int height, jgui::jpixelformat_t format)
{
	dynamic_cast<V4LComponentImpl *>(_component)->UpdateComponent(buffer, width, height, format);
}

void V4L2LightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _grabber != NULL) {
		_grabber->Start();
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_STARTED));
	}
}

void V4L2LightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _grabber != NULL) {
		_is_paused = true;
		
		_grabber->Pause();
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void V4L2LightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true && _grabber != NULL) {
		_is_paused = false;
		
		_grabber->Resume();
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void V4L2LightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_grabber != NULL) {
		_grabber->Stop();

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void V4L2LightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_grabber != NULL) {
		delete _grabber;
		_grabber = NULL;
	}
}

void V4L2LightPlayer::SetCurrentTime(uint64_t time)
{
}

uint64_t V4L2LightPlayer::GetCurrentTime()
{
	return 0LL;
}

uint64_t V4L2LightPlayer::GetMediaTime()
{
	return 0LL;
}

void V4L2LightPlayer::SetLoop(bool b)
{
}

bool V4L2LightPlayer::IsLoop()
{
	return false;
}

void V4L2LightPlayer::SetDecodeRate(double rate)
{
}

double V4L2LightPlayer::GetDecodeRate()
{
	return 0.0;
}

jgui::Component * V4L2LightPlayer::GetVisualComponent()
{
	return _component;
}

}

