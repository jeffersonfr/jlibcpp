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
#include "libavlightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvolumecontrol.h"
#include "jmediaexception.h"
#include "jgfxhandler.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

namespace jmedia {

namespace libavlightplayer {

class PlayerComponentImpl : public jgui::Component, jthread::Thread {

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
		jgui::jregion_t _dst;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		PlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_image = NULL;
			_player = player;
			
			_frame_size.width = w;
			_frame_size.height = h;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			_dst.x = 0;
			_dst.y = 0;
			_dst.width = w;
			_dst.height = h;

			SetVisible(true);
		}

		virtual ~PlayerComponentImpl()
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
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent(uint8_t *buffer, int width, int height)
		{
			if (width <= 0 || height <= 0) {
				return;
			}

			if (_src.width <= 0 || _src.height <= 0) {
				dynamic_cast<LibAVLightPlayer *>(_player)->_aspect = (double)width/(double)height;

				_frame_size.width = _src.width = width;
				_frame_size.height = _src.height = height;
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
			desc.preallocated[0].data = buffer;
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

				Run();
			}
#else
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = width;
			int sh = height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)buffer, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
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

			Run();
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

			g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, 0, 0, _size.width, _size.height);
				
			_mutex.Unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class VolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibAVLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(LibAVLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;
		}

		virtual ~VolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			jthread::AutoLock lock(&_player->_mutex);

			int level = 0;

			/*
			if (_player->_provider != NULL) {
				level = libvlc_audio_get_volume(_player->_provider);
			}
			*/

			return level;
		}

		virtual void SetLevel(int level)
		{
			jthread::AutoLock lock(&_player->_mutex);

			_level = (level < 0)?0:(level > 100)?100:level;

			if (_level != 0) {
				_is_muted = true;
			} else {
				_is_muted = false;
			}

			/*
			if (_player->_provider != NULL) {
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
				libvlc_audio_set_volume(_player->_provider, _level);
			}
			*/
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			_is_muted = b;
			
			if (_player->_provider != NULL) {
				avplay_mute(_player->_provider, _is_muted);
			}
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibAVLightPlayer *_player;

	public:
		VideoSizeControlImpl(LibAVLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			PlayerComponentImpl *impl = dynamic_cast<PlayerComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			PlayerComponentImpl *impl = dynamic_cast<PlayerComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<PlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<PlayerComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibAVLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(LibAVLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~VideoFormatControlImpl()
		{
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

		virtual double GetFramesPerSecond()
		{
			jthread::AutoLock lock(&_player->_mutex);

			return _player->_provider->frames_per_second;
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

};

static void render_callback(void *data, uint8_t *buffer, int width, int height)
{
	reinterpret_cast<PlayerComponentImpl *>(data)->UpdateComponent(buffer, width, height);
}

static void endofmedia_callback(void *data)
{
	LibAVLightPlayer *player = reinterpret_cast<LibAVLightPlayer *>(data);
	
	player->DispatchPlayerEvent(new PlayerEvent(player, JPE_FINISHED));
}

}

LibAVLightPlayer::LibAVLightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	
	avplay_init();

	_provider = avplay_open(_file.c_str());

	if (_provider == NULL) {
		throw MediaException("Cannot recognize the media file");
	}

	_component = new libavlightplayer::PlayerComponentImpl(this, 0, 0, -1, -1);//iw, ih);

	avplay_set_rendercallback(_provider, libavlightplayer::render_callback, (void *)_component);
	avplay_set_endofmediacallback(_provider, libavlightplayer::endofmedia_callback, (void *)this);
		
	if (_provider->has_audio == true) {
		_controls.push_back(new libavlightplayer::VolumeControlImpl(this));
	}
	
	if (_provider->has_video == true) {
		_controls.push_back(new libavlightplayer::VideoSizeControlImpl(this));
		_controls.push_back(new libavlightplayer::VideoFormatControlImpl(this));
	}
}

LibAVLightPlayer::~LibAVLightPlayer()
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

void LibAVLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		avplay_play(_provider);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_STARTED));
	}
}

void LibAVLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		_is_paused = true;
		
		avplay_pause(_provider, true);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void LibAVLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true && _provider != NULL) {
		_is_paused = false;
		
		avplay_pause(_provider, false);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void LibAVLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		avplay_stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibAVLightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_provider != NULL) {
		avplay_close(_provider);

		_provider = NULL;
	}
}

void LibAVLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		avplay_setcurrentmediatime(_provider, time);
	}
}

uint64_t LibAVLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	uint64_t time = 0LL;

	if (_provider != NULL) {
		time = (uint64_t)avplay_getcurrentmediatime(_provider);
	}

	return time;
}

uint64_t LibAVLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;

	if (_provider != NULL) {
		time = (uint64_t)avplay_getmediatime(_provider);
	}

	return time;
}

void LibAVLightPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		avplay_setloop(_provider, b);
	}
}

bool LibAVLightPlayer::IsLoop()
{
	if (_provider != NULL) {
		return avplay_isloop(_provider);
	}

	return false;
}

void LibAVLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != NULL) {
		// libvlc_media_player_set_rate(_provider, (float)rate);
	}
}

double LibAVLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		// rate = (double)libvlc_media_player_get_rate(_provider);
	}

	return rate;
}

jgui::Component * LibAVLightPlayer::GetVisualComponent()
{
	return _component;
}

}