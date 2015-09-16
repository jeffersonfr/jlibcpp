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
#include "directfblightplayer.h"
#include "nativeimage.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvideodevicecontrol.h"
#include "jaudioconfigurationcontrol.h"
#include "jvolumecontrol.h"
#include "jmediaexception.h"
#include "jimage.h"
#include "jgfxhandler.h"

#if defined(DIRECTFB_NODEPS_UI)
#include <directfb.h>
#else
#include <cairo.h>
#endif

namespace jmedia {

namespace directfblightplayer {

class DirectFBLightComponentImpl : public jgui::Component, jthread::Thread {

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
		jgui::jsize_t _frame_size;

	public:
		DirectFBLightComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			IDirectFB *engine = (IDirectFB *)jgui::GFXHandler::GetInstance()->GetGraphicEngine();

			DFBSurfaceDescription desc;

			desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_CAPS |  DSDESC_PIXELFORMAT);
			desc.caps = (DFBSurfaceCapabilities)DSCAPS_DOUBLE;
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

			_frame_size.width = w;
			_frame_size.height = h;

			_src.x = 0;
			_src.y = 0;
			_src.width = w;
			_src.height = h;

			SetVisible(true);
		}

		virtual ~DirectFBLightComponentImpl()
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

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent()
		{
#if defined(DIRECTFB_NODEPS_UI)
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

				IDirectFBSurface *frame;
				DFBSurfaceDescription desc;

				desc.flags = (DFBSurfaceDescriptionFlags)(DSDESC_CAPS | DSDESC_WIDTH | DSDESC_HEIGHT | DSDESC_PIXELFORMAT | DSDESC_PREALLOCATED);
				desc.caps = (DFBSurfaceCapabilities)(DSCAPS_NONE);
				desc.width = sw;
				desc.height = sh;
				desc.pixelformat = DSPF_ARGB;
				desc.preallocated[0].data = ptr;
				desc.preallocated[0].pitch = pitch;

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
				
					_surface->Unlock(_surface);
					_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)0);
				} else {
					_surface->Unlock(_surface);
				}
			}
					
			Start();
#else
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
				_surface->Flip(_surface, NULL, (DFBSurfaceFlipFlags)0);
				
			}

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
		DirectFBLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(DirectFBLightPlayer *player):
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

class AudioConfigurationControlImpl : public AudioConfigurationControl {
	
	private:
		/** \brief */
		DirectFBLightPlayer *_player;
		/** \brief */
		int64_t _audio_delay;

	public:
		AudioConfigurationControlImpl(DirectFBLightPlayer *player):
			AudioConfigurationControl()
		{
			_player = player;
			_audio_delay = 0LL;
		}

		virtual ~AudioConfigurationControlImpl()
		{
		}

		virtual void SetAudioDelay(int64_t delay)
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			if (_player->_provider != NULL) {
				if (_player->_provider->SetAudioDelay(_player->_provider, delay) == DFB_OK) {
					_audio_delay = delay;
				}
			}
		}

		virtual int64_t GetAudioDelay()
		{
			jthread::AutoLock lock(&_player->_mutex);
	
			return _audio_delay;
		}

};

class VideoSizeControlImpl : public VideoSizeControl {
	
	private:
		DirectFBLightPlayer *_player;

	public:
		VideoSizeControlImpl(DirectFBLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			DirectFBLightComponentImpl *impl = dynamic_cast<DirectFBLightComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			DirectFBLightComponentImpl *impl = dynamic_cast<DirectFBLightComponentImpl *>(_player->_component);

			jthread::AutoLock lock(&impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<DirectFBLightComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<DirectFBLightComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		DirectFBLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		VideoFormatControlImpl(DirectFBLightPlayer *player):
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

			return _player->_frames_per_second;
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

class VideoDeviceControlImpl : public VideoDeviceControl {
	
	private:
		DirectFBLightPlayer *_player;
		std::map<jvideo_control_t, int> _default_values;

	public:
		VideoDeviceControlImpl(DirectFBLightPlayer *player):
			VideoDeviceControl()
		{
			_player = player;
			
			_controls.push_back(JVC_CONTRAST);
			_controls.push_back(JVC_SATURATION);
			_controls.push_back(JVC_HUE);
			_controls.push_back(JVC_BRIGHTNESS);

			for (std::vector<jvideo_control_t>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
				_default_values[*i] = GetValue(*i);
			}
		}

		virtual ~VideoDeviceControlImpl()
		{
		}

		virtual int GetValue(jvideo_control_t id)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				if (HasControl(id) == true) {
					DFBColorAdjustment adj;

					if (id == JVC_CONTRAST) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_CONTRAST);
					} else if (id == JVC_SATURATION) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_SATURATION);
					} else if (id == JVC_HUE) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_HUE);
					} else if (id == JVC_BRIGHTNESS) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_BRIGHTNESS);
					}
					
					_player->_provider->GetColorAdjustment(_player->_provider, &adj);

					if (id == JVC_CONTRAST) {
						return adj.contrast;
					} else if (id == JVC_SATURATION) {
						return adj.saturation;
					} else if (id == JVC_HUE) {
						return adj.hue;
					} else if (id == JVC_BRIGHTNESS) {
						return adj.brightness;
					}
				}
			}
				
			return 0;
		}

		virtual bool SetValue(jvideo_control_t id, int value)
		{
			jthread::AutoLock lock(&_player->_mutex);

			if (_player->_provider != NULL) {
				if (HasControl(id) == true) {
					DFBColorAdjustment adj;

					adj.contrast = value;

					if (id == JVC_CONTRAST) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_CONTRAST);
					} else if (id == JVC_SATURATION) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_SATURATION);
					} else if (id == JVC_HUE) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_HUE);
					} else if (id == JVC_BRIGHTNESS) {
						adj.flags = (DFBColorAdjustmentFlags)(DCAF_BRIGHTNESS);
					}
					
					_player->_provider->SetColorAdjustment(_player->_provider, &adj);

					return true;
				}
			}

			return false;
		}

		virtual void Reset(jvideo_control_t id)
		{
			SetValue(id, _default_values[id]);
		}

};

}

DirectFBLightPlayer::DirectFBLightPlayer(std::string file):
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
	_frames_per_second = 0.0;

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

	_aspect = (double)sdsc.width/(double)sdsc.height;

	char tmp[256];

	sprintf(tmp, "%d", mdsc.year);

	_media_info.title = std::string(mdsc.title);
	_media_info.author = std::string(mdsc.author);
	_media_info.album = std::string(mdsc.album);
	_media_info.genre = std::string(mdsc.genre);
	_media_info.comments = std::string(mdsc.comment);
	_media_info.date = std::string(tmp);

	if (mdsc.caps & DVSCAPS_AUDIO) {
		_has_audio = true;

		_controls.push_back(new directfblightplayer::VolumeControlImpl(this));
		_controls.push_back(new directfblightplayer::AudioConfigurationControlImpl(this));
	}

	if (mdsc.caps & DVSCAPS_VIDEO) {
		_has_video = true;
		_aspect = mdsc.video.aspect;
		_frames_per_second = mdsc.video.framerate;

		_controls.push_back(new directfblightplayer::VideoSizeControlImpl(this));
		_controls.push_back(new directfblightplayer::VideoFormatControlImpl(this));
		_controls.push_back(new directfblightplayer::VideoDeviceControlImpl(this));
	}

	_component = new directfblightplayer::DirectFBLightComponentImpl(this, 0, 0, sdsc.width, sdsc.height);

	Start();
}

DirectFBLightPlayer::~DirectFBLightPlayer()
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

void DirectFBLightPlayer::Callback(void *ctx)
{
	reinterpret_cast<directfblightplayer::DirectFBLightComponentImpl *>(ctx)->UpdateComponent();
}
		
void DirectFBLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		IDirectFBSurface *surface = dynamic_cast<directfblightplayer::DirectFBLightComponentImpl *>(_component)->_surface;

		if (_has_video == true) {
			_provider->PlayTo(_provider, surface, NULL, DirectFBLightPlayer::Callback, (void *)_component);
		} else {
			_provider->PlayTo(_provider, surface, NULL, NULL, NULL);
		}

		// usleep(500000);
	}
}

void DirectFBLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void DirectFBLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;

		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void DirectFBLightPlayer::Stop()
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

void DirectFBLightPlayer::Close()
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

void DirectFBLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		_provider->SeekTo(_provider, (double)time/1000.0);
	}
}

uint64_t DirectFBLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetPos(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

uint64_t DirectFBLightPlayer::GetMediaTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetLength(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

void DirectFBLightPlayer::SetLoop(bool b)
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

bool DirectFBLightPlayer::IsLoop()
{
	return _is_loop;
}

void DirectFBLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	if (rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != NULL) {
		_provider->SetSpeed(_provider, rate);
	}
}

double DirectFBLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		_provider->GetSpeed(_provider, &rate);
	}

	return rate;
}

jgui::Component * DirectFBLightPlayer::GetVisualComponent()
{
	return _component;
}

void DirectFBLightPlayer::Run()
{
	while (_is_closed == false) {
		_events->WaitForEventWithTimeout(_events, 0, 100);

		while (_events->HasEvent(_events) == DFB_OK) {
			DFBVideoProviderEvent event;

			_events->GetEvent(_events, DFB_EVENT(&event));

			if (event.clazz == DFEC_VIDEOPROVIDER) {
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
