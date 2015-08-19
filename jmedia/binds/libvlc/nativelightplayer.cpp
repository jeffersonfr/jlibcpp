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

#include <cairo.h>

namespace jmedia {

libvlc_event_type_t mi_events[] = {
	// libvlc_MediaMetaChanged,	
	// libvlc_MediaSubItemAdded,	
	// libvlc_MediaDurationChanged,
	// libvlc_MediaParsedChanged,
	// libvlc_MediaFreed,
	// libvlc_MediaStateChanged,	
	// libvlc_MediaSubItemTreeAdded,	
	// libvlc_MediaPlayerMediaChanged,
	// libvlc_MediaPlayerNothingSpecial,
	// libvlc_MediaPlayerOpening,	
	// libvlc_MediaPlayerBuffering,
	libvlc_MediaPlayerPlaying,	
	libvlc_MediaPlayerPaused,	
	libvlc_MediaPlayerStopped,	
	// libvlc_MediaPlayerForward,	
	// libvlc_MediaPlayerBackward,	
	libvlc_MediaPlayerEndReached,
	// libvlc_MediaPlayerEncounteredError,	
	// libvlc_MediaPlayerTimeChanged,
	// libvlc_MediaPlayerPositionChanged,
	// libvlc_MediaPlayerSeekableChanged,
	// libvlc_MediaPlayerPausableChanged,
	// libvlc_MediaPlayerTitleChanged,	
	// libvlc_MediaPlayerSnapshotTaken,
	// libvlc_MediaPlayerLengthChanged,
	// libvlc_MediaPlayerVout,
	// libvlc_MediaPlayerScrambledChanged,
	// libvlc_MediaPlayerESAdded,	
	// libvlc_MediaPlayerESDeleted,	
	// libvlc_MediaPlayerESSelected,
	// libvlc_MediaPlayerCorked,	
	// libvlc_MediaPlayerUncorked,
	// libvlc_MediaPlayerMuted,	
	// libvlc_MediaPlayerUnmuted,
	// libvlc_MediaPlayerAudioVolume,	
	// libvlc_MediaPlayerAudioDevice,
	// libvlc_MediaListItemAdded,	
	// libvlc_MediaListWillAddItem,	
	// libvlc_MediaListItemDeleted,
	// libvlc_MediaListWillDeleteItem,	
	// libvlc_MediaListEndReached,	
	// libvlc_MediaListViewItemAdded,
	// libvlc_MediaListViewWillAddItem,	
	// libvlc_MediaListViewItemDeleted,	
	// libvlc_MediaListViewWillDeleteItem,	
	// libvlc_MediaListPlayerPlayed,
	// libvlc_MediaListPlayerNextItemSet,	
	// libvlc_MediaListPlayerStopped,	
	// libvlc_MediaDiscovererStarted,	
	// libvlc_MediaDiscovererEnded,	
	// libvlc_VlmMediaAdded,	
	// libvlc_VlmMediaRemoved,
	// libvlc_VlmMediaChanged,
	// libvlc_VlmMediaInstanceStarted,	
	// libvlc_VlmMediaInstanceStopped,
	// libvlc_VlmMediaInstanceStatusInit,	
	// libvlc_VlmMediaInstanceStatusOpening,	
	// libvlc_VlmMediaInstanceStatusPlaying,	
	// libvlc_VlmMediaInstanceStatusPause,
	// libvlc_VlmMediaInstanceStatusEnd,	
	// libvlc_VlmMediaInstanceStatusError,
};
	
int mi_events_len = sizeof(mi_events)/sizeof(*mi_events);

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

void * LockMediaSurface(void *data, void **p_pixels)
{
	VideoLightweightImpl *cmp = reinterpret_cast<VideoLightweightImpl *>(data);
	
	(*p_pixels) = cmp->_buffer[cmp->_buffer_index];

	cmp->_buffer_index = (cmp->_buffer_index + 1)%2;

	return NULL; 
}

void UnlockMediaSurface(void *data, void *id, void *const *p_pixels)
{
}

void DisplayMediaSurface(void *data, void *id)
{
	reinterpret_cast<VideoLightweightImpl *>(data)->UpdateComponent();
}

void MediaEventsCallback(const libvlc_event_t *event, void *data)
{
	NativeLightPlayer	*player = reinterpret_cast<NativeLightPlayer *>(data);

	if (event->type == libvlc_MediaMetaChanged) {
	//} else if (event->type == libvlc_MediaSubItemAdded) {
	//} else if (event->type == libvlc_MediaDurationChanged) {
	//} else if (event->type == libvlc_MediaParsedChanged) {
	//} else if (event->type == libvlc_MediaFreed) {
	//} else if (event->type == libvlc_MediaStateChanged) {
	//} else if (event->type == libvlc_MediaSubItemTreeAdded) {
	//} else if (event->type == libvlc_MediaPlayerMediaChanged) {
	//} else if (event->type == libvlc_MediaPlayerNothingSpecial) {
	//} else if (event->type == libvlc_MediaPlayerOpening) {
	//} else if (event->type == libvlc_MediaPlayerBuffering) {
	} else if (event->type == libvlc_MediaPlayerPlaying) {
		player->DispatchPlayerEvent(new PlayerEvent(player, JPE_STARTED));
	// } else if (event->type == libvlc_MediaPlayerPaused) {
	} else if (event->type == libvlc_MediaPlayerStopped) {
		player->DispatchPlayerEvent(new PlayerEvent(player, JPE_STOPPED));
	//} else if (event->type == libvlc_MediaPlayerForward) {
	//} else if (event->type == libvlc_MediaPlayerBackward) {
	} else if (event->type == libvlc_MediaPlayerEndReached) {
		player->DispatchPlayerEvent(new PlayerEvent(player, JPE_FINISHED));
	// } else if (event->type == libvlc_MediaPlayerEncounteredError) {
	// } else if (event->type == libvlc_MediaPlayerTimeChanged) {
	// } else if (event->type == libvlc_MediaPlayerPositionChanged) {
	// } else if (event->type == libvlc_MediaPlayerSeekableChanged) {
	// } else if (event->type == libvlc_MediaPlayerPausableChanged) {
	// } else if (event->type == libvlc_MediaPlayerTitleChanged) {
	// } else if (event->type == libvlc_MediaPlayerSnapshotTaken) {
	// } else if (event->type == libvlc_MediaPlayerLengthChanged) {
	// } else if (event->type == libvlc_MediaPlayerVout) {
	// } else if (event->type == libvlc_MediaPlayerScrambledChanged) {
	// } else if (event->type == libvlc_MediaPlayerESAdded) {
	// } else if (event->type == libvlc_MediaPlayerESDeleted) {
	// } else if (event->type == libvlc_MediaPlayerESSelected) {
	// } else if (event->type == libvlc_MediaPlayerCorked) {
	// } else if (event->type == libvlc_MediaPlayerUncorked) {
	// } else if (event->type == libvlc_MediaPlayerMuted) {
	// } else if (event->type == libvlc_MediaPlayerUnmuted) {
	// } else if (event->type == libvlc_MediaPlayerAudioVolume) {
	// } else if (event->type == libvlc_MediaPlayerAudioDevice) {
	// } else if (event->type == libvlc_MediaListItemAdded) {
	// } else if (event->type == libvlc_MediaListWillAddItem) {
	// } else if (event->type == libvlc_MediaListItemDeleted) {
	// } else if (event->type == libvlc_MediaListWillDeleteItem) {
	// } else if (event->type == libvlc_MediaListEndReached) {
	// } else if (event->type == libvlc_MediaListViewItemAdded) {
	// } else if (event->type == libvlc_MediaListViewWillAddItem) {
	// } else if (event->type == libvlc_MediaListViewItemDeleted) {
	// } else if (event->type == libvlc_MediaListViewWillDeleteItem) {
	// } else if (event->type == libvlc_MediaListPlayerPlayed) {
	// } else if (event->type == libvlc_MediaListPlayerNextItemSet) {
	// } else if (event->type == libvlc_MediaListPlayerStopped) {
	// } else if (event->type == libvlc_MediaDiscovererStarted) {
	// } else if (event->type == libvlc_MediaDiscovererEnded) {
	// } else if (event->type == libvlc_VlmMediaAdded) {
	// } else if (event->type == libvlc_VlmMediaRemoved) {
	// } else if (event->type == libvlc_VlmMediaChanged) {
	// } else if (event->type == libvlc_VlmMediaInstanceStarted) {
	// } else if (event->type == libvlc_VlmMediaInstanceStopped) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusInit) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusOpening) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusPlaying) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusPause) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusEnd) {
	// } else if (event->type == libvlc_VlmMediaInstanceStatusError) {
	}
}

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

			if (_player->_provider != NULL) {
				level = libvlc_audio_get_volume(_player->_provider);
			}

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

			if (_player->_provider != NULL) {
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
				libvlc_audio_set_volume(_player->_provider, _level);
			}
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
				libvlc_audio_set_mute(_player->_provider, (_is_muted == true)?1:0);
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

NativeLightPlayer::NativeLightPlayer(std::string file):
	jmedia::Player()
{
	_file = file;
	_is_paused = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 16.0/9.0;
	_media_time = 0LL;
	
	char const *vlc_argv[] = {
		"--vout=dummy"
	};
	int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

	libvlc_media_t *media;

	_engine = libvlc_new(vlc_argc, vlc_argv);
	media = libvlc_media_new_path(_engine, file.c_str());
	_provider = libvlc_media_player_new_from_media(media);

	libvlc_media_parse(media);

 	libvlc_media_track_t **tracks = NULL;
	int count = libvlc_media_tracks_get(media, &tracks);

	for (int i=0; i<count; i++) {
		libvlc_media_track_t *t = tracks[i];

		if (t->i_type == libvlc_track_audio) {
			if (_has_audio == false) {
				_has_audio = true;

				_controls.push_back(new VolumeControlImpl(this));
			}
		} else if (t->i_type == libvlc_track_video) {
			if (_has_video == false) {
				_has_video = true;
				
				_controls.push_back(new VideoSizeControlImpl(this));
			}
		}
	}

	libvlc_media_tracks_release(tracks, count);

	uint32_t iw;
	uint32_t ih;

	if (libvlc_video_get_size(_provider, 0, &iw, &ih) != 0) {
		throw jcommon::RuntimeException("Cannot retrive the size of media content");
	}

	if (iw <= 0 || ih <= 0) {
		throw jcommon::RuntimeException("Invalid size of media content");
	}

	if (_aspect == 0.0) {
		_aspect = (double)(iw/(double)ih);
	}

	_media_time = (uint64_t)libvlc_media_get_duration(media);
	// _frame_per_sec = libvlc_media_player_get_fps(_provider);

	_component = new VideoLightweightImpl(this, 0, 0, iw, ih);

	libvlc_video_set_format(_provider, "RV32", iw, ih, iw*4);
	libvlc_video_set_callbacks(_provider, LockMediaSurface, UnlockMediaSurface, DisplayMediaSurface, _component);

	char *str;

	str = libvlc_media_get_meta(media, libvlc_meta_Title);
	if (str != NULL) {
		_media_info.title = str;
	}

	str = libvlc_media_get_meta(media, libvlc_meta_Artist);
	if (str != NULL) {
		_media_info.author = str;
	}

	str = libvlc_media_get_meta(media, libvlc_meta_Album);
	if (str != NULL) {
		_media_info.album = str;
	}

	str = libvlc_media_get_meta(media, libvlc_meta_Genre);
	if (str != NULL) {
		_media_info.genre = str;
	}

	str = libvlc_media_get_meta(media, libvlc_meta_Description);
	if (str != NULL) {
		_media_info.comments = str;
	}

	_media_info.year = 1900;
	
	/*
	libvlc_meta_Copyright
	libvlc_meta_TrackNumber
	libvlc_meta_Rating
	libvlc_meta_Date
	libvlc_meta_Setting
	libvlc_meta_URL
	libvlc_meta_Language
	libvlc_meta_NowPlaying
	libvlc_meta_Publisher
	libvlc_meta_EncodedBy
	libvlc_meta_ArtworkURL
	libvlc_meta_TrackID
	libvlc_meta_TrackTotal
	libvlc_meta_Director
	libvlc_meta_Season
	libvlc_meta_Episode
	libvlc_meta_ShowName
	libvlc_meta_Actors
	libvlc_meta_AlbumArtist
	libvlc_meta_DiscNumber 
	*/

	libvlc_media_release(media);
	
	_event_manager = libvlc_media_player_event_manager(_provider);

	for (int i = 0; i < mi_events_len; i++) {
		libvlc_event_attach(_event_manager, mi_events[i], MediaEventsCallback, this);
	}
}

NativeLightPlayer::~NativeLightPlayer()
{
	for (int i=0; i<mi_events_len; i++) {
		libvlc_event_detach(_event_manager, mi_events[i], MediaEventsCallback, this);
	}

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

	if (_is_paused == false && _provider != NULL) {
		libvlc_media_player_play(_provider);
	}
}

void NativeLightPlayer::Pause()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == false) {
		_is_paused = true;

		if (libvlc_media_player_can_pause(_provider) == true) {
			libvlc_media_player_set_pause(_provider, 1);
		}
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_PAUSED));
	}
}

void NativeLightPlayer::Resume()
{
	jthread::AutoLock lock(&_mutex);

	if (_is_paused == true) {
		_is_paused = false;

		libvlc_media_player_set_pause(_provider, 0);
		
		DispatchPlayerEvent(new PlayerEvent(this, JPE_RESUMED));
	}
}

void NativeLightPlayer::Stop()
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		libvlc_media_player_stop(_provider);

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

	if (_provider != NULL) {
		// INFO:: cause crashes when release and create players
		// _events->Release(_events);
		// _events = NULL;

		libvlc_media_player_release(_provider);
		_provider = NULL;

		libvlc_release(_engine);
		_engine = NULL;
	}
}

void NativeLightPlayer::SetCurrentTime(uint64_t time)
{
	jthread::AutoLock lock(&_mutex);

	if (_provider != NULL) {
		if (libvlc_media_player_is_seekable(_provider) == true) {
			libvlc_media_player_set_time(_provider, (libvlc_time_t)time);
		}
	}
}

uint64_t NativeLightPlayer::GetCurrentTime()
{
	jthread::AutoLock lock(&_mutex);

	uint64_t time = 0LL;

	if (_provider != NULL) {
		time = (uint64_t)libvlc_media_player_get_time(_provider);
	}

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

	if (_provider != NULL) {
		libvlc_media_player_set_rate(_provider, (float)rate);
	}
}

double NativeLightPlayer::GetDecodeRate()
{
	jthread::AutoLock lock(&_mutex);

	double rate = 1.0;

	if (_provider != NULL) {
		rate = (double)libvlc_media_player_get_rate(_provider);
	}

	return rate;
}

jgui::Component * NativeLightPlayer::GetVisualComponent()
{
	return _component;
}

}
