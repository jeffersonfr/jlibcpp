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
#include "libvlclightplayer.h"

#include "jmedia/jvideosizecontrol.h"
#include "jmedia/jvideoformatcontrol.h"
#include "jmedia/jvolumecontrol.h"
#include "jmedia/jaudioconfigurationcontrol.h"
#include "jgui/jbufferedimage.h"
#include "jexception/jmediaexception.h"
#include "jexception/jcontrolexception.h"

#include <vlc/vlc.h>

#include <cairo.h>

namespace jmedia {

static libvlc_event_type_t mi_events[] = {
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
	
static int mi_events_len = sizeof(mi_events)/sizeof(*mi_events);

class LibvlcPlayerComponentImpl : public jgui::Component {

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
    jgui::Image **_buffer;
		/** \brief */
		int _buffer_index;
		/** \brief */
		jgui::jsize_t _frame_size;

	public:
		LibvlcPlayerComponentImpl(Player *player, int x, int y, int w, int h):
			jgui::Component(x, y, w, h)
		{
			_buffer = new jgui::Image*[2];
			
			_buffer[0] = new jgui::BufferedImage(jgui::JPF_RGB32, w, h);
			_buffer[1] = new jgui::BufferedImage(jgui::JPF_RGB32, w, h);

			_buffer_index = 0;

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

		virtual ~LibvlcPlayerComponentImpl()
		{
      if (_surface != nullptr) {
        cairo_surface_destroy(_surface);
      }

			if (_buffer != nullptr) {
				delete _buffer[0];
				delete _buffer[1];

				delete [] _buffer;
				_buffer = nullptr;
			}
		}

		virtual jgui::jsize_t GetPreferredSize()
		{
			return _frame_size;
		}

		virtual void UpdateComponent()
		{
      Repaint();
		}

		virtual void Paint(jgui::Graphics *g)
		{
			// jgui::Component::Paint(g);

      jgui::jsize_t
        size = GetSize();

      jgui::Image *image = _buffer[(_buffer_index + 1)%2];

      image->LockData();

			_player->DispatchFrameGrabberEvent(new jevent::FrameGrabberEvent(image, jevent::JFE_GRABBED));

	    g->SetAntialias(jgui::JAM_NONE);

      if (_src.x == 0 and _src.y == 0 and _src.width == _frame_size.width and _src.height == _frame_size.height) {
			  g->DrawImage(image, 0, 0, size.width, size.height);
      } else {
			  g->DrawImage(image, _src.x, _src.y, _src.width, _src.height, 0, 0, size.width, size.height);
      }
      
      image->UnlockData();
		}

		virtual Player * GetPlayer()
		{
			return _player;
		}

};

static void * LockMediaSurface(void *data, void **p_pixels)
{
	LibvlcPlayerComponentImpl *cmp = reinterpret_cast<LibvlcPlayerComponentImpl *>(data);
	
  jgui::Image *image = cmp->_buffer[(cmp->_buffer_index)%2];

	(*p_pixels) = image->LockData();

	return nullptr; 
}

static void UnlockMediaSurface(void *data, void *id, void *const *p_pixels)
{
	LibvlcPlayerComponentImpl *cmp = reinterpret_cast<LibvlcPlayerComponentImpl *>(data);

	jgui::Image *image = cmp->_buffer[(cmp->_buffer_index++)%2];

	image->UnlockData();
}

static void DisplayMediaSurface(void *data, void *id)
{
	reinterpret_cast<LibvlcPlayerComponentImpl *>(data)->UpdateComponent();
}

static void MediaEventsCallback(const libvlc_event_t *event, void *data)
{
	LibVLCLightPlayer	*player = reinterpret_cast<LibVLCLightPlayer *>(data);

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
		player->DispatchPlayerEvent(new jevent::PlayerEvent(player, jevent::JPE_STARTED));
	// } else if (event->type == libvlc_MediaPlayerPaused) {
	} else if (event->type == libvlc_MediaPlayerStopped) {
		player->DispatchPlayerEvent(new jevent::PlayerEvent(player, jevent::JPE_STOPPED));
	//} else if (event->type == libvlc_MediaPlayerForward) {
	//} else if (event->type == libvlc_MediaPlayerBackward) {
	} else if (event->type == libvlc_MediaPlayerEndReached) {
		player->Play();
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

class LibvlcVolumeControlImpl : public VolumeControl {
	
	private:
		/** \brief */
		LibVLCLightPlayer *_player;
		/** \brief */
		int _level;

	public:
		LibvlcVolumeControlImpl(LibVLCLightPlayer *player):
			VolumeControl()
		{
			_player = player;
			_level = 50;

			SetLevel(100);
		}

		virtual ~LibvlcVolumeControlImpl()
		{
		}

		virtual int GetLevel()
		{
			int level = 0;

			if (_player->_provider != nullptr) {
				level = libvlc_audio_get_volume(_player->_provider);
			}

			return level;
		}

		virtual void SetLevel(int level)
		{
			_level = level;

			if (_level <= 0) {
				_level = 0;
			} else {
				if (_level > 100) {
					_level = 100;
				}
			}

			if (_player->_provider != nullptr) {
				libvlc_audio_set_volume(_player->_provider, _level);
			}
		}
		
		virtual bool IsMute()
		{
			if (_player->_provider != nullptr) {
        return libvlc_audio_get_mute(_player->_provider) != 0;
      }

      return false;
		}

		virtual void SetMute(bool mute)
		{
			if (_player->_provider != nullptr) {
				libvlc_audio_set_mute(_player->_provider, (mute == true)?1:0);
			}
		}

};

class LibvlcAudioConfigurationControlImpl : public AudioConfigurationControl {
	
	private:
		/** \brief */
		LibVLCLightPlayer *_player;

	public:
		LibvlcAudioConfigurationControlImpl(LibVLCLightPlayer *player):
			AudioConfigurationControl()
		{
			_player = player;
		}

		virtual ~LibvlcAudioConfigurationControlImpl()
		{
		}

		virtual void SetAudioDelay(int64_t delay)
		{
			if (_player->_provider != nullptr) {
				 libvlc_audio_set_delay(_player->_provider, delay);
			}
		}

		virtual int64_t GetAudioDelay()
		{
			int64_t delay = 0LL;

			if (_player->_provider != nullptr) {
				 delay = libvlc_audio_get_delay(_player->_provider);
			}

			return delay;
		}

};

class LibvlcVideoSizeControlImpl : public VideoSizeControl {
	
	private:
		LibVLCLightPlayer *_player;

	public:
		LibvlcVideoSizeControlImpl(LibVLCLightPlayer *player):
			VideoSizeControl()
		{
			_player = player;
		}

		virtual ~LibvlcVideoSizeControlImpl()
		{
		}

		virtual void SetSource(int x, int y, int w, int h)
		{
			LibvlcPlayerComponentImpl *impl = dynamic_cast<LibvlcPlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);
			
			impl->_src.x = x;
			impl->_src.y = y;
			impl->_src.width = w;
			impl->_src.height = h;
		}

		virtual void SetDestination(int x, int y, int w, int h)
		{
			LibvlcPlayerComponentImpl *impl = dynamic_cast<LibvlcPlayerComponentImpl *>(_player->_component);

      std::unique_lock<std::mutex> lock(impl->_mutex);

			impl->SetBounds(x, y, w, h);
		}

		virtual jgui::jregion_t GetSource()
		{
			return dynamic_cast<LibvlcPlayerComponentImpl *>(_player->_component)->_src;
		}

		virtual jgui::jregion_t GetDestination()
		{
			return dynamic_cast<LibvlcPlayerComponentImpl *>(_player->_component)->GetVisibleBounds();
		}

};

class LibvlcVideoFormatControlImpl : public VideoFormatControl {
	
	private:
		LibVLCLightPlayer *_player;
		jvideo_mode_t _video_mode;
		jhd_video_format_t _hd_video_format;
		jsd_video_format_t _sd_video_format;

	public:
		LibvlcVideoFormatControlImpl(LibVLCLightPlayer *player):
			VideoFormatControl()
		{
			_player = player;
		
			_video_mode = LVM_FULL;
			_hd_video_format = LHVF_1080p;
			_sd_video_format = LSVF_PAL_M;
		}

		virtual ~LibvlcVideoFormatControlImpl()
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

LibVLCLightPlayer::LibVLCLightPlayer(jnetwork::URL url):
	jmedia::Player()
{
	_file = url.GetPath();
	_is_paused = false;
	_is_loop = false;
	_is_closed = false;
	_has_audio = false;
	_has_video = false;
	_aspect = 1.0;
	_media_time = 0LL;
	_decode_rate = 1.0;
	_frames_per_second = 0.0;
	
	char const *vlc_argv[] = {
		"--vout=dummy"
	};
	int vlc_argc = sizeof(vlc_argv) / sizeof(*vlc_argv);

	libvlc_media_t *media;

	_engine = libvlc_new(vlc_argc, vlc_argv);

	media = libvlc_media_new_path(_engine, _file.c_str());

	_provider = libvlc_media_player_new_from_media(media);

 	libvlc_media_track_t **tracks = nullptr;
	uint32_t 
	  iw = 0,
	  ih = 0;
  int
    count;
  
	libvlc_media_parse_with_options(
      media, (libvlc_media_parse_flag_t)(libvlc_media_parse_local | libvlc_media_parse_network | libvlc_media_fetch_local | libvlc_media_fetch_network), 2000);

  count = libvlc_media_tracks_get(media, &tracks);

  if (count == 0) {
    std::string ext = _file.substr(_file.size() - 3);

    if (strcasecmp(ext.c_str(), "mp3") != 0 && 
        strcasecmp(ext.c_str(), "wav") != 0 && 
        strcasecmp(ext.c_str(), "ogg") != 0) {
      libvlc_audio_set_mute(_provider, 1);
      libvlc_media_player_play(_provider);
      libvlc_media_player_pause(_provider);

      // waits 5 seconds to get video size
      for (int i=0; i<50; i++) {
        libvlc_video_get_size(_provider, 0, &iw, &ih);

        if (iw > 0 && ih > 0) {
          break;
        }

        usleep(100000);
      }

      libvlc_media_player_stop(_provider);
      libvlc_audio_set_mute(_provider, 0);

      if (iw <= 0 || ih <= 0) {
        libvlc_media_player_release(_provider);
        libvlc_release(_engine);

        throw jexception::RuntimeException("Cannot retrive the size of media content");
      }

      _aspect = static_cast<float>(iw)/ih;

      _controls.push_back(new LibvlcVideoSizeControlImpl(this));
      _controls.push_back(new LibvlcVideoFormatControlImpl(this));
    }
  } else {
    for (int i=0; i<count; i++) {
      libvlc_media_track_t *t = tracks[i];

      if (t->i_type == libvlc_track_audio) {
        if (_has_audio == false) {
          _has_audio = true;

          _controls.push_back(new LibvlcVolumeControlImpl(this));
          _controls.push_back(new LibvlcAudioConfigurationControlImpl(this));
        }
      } else if (t->i_type == libvlc_track_video) {
        if (_has_video == false) {
          _has_video = true;

          _controls.push_back(new LibvlcVideoSizeControlImpl(this));
          _controls.push_back(new LibvlcVideoFormatControlImpl(this));
        }

        iw = t->video->i_width;
        ih = t->video->i_height;

        _aspect = static_cast<float>(t->video->i_sar_num)/t->video->i_sar_den;
        _frames_per_second = static_cast<float>(t->video->i_frame_rate_num)/t->video->i_frame_rate_den;
      }
    }
  }

	libvlc_media_tracks_release(tracks, count);

	_media_time = (uint64_t)libvlc_media_get_duration(media);

	_component = new LibvlcPlayerComponentImpl(this, 0, 0, iw, ih);

	libvlc_video_set_format(_provider, "RV32", iw, ih, iw*4);
	libvlc_video_set_callbacks(_provider, LockMediaSurface, UnlockMediaSurface, DisplayMediaSurface, _component);

	_media_info.title = std::string(libvlc_media_get_meta(media, libvlc_meta_Title)?:"");
	_media_info.author = std::string(libvlc_media_get_meta(media, libvlc_meta_Artist)?:"");
	_media_info.album = std::string(libvlc_media_get_meta(media, libvlc_meta_Album)?:"");
	_media_info.genre = std::string(libvlc_media_get_meta(media, libvlc_meta_Genre)?:"");
	_media_info.comments = std::string(libvlc_media_get_meta(media, libvlc_meta_Description)?:"");
	_media_info.date = std::string(libvlc_media_get_meta(media, libvlc_meta_Date)?:"");
	
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

	for (int i=0; i<mi_events_len; i++) {
		libvlc_event_attach(_event_manager, mi_events[i], MediaEventsCallback, this);
	}
}

LibVLCLightPlayer::~LibVLCLightPlayer()
{
	for (int i=0; i<mi_events_len; i++) {
		libvlc_event_detach(_event_manager, mi_events[i], MediaEventsCallback, this);
	}

	Close();
	
	delete _component;
	_component = nullptr;

	for (std::vector<Control *>::iterator i=_controls.begin(); i!=_controls.end(); i++) {
		Control *control = (*i);

		delete control;
	}

	_controls.clear();

	libvlc_release(_engine);
}

void LibVLCLightPlayer::Run()
{
	if (IsLoop() == true) {
		Stop();
		Play();
	} else {
		Stop();

		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_FINISHED));
	}
}

void LibVLCLightPlayer::Play()
{
	if (_is_paused == false && _provider != nullptr) {
		libvlc_media_player_play(_provider);
	}
}

void LibVLCLightPlayer::Pause()
{
	if (_is_paused == false && _provider != nullptr) {
		_is_paused = true;

		if (libvlc_media_player_can_pause(_provider) == true) {
			libvlc_media_player_set_pause(_provider, 1);
		}
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_PAUSED));
	}
}

void LibVLCLightPlayer::Resume()
{
	if (_is_paused == true && _provider != nullptr) {
		_is_paused = false;

		libvlc_media_player_set_pause(_provider, 0);
		
		DispatchPlayerEvent(new jevent::PlayerEvent(this, jevent::JPE_RESUMED));
	}
}

void LibVLCLightPlayer::Stop()
{
	if (_provider != nullptr) {
		libvlc_media_player_stop(_provider);

		if (_has_video == true) {
			_component->Repaint();
		}

		_is_paused = false;
	}
}

void LibVLCLightPlayer::Close()
{
	if (_is_closed == true) {
		return;
	}

	_is_closed = true;

	if (_provider != nullptr) {
		// INFO:: cause crashes when release and create players
		// _events->Release(_events);
		// _events = nullptr;

		libvlc_media_player_release(_provider);
		_provider = nullptr;

		libvlc_release(_engine);
		_engine = nullptr;
	}
}

void LibVLCLightPlayer::SetCurrentTime(uint64_t time)
{
	if (_provider != nullptr) {
		if (libvlc_media_player_is_seekable(_provider) == true) {
			libvlc_media_player_set_time(_provider, (libvlc_time_t)time);
		}
	}
}

uint64_t LibVLCLightPlayer::GetCurrentTime()
{
	uint64_t time = 0LL;

	if (_provider != nullptr) {
		time = (uint64_t)libvlc_media_player_get_time(_provider);
	}

	return time;
}

uint64_t LibVLCLightPlayer::GetMediaTime()
{
	return _media_time;
}

void LibVLCLightPlayer::SetLoop(bool b)
{
	_is_loop = b;
}

bool LibVLCLightPlayer::IsLoop()
{
	return _is_loop;
}

void LibVLCLightPlayer::SetDecodeRate(double rate)
{
	_decode_rate = rate;

	if (_decode_rate != 0.0) {
		_is_paused = false;
	}

	if (_provider != nullptr) {
		libvlc_media_player_set_rate(_provider, (float)rate);
	}
}

double LibVLCLightPlayer::GetDecodeRate()
{
	double rate = 1.0;

	if (_provider != nullptr) {
		rate = (double)libvlc_media_player_get_rate(_provider);
	}

	return rate;
}

jgui::Component * LibVLCLightPlayer::GetVisualComponent()
{
	return _component;
}

}
