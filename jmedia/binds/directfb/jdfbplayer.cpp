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
#include "jdfbplayer.h"
#include "jcontrolexception.h"
#include "jvideosizecontrol.h"
#include "jvideoformatcontrol.h"
#include "jvolumecontrol.h"
#include "jmediaexception.h"
#include "jimage.h"
#include "jgfxhandler.h"

namespace jmedia {

class VideoComponentImpl : public jgui::Component, jthread::Thread {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		jthread::Mutex _mutex;
		/** \brief */
		jgui::Image *_image;
		/** \brief */
		jgui::jregion_t _src;
		/** \brief */
		jgui::jregion_t _dst;
		/** \brief */
		bool _diff;

	public:
		VideoComponentImpl(Player *player, int x, int y, int w, int h, int iw, int ih):
			jgui::Component(x, y, w, h),
			_mutex(jthread::JMT_RECURSIVE)
		{
			_player = player;

			_image = jgui::Image::CreateImage(w, h, jgui::JPF_ARGB, iw, ih);

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

		virtual ~VideoComponentImpl()
		{
			if (IsRunning() == true) {
				WaitThread();
			}

			delete _image;
		}

		virtual void UpdateComponent()
		{
			_player->DispatchFrameEvent(new FrameEvent(_player, JFE_GRABBED, _image));

			if (IsRunning() == true) {
				WaitThread();
			}

			Start();
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

			if (_diff == false) {
				g->DrawImage(_image, 0, 0, GetWidth(), GetHeight());
			} else {
				g->DrawImage(_image, _src.x, _src.y, _src.width, _src.height, _dst.x, _dst.y, _dst.width, _dst.height);
			}
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

		virtual jgui::Image * GetFrame()
		{
			return _image;
		}

};

class VolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		DFBPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		VolumeControlImpl(DFBPlayer *player):
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
		
		virtual bool IsMuted()
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
		DFBPlayer *_player;

	public:
		VideoSizeControlImpl(DFBPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~VideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			jthread::AutoLock lock(&_player->_component->_mutex);
			
			_player->_component->_src.x = x;
			_player->_component->_src.y = y;
			_player->_component->_src.width = w;
			_player->_component->_src.height = h;
			
			_player->_component->_diff = false;

			if (_player->_component->_src.x != _player->_component->_dst.x ||
					_player->_component->_src.y != _player->_component->_dst.y ||
					_player->_component->_src.width != _player->_component->_dst.width ||
					_player->_component->_src.height != _player->_component->_dst.height) {
				_player->_component->_diff = true;
			}
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			_player->_component->_dst.x = x;
			_player->_component->_dst.y = y;
			_player->_component->_dst.width = w;
			_player->_component->_dst.height = h;
			
			_player->_component->_diff = false;

			if (_player->_component->_src.x != _player->_component->_dst.x ||
					_player->_component->_src.y != _player->_component->_dst.y ||
					_player->_component->_src.width != _player->_component->_dst.width ||
					_player->_component->_src.height != _player->_component->_dst.height) {
				_player->_component->_diff = true;
			}
		}

		virtual jgui::jregion_t GetSource()
		{
			return _player->_component->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return _player->_component->_dst;
		}

};

class VideoFormatControlImpl : public VideoFormatControl {
	
	private:
		DFBPlayer *_player;

	public:
		VideoFormatControlImpl(DFBPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		}

		virtual ~VideoFormatControlImpl()
		{
		}

		virtual void SetAspectRatio(jaspect_ratio_t t)
		{
		}

		virtual void SetContentMode(jvideo_mode_t t)
		{
		}

		virtual void SetVideoFormatHD(jhd_video_format_t vf)
		{
		}

		virtual void SetVideoFormatSD(jsd_video_format_t vf)
		{
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
			} else if (aspect == (2.0/3.0)) {
				return LAR_2x3;
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

DFBPlayer::DFBPlayer(std::string file):
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

	_component = new VideoComponentImpl(this, 0, 0, sdsc.width, sdsc.height, sdsc.width, sdsc.height);

	Start();
}

DFBPlayer::~DFBPlayer()
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

void DFBPlayer::Callback(void *ctx)
{
	reinterpret_cast<VideoComponentImpl *>(ctx)->UpdateComponent();
}
		
void DFBPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false && _provider != NULL) {
		jgui::Graphics *graphics = dynamic_cast<VideoComponentImpl *>(_component)->GetFrame()->GetGraphics();

		if (_has_video == true) {
			_provider->PlayTo(_provider, (IDirectFBSurface *)graphics->GetNativeSurface(), NULL, DFBPlayer::Callback, (void *)_component);
		} else {
			_provider->PlayTo(_provider, (IDirectFBSurface *)graphics->GetNativeSurface(), NULL, NULL, NULL);
		}

		usleep(500000);
	}
}

void DFBPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;
		_decode_rate = GetDecodeRate();

		SetDecodeRate(0.0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void DFBPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;

		SetDecodeRate(_decode_rate);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void DFBPlayer::Stop()
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

void DFBPlayer::Close()
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

void DFBPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		_provider->SeekTo(_provider, (double)time/1000.0);
	}
}

uint64_t DFBPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetPos(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

uint64_t DFBPlayer::GetMediaTime()
{
	jthread::AutoLock lock(&_mutex);

	double time = 0.0;

	if (_provider != NULL) {
		_provider->GetLength(_provider, &time);
	}

	return (uint64_t)(time*1000LL);
}

void DFBPlayer::SetLoop(bool b)
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

bool DFBPlayer::IsLoop()
{
	return _is_loop;
}

void DFBPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	if (rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != NULL) {
		_provider->SetSpeed(_provider, rate);
	}
}

double DFBPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		_provider->GetSpeed(_provider, &rate);
	}

	return rate;
}

jgui::Component * DFBPlayer::GetVisualComponent()
{
	return _component;
}

void DFBPlayer::Run()
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
