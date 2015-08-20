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
#include "jgfxhandler.h"
#include "genericimage.h"
#include "avplay.h"

#include <cairo.h>

namespace jmedia {

class VideoLightweightImpl : public jgui::Component, jthread::Thread {

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
		uint32_t **_buffer;
		/** \brief */
		int _buffer_index;
		/** \brief */
		int _media_width;
		/** \brief */
		int _media_height;
		/** \brief */
		bool _diff;

	public:
		VideoLightweightImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = new uint32_t*[2];
			
			_buffer[0] = new uint32_t[w*h];
			_buffer[1] = new uint32_t[w*h];

			_buffer_index = 0;

			_image = NULL;
			_player = player;
			
			_media_width = w;
			_media_height = h;

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

			if (_buffer != NULL) {
				delete [] _buffer[0];
				delete [] _buffer[1];

				delete _buffer;
				_buffer = NULL;
			}

			_mutex.Unlock();
		}

		virtual void UpdateComponent()
		{
			if (IsRunning() == true) {
				WaitThread();
			}

			int sw = _media_width;
			int sh = _media_height;

			cairo_surface_t *cairo_surface = cairo_image_surface_create_for_data(
					(uint8_t *)_buffer[(_buffer_index+1)%2], CAIRO_FORMAT_ARGB32, sw, sh, cairo_format_stride_for_width(CAIRO_FORMAT_ARGB32, sw));
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
			
			/*
			if (_player->_provider != NULL) {
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
			}
			*/
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

VideoState *_provider = NULL;

NativeLightPlayer::NativeLightPlayer(std::string file):
	jmedia::Player()
{
	// player->DispatchPlayerEvent(new PlayerEvent(player, JPE_STARTED));
	// player->DispatchPlayerEvent(new PlayerEvent(player, JPE_STOPPED));
	// player->DispatchPlayerEvent(new PlayerEvent(player, JPE_FINISHED));

	_file = file;
	_is_paused = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 16.0/9.0;
	_media_time = 0LL;
	
	avplay_init();

	_provider = avplay_open(_file.c_str());

	_component = new VideoLightweightImpl(this, 0, 0, 100, 100);//iw, ih);
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

void NativeLightPlayer::Play()
{
	jthread::AutoLock lock(&_mutex);

	/*
	if (_is_paused == false && _provider != NULL) {
		libvlc_media_player_play(_provider);
	}
	*/
}

void NativeLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	/*
	if (_is_paused == false) {
		_is_paused = true;

		if (libvlc_media_player_can_pause(_provider) == true) {
			libvlc_media_player_set_pause(_provider, 1);
		}
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
	*/
}

void NativeLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	/*
	if (_is_paused == true) {
		_is_paused = false;

		libvlc_media_player_set_pause(_provider, 0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
	*/
}

void NativeLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	/*
	if (_provider != NULL) {
		libvlc_media_player_stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
	*/
}

void NativeLightPlayer::Close()
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

void NativeLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	/*
	if (_provider != NULL) {
		if (libvlc_media_player_is_seekable(_provider) == true) {
			libvlc_media_player_set_time(_provider, (libvlc_time_t)time);
		}
	}
	*/
}

uint64_t NativeLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	uint64_t time = 0LL;

	/*
	if (_provider != NULL) {
		time = (uint64_t)libvlc_media_player_get_time(_provider);
	}
	*/

	return time;
}

uint64_t NativeLightPlayer::GetMediaTime()
{
	return _media_time;
}

void NativeLightPlayer::SetLoop(bool b)
{
	jthread::AutoLock lock(&_mutex);

	_is_loop = b;
}

bool NativeLightPlayer::IsLoop()
{
	return _is_loop;
}

void NativeLightPlayer::SetDecodeRate(double rate)
{
	jthread::AutoLock lock(&_mutex);

	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	/*
	if (_provider != NULL) {
		libvlc_media_player_set_rate(_provider, (float)rate);
	}
	*/
}

double NativeLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	/*
	if (_provider != NULL) {
		rate = (double)libvlc_media_player_get_rate(_provider);
	}
	*/

	return rate;
}

jgui::Component * NativeLightPlayer::GetVisualComponent()
{
	return _component;
}

}
