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
#include "libavlightplayer.h"
#include "libavplay.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvolumecontrol.h"
#include "jgui/jbufferedimage.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#include <cairo.h>

namespace jmedia {

class LibavPlayerComponentImpl : public jgui::Component {

	public:
		/** \brief */
		Player *_player;
		/** \brief */
		cairo_surface_t *_surface;
		/** \brief */
    std::mutex _mutex;
		/** \brief */
		jgui::jregion_t _src;
		/** \brief */
		jgui::jregion_t _dst;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		LibavPlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_surface = nullptr;
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

		virtual ~LibavPlayerComponentImpl()
		{
      if (_surface != nullptr) {
        cairo_surface_destroy(_surface);
      }
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

			int sw = width;
			int sh = height;

			_mutex.lock();

			_surface = cairo_image_surface_create_for_data(
					(uint8_t *)buffer, CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
			
      _mutex.unlock();

      Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			jgui::Component::Paint(g);

      jgui::jsize_t
        size = GetSize();

			_mutex.lock();

      if (_surface == nullptr) {
        _mutex.unlock();

        return;
      }

			cairo_t *context = cairo_create(_surface);
      jgui::Image *image = new jgui::BufferedImage(context);

			_player->DispatchFrameGrabberEvent(new jevent::FrameGrabberEvent(image, jevent::JFE_GRABBED));

			cairo_surface_mark_dirty(_surface);

			g->DrawImage(image, _src.x, _src.y, _src.width, _src.height, 0, 0, size.width, size.height);

      delete image;
      image = nullptr;

			cairo_surface_destroy(_surface);

      _surface = nullptr;

			_mutex.unlock();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

class LibavVolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibAVLightPlayer *_player;
		/** \brief */
		int _level;
		/** \brief */
		bool _is_muted;

	public:
		LibavVolumeControlImpl(LibAVLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;
			_is_muted = false;

			SetLevel(100);
		}

		virtual ~LibavVolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			int level = 0;

			/*
			if (_player->_provider != nullptr) {
				level = libvlc_audio_get_volume(_player->_provider);
			}
			*/

			return level;
		}

		virtual void SetLevel(int level)
		{
			_level = (level < 0)?0:(level > 100)?100:level;

			if (_level != 0) {
				_is_muted = true;
			} else {
				_is_muted = false;
			}

			/*
			if (_player->_provider != nullptr) {
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
      std::unique_lock<std::mutex> lock(_player->_mutex);
	
			_is_muted = b;
			
			if (_player->_provider != nullptr) {
				avplay_mute(_player->_provider, _is_muted);
			}
		}

};

class LibavVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibAVLightPlayer *_player;

	public:
		LibavVideoSizeControlImpl(LibAVLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~LibavVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			LibavPlayerComponentImpl *impl = dynamic_cast<LibavPlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			LibavPlayerComponentImpl *impl = dynamic_cast<LibavPlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<LibavPlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<LibavPlayerComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class LibavVideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibAVLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		LibavVideoFormatControlImpl(LibAVLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~LibavVideoFormatControlImpl()
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
      std::unique_lock<std::mutex> lock(_player->_mutex);

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
      std::unique_lock<std::mutex> lock(_player->_mutex);

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
	reinterpret_cast<LibavPlayerComponentImpl *>(data)->UpdateComponent(buffer, width, height);
}

static void endofmedia_callback(void *data)
{
	LibAVLightPlayer *player = reinterpret_cast<LibAVLightPlayer *>(data);
	
	player->DispatchPlayerEvent(new jevent::PlayerEvent(player, jevent::JPE_FINISHED));
}

LibAVLightPlayer::LibAVLightPlayer(jnetwork::URL url):
	jmedia::Player()
{
	_file = url.GetPath();
	_is_paused = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	
	avplay_init();

	_provider = avplay_open(_file.c_str());

	if (_provider == nullptr) {
		throw jexception::MediaException("Cannot recognize the media file");
	}

	_component = new LibavPlayerComponentImpl(this, 0, 0, -1, -1);//iw, ih);

	avplay_set_rendercallback(_provider, render_callback, (void *)_component);
	avplay_set_endofmediacallback(_provider, endofmedia_callback, (void *)this);
		
	if (_provider->wanted_stream[AVMEDIA_TYPE_AUDIO] != -1) {
		_controls.push_back(new LibavVolumeControlImpl(this));
	}
	
	if (_provider->wanted_stream[AVMEDIA_TYPE_VIDEO] != -1) {
		_controls.push_back(new LibavVideoSizeControlImpl(this));
		_controls.push_back(new LibavVideoFormatControlImpl(this));
	}
}

LibAVLightPlayer::~LibAVLightPlayer()
{
	Close();
	
	delete _component;
	_component = nullptr;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();

  delete _provider;
  _provider = nullptr;
}

void LibAVLightPlayer::Play()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _provider != nullptr) {
		avplay_play(_provider);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_STARTED));
	}
}

void LibAVLightPlayer::Pause()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == false && _provider != nullptr) {
		_is_paused = true;
		
		avplay_pause(_provider, true);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_PAUSED));
	}
}

void LibAVLightPlayer::Resume()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_paused == true && _provider != nullptr) {
		_is_paused = false;
		
		avplay_pause(_provider, false);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_RESUMED));
	}
}

void LibAVLightPlayer::Stop()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		avplay_stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibAVLightPlayer::Close()
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_provider != nullptr) {
		avplay_close(_provider);

		_provider = nullptr;
	}
}

void LibAVLightPlayer::SetCurrentTime(uint64_t time)
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		avplay_setcurrentmediatime(_provider, time);
	}
}

uint64_t LibAVLightPlayer::GetCurrentTime()
{
	uint64_t time = 0LL;

  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		time = (uint64_t)avplay_getcurrentmediatime(_provider);
	}

	return time;
}

uint64_t LibAVLightPlayer::GetMediaTime()
{
	uint64_t time = 0LL;

	if (_provider != nullptr) {
		time = (uint64_t)avplay_getmediatime(_provider);
	}

	return time;
}

void LibAVLightPlayer::SetLoop(bool b)
{
  std::unique_lock<std::mutex> lock(_mutex);

	if (_provider != nullptr) {
		avplay_setloop(_provider, b);
	}
}

bool LibAVLightPlayer::IsLoop()
{
	if (_provider != nullptr) {
		return avplay_isloop(_provider);
	}

	return false;
}

void LibAVLightPlayer::SetDecodeRate(double rate)
{
  std::unique_lock<std::mutex> lock(_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != nullptr) {
		// libvlc_media_player_set_rate(_provider, (float)rate);
	}
}

double LibAVLightPlayer::GetDecodeRate()
{
  std::unique_lock<std::mutex> lock(_mutex);

	double rate = 1.0;

	if (_provider != nullptr) {
		// rate = (double)libvlc_media_player_get_rate(_provider);
	}

	return rate;
}

jgui::Component * LibAVLightPlayer::GetVisualComponent()
{
	return _component;
}

}
