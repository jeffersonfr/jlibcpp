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
#include "nativelightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvolumecontrol.h"
#include "jmediaexception.h"
#include "jimage.h"
#include "jgfxhandler.h"

#include <cairo.h>

namespace jmedia {

class VideoLightweightImpl : public jgui::Component, jthread::Thread {

	public:
		/** \brief */
		IDirectFBSurface *_surface;
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
		bool _diff;

	public:
		VideoLightweightImpl(Player *player, int x, int y, int w, int h, int iw, int ih):
			jgui::Component(x, y, w, h)
		{
			IDirectFB *engine = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

			DFBSurfaceDescription desc;

			desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
			desc.pixelformat = DSPF_ARGB;
			desc.width = w;
			desc.height = h;

			if (engine->CreateSurface(engine, &desc, &_surface) != DFB_OK) {
				throw jcommon::RuntimeException("Cannot allocate memory to the image surface");
			}

			_surface->SetPorterDuff(_surface, DSPD_NONE);
			_surface->SetBlittingFlags(_surface, (DFBSurfaceBlittingFlags)(DSBLIT_BLEND_ALPHACHANNEL));
			_surface->Clear(_surface, 0x00, 0x00, 0x00, 0x00);

			_image = NULL;

			_player = player;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			_dst.x = 0;
			_dst.y = 0;
			_dst.width = w;
			_dst.height = h;

			_diff = false;

			SetVisible(true);
		}

		virtual ~VideoLightweightImpl()
		{
			if (IsRunning() == true) {
				WaitThread();
			}

			_mutex.Lock();

			if (_image != NULL) {
				delete _image;
				_image = NULL;
			}

			_surface->Release(_surface);
			_surface = NULL;

			_mutex.Unlock();
		}

		virtual void UpdateComponent()
		{
			if (IsRunning() == true) {
				WaitThread();
			}
	
			if (_surface != NULL) {
				void *ptr;
				int pitch;
				int sw,
						sh;

				_surface->GetSize(_surface, &sw, &sh);
				_surface->Lock(_surface, (DFBSurfaceLockFlags)(DSLF_WRITE), &ptr, &pitch);

				cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
						(uint8_t *)ptr, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
				cairo_t *cairo_context = cairo_create(cairo_surface);

				_mutex.Lock();

				if (_image != NULL) {
					delete _image;
					_image = NULL;
				}

				_image = new jgui::NativeImage(cairo_context, jgui::JPF_ARGB, sw, sh);

				_player->DispatchFrameGrabberEvent(new FrameGrabberEvent(_player, JFE_GRABBED, _image));

				cairo_surface_flush(cairo_surface);
				cairo_surface_destroy(cairo_surface);
			
				_mutex.Unlock();

				_surface->Unlock(_surface);
				
			}

			Run();
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

			if (_diff == false) {
				g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
			} else {
				g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, _dst.x, _dst.y, _dst.width, _dst.height);
			}
				
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
		NativeLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(NativeLightPlayer *player):
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

			float level = 0.0f;

			if (_player->_provider != NULL) {
				_player->_provider->GetVolume(_player->_provider, &level);
			}

			return (int)(level*100.0f);
		}

		virtual void SetLevel(int level)
		{
			jthread::AutoLock lock(&_player->_mutex);

			_level = level;

			if (_level <= 0) {
				_level = 0;
				_is_muted = true;
			} else {
				if (_level > 100) {
					_level = 100;
				}

				_is_muted = false;
			}

			if (_player->_provider != NULL) {
				_player->_provider->SetVolume(_player->_provider, _level/100.0f);
			}
		}
		
		virtual bool IsMute()
		{
			return _is_muted;
		}

		virtual void SetMute(bool b)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_is_muted == b) {
				return;
			}

			if (_is_muted == false) {
				float level = _level/100.0f;

				if (_player->_provider != NULL) {
					_player->_provider->SetVolume(_player->_provider, level);
				}
			} else {
				if (_player->_provider != NULL) {
					_player->_provider->SetVolume(_player->_provider, 0.0f);
				}
			}
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		NativeLightPlayer *_player;

	public:
		VideoSizeControlImpl(NativeLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			VideoLightweightImpl *impl = dynamic_cast<VideoLightweightImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
			
			impl->_diff = false;

			if (impl->_src.x != impl->_dst.x ||
					impl->_src.y != impl->_dst.y ||
					impl->_src.width != impl->_dst.width ||
					impl->_src.height != impl->_dst.height) {
				impl->_diff = true;
			}
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			VideoLightweightImpl *impl = dynamic_cast<VideoLightweightImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->_dst.x = x;
			impl->_dst.y = y;
			impl->_dst.width = w;
			impl->_dst.height = h;
			
			impl->_diff = false;

			if (impl->_src.x != impl->_dst.x ||
					impl->_src.y != impl->_dst.y ||
					impl->_src.width != impl->_dst.width ||
					impl->_src.height != impl->_dst.height) {
				impl->_diff = true;
			}
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<VideoLightweightImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<VideoLightweightImpl *>(_player->_component)->_dst;
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		NativeLightPlayer *_player;
		jaspect_ratio_t _aspect_ratio;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(NativeLightPlayer *player):
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

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_CONTRAST);
				adj.contrast = value;

				_player->_provider->SetColorAdjustment(_player->_provider, &adj);
			}
		}

		virtual void SetSaturation(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_SATURATION);
				adj.saturation = value;

				_player->_provider->SetColorAdjustment(_player->_provider, &adj);
			}
		}

		virtual void SetHUE(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_HUE);
				adj.hue = value;

				_player->_provider->SetColorAdjustment(_player->_provider, &adj);
			}
		}

		virtual void SetBrightness(int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_BRIGHTNESS);
				adj.brightness = value;

				_player->_provider->SetColorAdjustment(_player->_provider, &adj);
			}
		}

		virtual void SetSharpness(int value)
		{
			// TODO::
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

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_CONTRAST);

				_player->_provider->GetColorAdjustment(_player->_provider, &adj);

				return adj.contrast;
			}

			return 0;
		}

		virtual int GetSaturation()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_SATURATION);

				_player->_provider->GetColorAdjustment(_player->_provider, &adj);

				return adj.saturation;
			}

			return 0;
		}

		virtual int GetHUE()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_HUE);

				_player->_provider->GetColorAdjustment(_player->_provider, &adj);

				return adj.hue;
			}

			return 0;
		}

		virtual int GetBrightness()
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				DFBColorAdjustment adj;

				adj.flags = (DFBColorAdjustmentFlags)(DCAF_BRIGHTNESS);

				_player->_provider->GetColorAdjustment(_player->_provider, &adj);

				return adj.brightness;
			}

			return 0;
		}

		virtual int GetSharpness()
		{
			return 0;
		}

};

NativeLightPlayer::NativeLightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_decode_rate = 1.0;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_component = NULL;

	IDirectFB *directfb = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

	if (directfb->CreateVideoProvider(directfb, _file.c_str(), &_provider) != DFB_OK) {
		_provider = NULL;

		throw jmedia::MediaException("Media format not supported");
	}

	DFBSurfaceDescription sdsc;
	DFBStreamDescription mdsc;

	_provider->SetPlaybackFlags(_provider, DVPLAY_NOFX);
	_provider->GetSurfaceDescription(_provider, &sdsc);
	_provider->GetStreamDescription(_provider, &mdsc);
	_provider->CreateEventBuffer(_provider, &_events);

	_aspect = 16.0/9.0;

	_media_info.title = std::string(mdsc.title);
	_media_info.author = std::string(mdsc.author);
	_media_info.album = std::string(mdsc.album);
	_media_info.genre = std::string(mdsc.genre);
	_media_info.comments = std::string(mdsc.comment);
	_media_info.year = mdsc.year;

	if (mdsc.caps & DVSCAPS_AUDIO) {
		_has_audio = true;

		_controls.push_back(new VolumeControlImpl(this));
	}

	if (mdsc.caps & DVSCAPS_VIDEO) {
		_has_video = true;
		_aspect = mdsc.video.aspect;

		_controls.push_back(new VideoSizeControlImpl(this));
		_controls.push_back(new VideoFormatControlImpl(this));
	}

	_component = new VideoLightweightImpl(this, 0, 0, sdsc.width, sdsc.height, sdsc.width, sdsc.height);

	Start();
}

NativeLightPlayer::~NativeLightPlayer()
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

void NativeLightPlayer::Callback(void *ctx)
{
	reinterpret_cast<VideoLightweightImpl *>(ctx)->UpdateComponent();
}
		
void NativeLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		IDirectFBSurface *surface = dynamic_cast<VideoLightweightImpl *>(_component)->_surface;

		if (_has_video == true) {
			_provider->PlayTo(_provider, surface, NULL, NativeLightPlayer::Callback, (void *)_component);
		} else {
			_provider->PlayTo(_provider, surface, NULL, NULL, NULL);
		}

		// usleep(500000);
	}
}

void NativeLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void NativeLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;

		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void NativeLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		_provider->Stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void NativeLightPlayer::Close()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	WaitThread();

	if (_provider != NULL) {
		// INFO:: cause crashes when release and create players
		//_events->Release(_events);
		_events = NULL;

		_provider->Release(_provider);
		_provider = NULL;
	}
}

void NativeLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		_provider->SeekTo(_provider, (double)time/1000.0);
	}
}

uint64_t NativeLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetPos(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

uint64_t NativeLightPlayer::GetMediaTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetLength(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

void NativeLightPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;

	if (_provider != NULL) {
		if (_is_loop == false) {
			_provider->SetPlaybackFlags(_provider, DVPLAY_NOFX);
		} else {
			_provider->SetPlaybackFlags(_provider, DVPLAY_LOOPING);
		}
	}
}

bool NativeLightPlayer::IsLoop()
{
	return _is_loop;
}

void NativeLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	if (rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != NULL) {
		_provider->SetSpeed(_provider, rate);
	}
}

double NativeLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		_provider->GetSpeed(_provider, &rate);
	}

	return rate;
}

jgui::Component * NativeLightPlayer::GetVisualComponent()
{
	return _component;
}

void NativeLightPlayer::Run()
{
	while (_is_closed == false) {
		_events->WaitForEventWithTimeout(_events, 0, 100);

		while (_events->HasEvent(_events) == DFB_OK) {
			DFBVideoProviderEvent event;

			_events->GetEvent(_events, DFB_EVENT(&event));

			if (event.clazz == DFEC_VIDEOPROVIDER) {
				// TODO:: disparar eventos de midia
				if (event.type == DVPET_STARTED) {
					DispatchPlayerEvent(new PlayerEvent(this, JPE_STARTED));
				} else if (event.type == DVPET_STOPPED) {
					DispatchPlayerEvent(new PlayerEvent(this, JPE_STOPPED));
				} else if (event.type == DVPET_FINISHED) {
					DispatchPlayerEvent(new PlayerEvent(this, JPE_FINISHED));
				}
			}
		}
	}
}

}
